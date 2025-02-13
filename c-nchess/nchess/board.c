/*
    board.c

    This file contains the function definitions for the board functions.
*/

#include "board.h"
#include "config.h"
#include "bitboard.h"
#include "generate_utils.h"
#include "utils.h"
#include "hash.h"
#include "board_utils.h"
#include "makemove.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NCH_STATIC_FINLINE void
_init_board_flags_and_states(Board* board){
    board->castles = Board_CASTLE_WK | Board_CASTLE_WQ | Board_CASTLE_BK | Board_CASTLE_WQ;

    board->en_passant_idx = 0;
    board->en_passant_map = 0ULL;
    board->en_passant_trg = 0ULL;
    board->flags = Board_TURN;
    board->nmoves = 0;
    board->fifty_counter = 0;
    board->captured_piece = NCH_NO_PIECE;
}

NCH_STATIC_FINLINE void
_init_board(Board* board){
    set_board_occupancy(board);
    init_piecetables(board);
    _init_board_flags_and_states(board);
}

NCH_STATIC_FINLINE Board*
new_board(){
    Board* board = malloc(sizeof(Board));
    if (!board){
        return NULL;
    }

    BoardDict_Init(&board->dict);
    MoveList_Init(&board->movelist);

    return board;
}

Board*
Board_New(){
    Board* board = new_board();
    if (!board)
        return NULL;
    
    Board_Init(board);
    return board;
}

Board*
Board_NewEmpty(){
    Board* board = new_board();
    if (!board)
        return NULL;
    
    Board_InitEmpty(board);
    return board;
}

void
Board_Free(Board* board){
    if (board){
        BoardDict_Free(&board->dict);
        MoveList_Free(&board->movelist);
        free(board);
    }
}

void
Board_Init(Board* board){
    board->bitboards[NCH_White][NCH_Pawn]   = NCH_BOARD_W_PAWNS_STARTPOS;
    board->bitboards[NCH_White][NCH_Knight] = NCH_BOARD_W_KNIGHTS_STARTPOS;
    board->bitboards[NCH_White][NCH_Bishop] = NCH_BOARD_W_BISHOPS_STARTPOS;
    board->bitboards[NCH_White][NCH_Rook]   = NCH_BOARD_W_ROOKS_STARTPOS;
    board->bitboards[NCH_White][NCH_Queen]  = NCH_BOARD_W_QUEEN_STARTPOS;
    board->bitboards[NCH_White][NCH_King]   = NCH_BOARD_W_KING_STARTPOS;

    board->bitboards[NCH_Black][NCH_Pawn]   = NCH_BOARD_B_PAWNS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Knight] = NCH_BOARD_B_KNIGHTS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Bishop] = NCH_BOARD_B_BISHOPS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Rook]   = NCH_BOARD_B_ROOKS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Queen]  = NCH_BOARD_B_QUEEN_STARTPOS;
    board->bitboards[NCH_Black][NCH_King]   = NCH_BOARD_B_KING_STARTPOS;

    _init_board(board);

    // starting position does not cause a check to any side but it is better to call
    // the update_check function to make sure that the board is in a valid state.
    update_check(board);
}

void
Board_InitEmpty(Board* board){
    board->bitboards[NCH_White][NCH_Pawn] = 0ULL;
    board->bitboards[NCH_White][NCH_Knight] = 0ULL;
    board->bitboards[NCH_White][NCH_Bishop] = 0ULL;
    board->bitboards[NCH_White][NCH_Rook] = 0ULL;
    board->bitboards[NCH_White][NCH_Queen] = 0ULL;
    board->bitboards[NCH_White][NCH_King] = 0ULL;

    board->bitboards[NCH_Black][NCH_Pawn] = 0ULL;
    board->bitboards[NCH_Black][NCH_Knight] = 0ULL;
    board->bitboards[NCH_Black][NCH_Bishop] = 0ULL;
    board->bitboards[NCH_Black][NCH_Rook] = 0ULL;
    board->bitboards[NCH_Black][NCH_Queen] = 0ULL;
    board->bitboards[NCH_Black][NCH_King] = 0ULL;

    _init_board(board);

    // the _init_board sets the castles to the initial state and here we reset them
    // because the board is empty. Considereblt this is not the best way to do this
    // setting the value of the same variable twice like this but it is the easiest way
    // for now and it a new desing would be implemented in the future.
    board->castles = 0;
}

