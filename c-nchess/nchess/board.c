/*
    board.c

    This file contains the function definitions for the board functions.
*/

#include "board.h"
#include "config.h"
#include "bitboard.h"
#include "utils.h"
#include "hash.h"
#include "board_utils.h"
#include "makemove.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NCH_STATIC_FINLINE void
_init_board_flags_and_states(Board* board){
    Board_CASTLES(board) = Board_CASTLE_WK | Board_CASTLE_WQ | Board_CASTLE_BK | Board_CASTLE_WQ;
    Board_ENP_IDX(board) = 0;
    Board_ENP_MAP(board) = 0ULL;
    Board_ENP_TRG(board) = 0ULL;
    Board_FLAGS(board) = 0;
    Board_FIFTY_COUNTER(board) = 0;
    Board_CAP_PIECE(board) = NCH_NO_PIECE;
    Board_SIDE(board) = NCH_White;

    Board_CASTLE_SQUARES(board, NCH_G1) = NCH_H1;
    Board_CASTLE_SQUARES(board, NCH_C1) = NCH_A1;
    Board_CASTLE_SQUARES(board, NCH_G8) = NCH_H8;
    Board_CASTLE_SQUARES(board, NCH_C8) = NCH_A8;

    Board_CASTLE_SQUARES(board, NCH_H1) = NCH_F1;
    Board_CASTLE_SQUARES(board, NCH_A1) = NCH_D1;
    Board_CASTLE_SQUARES(board, NCH_H8) = NCH_F8;
    Board_CASTLE_SQUARES(board, NCH_A8) = NCH_D8;

    board->nmoves = 0;
}

NCH_STATIC_FINLINE void
_init_board(Board* board){
    set_board_occupancy(board);
    init_piecetables(board);
    _init_board_flags_and_states(board);

    BoardDict_Init(&board->dict);
    MoveList_Init(&board->movelist);
}

Board*
Board_New(){
    Board* board = malloc(sizeof(Board));
    if (!board)
        return NULL;
    
    Board_Init(board);
    return board;
}

Board*
Board_NewEmpty(){
    Board* board = malloc(sizeof(Board));
    if (!board)
        return NULL;
    
    Board_InitEmpty(board);
    return board;
}

void
Board_FreeExtraOnly(Board* board){
    if (board){
        BoardDict_FreeExtra(&Board_DICT(board));
        MoveList_Free(&Board_MOVELIST(board));
    }
}

void
Board_Free(Board* board){
    if (board){
        Board_FreeExtraOnly(board);
        free(board);
    }
}

void
Board_Init(Board* board){
    Board_BB(board, NCH_White, NCH_Pawn)   = NCH_BOARD_W_PAWNS_STARTPOS;
    Board_BB(board, NCH_White, NCH_Knight) = NCH_BOARD_W_KNIGHTS_STARTPOS;
    Board_BB(board, NCH_White, NCH_Bishop) = NCH_BOARD_W_BISHOPS_STARTPOS;
    Board_BB(board, NCH_White, NCH_Rook)   = NCH_BOARD_W_ROOKS_STARTPOS;
    Board_BB(board, NCH_White, NCH_Queen)  = NCH_BOARD_W_QUEEN_STARTPOS;
    Board_BB(board, NCH_White, NCH_King)   = NCH_BOARD_W_KING_STARTPOS;

    Board_BB(board, NCH_Black, NCH_Pawn)   = NCH_BOARD_B_PAWNS_STARTPOS;
    Board_BB(board, NCH_Black, NCH_Knight) = NCH_BOARD_B_KNIGHTS_STARTPOS;
    Board_BB(board, NCH_Black, NCH_Bishop) = NCH_BOARD_B_BISHOPS_STARTPOS;
    Board_BB(board, NCH_Black, NCH_Rook)   = NCH_BOARD_B_ROOKS_STARTPOS;
    Board_BB(board, NCH_Black, NCH_Queen)  = NCH_BOARD_B_QUEEN_STARTPOS;
    Board_BB(board, NCH_Black, NCH_King)   = NCH_BOARD_B_KING_STARTPOS;

    _init_board(board);

    // starting position does not cause a check to any side but it is better to call
    // the update_check function to make sure that the board is in a valid state.
    update_check(board);
}

void
Board_InitEmpty(Board* board){
    Board_BB(board, NCH_White, NCH_Pawn)   = 0ULL;
    Board_BB(board, NCH_White, NCH_Knight) = 0ULL;
    Board_BB(board, NCH_White, NCH_Bishop) = 0ULL;
    Board_BB(board, NCH_White, NCH_Rook)   = 0ULL;
    Board_BB(board, NCH_White, NCH_Queen)  = 0ULL;
    Board_BB(board, NCH_White, NCH_King)   = 0ULL;

    Board_BB(board, NCH_Black, NCH_Pawn)   = 0ULL;
    Board_BB(board, NCH_Black, NCH_Knight) = 0ULL;
    Board_BB(board, NCH_Black, NCH_Bishop) = 0ULL;
    Board_BB(board, NCH_Black, NCH_Rook)   = 0ULL;
    Board_BB(board, NCH_Black, NCH_Queen)  = 0ULL;
    Board_BB(board, NCH_Black, NCH_King)   = 0ULL;

    _init_board(board);

    // the _init_board sets the castles to the initial state and here we reset them
    // because the board is empty. Considereblt this is not the best way to do this
    // setting the value of the same variable twice like this but it is the easiest way
    // for now and it a new desing would be implemented in the future.
    Board_CASTLES(board) = 0;
}

int
Board_IsCheck(const Board* board){
    Side side = Board_SIDE(board);
    return get_checkmap(
            board,
            side,
            NCH_SQRIDX( Board_BB(board, side, NCH_King) ),
            Board_ALL_OCC(board)
        ) != 0ULL;
}

void
Board_Reset(Board* board){
    for (int i = i; i < Board_NMOVES(board); i++){
        Board_Undo(board);
    }
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
        // other ways we need to check if there is only two bishops on the board
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
    return BoardDict_GetCount(&Board_DICT(board), board->bitboards) > 2;
}

int
Board_IsFiftyMoves(const Board* board){
    return Board_FIFTY_COUNTER(board) >= 50;
}

int
Board_Copy(const Board* src_board, Board* dst_board){
    *dst_board = *src_board;

    int res = MoveList_CopyExtra(&Board_MOVELIST(src_board), &Board_MOVELIST(dst_board));
    if (res < 0)
        return -1;
    
    res = BoardDict_CopyExtra(&Board_DICT(src_board), &Board_DICT(dst_board));
    if (res < 0){
        MoveList_Free(&Board_MOVELIST(dst_board));
        return -1;
    }

    return 0;
}

Board*
Board_NewCopy(const Board* src_board){
    Board* dst_board = malloc(sizeof(Board));
    if (!dst_board)
        return NULL;

    int res = Board_Copy(src_board, dst_board);
    if (res < 0)
        return NULL;
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