int
Board_IsCheck(const Board* board){
    return get_checkmap(
            board,
            Board_IS_WHITETURN(board) ? NCH_White : NCH_Black,
            NCH_SQRIDX( Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
            Board_ALL_OCC(board)
        ) != 0ULL;
}

void
Board_Reset(Board* board){
    BoardDict_Reset(&board->dict);
    MoveList_Reset(&board->movelist);
    Board_Init(board);
}

int
Board_IsInsufficientMaterial(const Board* board){
    // if there are pawns, rooks or queens on the board then it is not
    // in a state of insufficient material.
    uint64 enough = Board_WHITE_QUEENS(board)
                  | Board_BLACK_QUEENS(board)
                  | Board_WHITE_PAWNS(board)
                  | Board_BLACK_PAWNS(board)
                  | Board_WHITE_ROOKS(board)
                  | Board_BLACK_ROOKS(board);

    if (enough)
        return 0;

    uint64 bishops = Board_WHITE_BISHOPS(board)
                   | Board_BLACK_BISHOPS(board);

    uint64 knights = Board_WHITE_KNIGHTS(board)
                    | Board_BLACK_KNIGHTS(board); 
    
    if (!bishops){
        // if there are no bishops on the board there is two ways to be not it a state of
        // insufficient material.
        // if there are more then two knights and if there are two knights but not on the
        // same color.
        if (more_then_two(knights) || (Board_WHITE_KNIGHTS(board) && Board_BLACK_KNIGHTS(board)))
            return 0;
        return 1;
    }

    if (!knights){
        // if there are no bishops on the board what we do first is to check if there are
        // more then one bishop on the board. if not it is a insufficient material.
        // other ways we need to check if check if there is only two bishops on the board
        // and those bishop are on different sides of the board. if so the result is insufficient
        // material if bishops are on the same color. other ways it is not. 
        if (more_then_one(bishops)){
            if (has_two_bits(bishops) && Board_WHITE_BISHOPS(board) && Board_BLACK_BISHOPS(board)){
                int b1 =  NCH_SQRIDX(Board_WHITE_BISHOPS(board));
                int b2 =  NCH_SQRIDX(Board_BLACK_BISHOPS(board));
    
                if (NCH_SQR_SAME_COLOR(b1, b2))
                    return 0;
                return 1;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}

int
Board_IsThreeFold(const Board* board){
    return BoardDict_GetCount(&board->dict, board->bitboards) > 2;
}

int
Board_IsFiftyMoves(const Board* board){
    return board->fifty_counter >= 50;
}

Board*
Board_Copy(const Board* src_board){
    // the way how this function behaves would be replaces in the future
    // the board needs to support the stack allocation and the copy function
    // should be able to copy the board to the stack.
    // This function would take trg_board as a parameter and copy the src_board
    // in the future.

    Board* dst_board = malloc(sizeof(Board));
    if (!dst_board)
        return NULL;

    *dst_board = *src_board;

    int res = MoveList_CopyExtra(&src_board->movelist, &dst_board->movelist);
    if (res < 0){
        free(dst_board);
        return NULL;
    }
    
    BoardDict* new_dict = BoardDict_Copy(&src_board->dict);
    if (!new_dict){
        MoveList_Free(&dst_board->movelist);
        free(dst_board);
        return NULL;
    }

    dst_board->dict = *new_dict;
    free(new_dict);

    return dst_board;
}

GameState
Board_State(const Board* board, int can_move){
    if (can_move){
        if (Board_IsThreeFold(board))
            return NCH_GS_Draw_ThreeFold;

        if (Board_IsFiftyMoves(board))
            return NCH_GS_Draw_FiftyMoves;

        if (Board_IsInsufficientMaterial(board))
            return NCH_GS_Draw_InsufficientMaterial;
    }
    else{
        if (!Board_IS_CHECK(board))
            return NCH_GS_Draw_Stalemate;

        if (Board_IS_WHITETURN(board))
            return NCH_GS_BlackWin;
        else
            return NCH_GS_WhiteWin;
    }

    return NCH_GS_Playing;
}