#include "board.h"
#include "config.h"
#include "io.h"
#include "bitboard.h"
#include "generate_utils.h"
#include "generate.h"
#include <stdlib.h>
#include <string.h>
#include "move.h"
#include <stdio.h>
#include "utils.h"
#include "hash.h"
#include "board_utils.h"

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
    Board_Update(board);
}

NCH_STATIC_FINLINE Board*
new_board(){
    Board* board = malloc(sizeof(Board));
    if (!board){
        return NULL;
    }

    MoveList_Init(&board->movelist);

    board->dict = BoardDict_New();
    if (!board->dict){
        free(board);
        return NULL;
    }

    return board;
}

Board*
Board_New(){
    Board* board = new_board();
    if (!board){
        return NULL;
    }
    Board_Init(board);
    return board;
}


Board*
Board_NewEmpty(){
    Board* board = new_board();
    if (!board){
        return NULL;
    }
    Board_InitEmpty(board);
    return board;
}

void
Board_Free(Board* board){
    if (board){
        BoardDict_Free(board->dict);
        free(board);
    }
}

void
Board_Init(Board* board){
    board->bitboards[NCH_White][NCH_Pawn] = NCH_BOARD_W_PAWNS_STARTPOS;
    board->bitboards[NCH_White][NCH_Knight] = NCH_BOARD_W_KNIGHTS_STARTPOS;
    board->bitboards[NCH_White][NCH_Bishop] = NCH_BOARD_W_BISHOPS_STARTPOS;
    board->bitboards[NCH_White][NCH_Rook] = NCH_BOARD_W_ROOKS_STARTPOS;
    board->bitboards[NCH_White][NCH_Queen] = NCH_BOARD_W_QUEEN_STARTPOS;
    board->bitboards[NCH_White][NCH_King] = NCH_BOARD_W_KING_STARTPOS;

    board->bitboards[NCH_Black][NCH_Pawn] = NCH_BOARD_B_PAWNS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Knight] = NCH_BOARD_B_KNIGHTS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Bishop] = NCH_BOARD_B_BISHOPS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Rook] = NCH_BOARD_B_ROOKS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Queen] = NCH_BOARD_B_QUEEN_STARTPOS;
    board->bitboards[NCH_Black][NCH_King] = NCH_BOARD_B_KING_STARTPOS;

    _init_board(board);
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
    board->castles = 0;
}

int
Board_IsCheck(Board* board){
    return get_checkmap(
            board,
            Board_IS_WHITETURN(board) ? NCH_White : NCH_Black,
            NCH_SQRIDX( Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
            Board_ALL_OCC(board)
        ) != 0ULL;
}

NCH_STATIC_FINLINE void
update_check(Board* board){
    uint64 check_map = get_checkmap(
        board,
        Board_GET_SIDE(board),
        NCH_SQRIDX( Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
        Board_ALL_OCC(board)
    );

    if (check_map)
        NCH_SETFLG(board->flags, more_then_one(check_map) ? Board_CHECK | Board_DOUBLECHECK : Board_CHECK);
}

void
Board_Update(Board* board){
    update_check(board);

    if (BoardDict_GetCount(board->dict, board->bitboards) > 2){
        end_game_by_draw(board, Board_THREEFOLD);
        return;
    }

    if (board->fifty_counter > 49){
        end_game_by_draw(board, Board_FIFTYMOVES);
        return;
    }

    if (!at_least_one_move(board)){
        if (Board_IS_CHECK(board)){
            end_game_by_wl(board);
        }
        else{
            end_game_by_draw(board, Board_STALEMATE);
        }
    }
}

void
Board_Reset(Board* board){
    BoardDict_Reset(board->dict);
    MoveList_Reset(&board->movelist);
    Board_Init(board);
}

int
Board_IsInsufficientMaterial(Board* board){
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

    if (!bishops){
        uint64 knights = Board_WHITE_KNIGHTS(board)
                       | Board_BLACK_KNIGHTS(board); 

        if (more_then_one(knights) && !has_two_bits(knights))
            return 0;

        return 1;
    }

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

    uint64 knights = Board_WHITE_KNIGHTS(board)
                   | Board_BLACK_KNIGHTS(board); 
    
    if (more_then_one(knights))
        return 0;

    uint64 kb = knights | bishops;

    uint64 w_kb = Board_WHITE_OCC(board) & kb;
    if (w_kb){
        if (more_then_one(w_kb))
            return 1;
        return 0;
    }

    uint64 b_kb = Board_BLACK_OCC(board) & kb;
    if (b_kb){
        if (more_then_one(b_kb))
            return 1;
        return 0;
    }

    return 1;
}

int
Board_IsThreeFold(Board* board){
    return BoardDict_GetCount(board->dict, board->bitboards) > 2;
}

int
Board_IsFiftyMoves(Board* board){
    return board->fifty_counter >= 50;
}

int
Board_GetMovesOf(Board* board, Square s, Move* moves){
    Piece p = Board_ON_SQUARE(board, s);
    if (p == NCH_NO_PIECE)
        return 0;

    Move pseudo_moves[30];
    Move* tail = pseudo_moves;

    if (p == NCH_King){
        tail = generate_castle_moves(board, tail);
        tail = generate_king_moves(board, tail);
    }
    else{
        uint64 allowed_squares = Board_IS_WHITETURN(board) ? ~Board_WHITE_OCC(board) : ~Board_BLACK_OCC(board);
        tail = generate_any_move(board, Board_GET_SIDE(board),
                                s, Board_ALL_OCC(board),
                                allowed_squares, pseudo_moves);
    }

    int len = (int)(tail - pseudo_moves);
    int nmoves = 0;
    for (int i = 0; i < len; i++){
        Piece captured = make_move(board, pseudo_moves[i]);
        int is_check = Board_IsCheck(board);
        if (!is_check){
            moves[nmoves] = pseudo_moves[i];
            nmoves++;
        }

        undo_move(board, Board_GET_SIDE(board), pseudo_moves[i], captured);
    }

    return nmoves;
}

Board*
Board_Copy(const Board* src_board){
    Board* dst_board = malloc(sizeof(Board));
    if (!dst_board)
        return NULL;

    *dst_board = *src_board;

    int res = MoveList_CopyExtra(&src_board->movelist, &dst_board->movelist);
    if (res < 0){
        free(dst_board);
        return NULL;
    }
    
    BoardDict* new_dict = BoardDict_Copy(src_board->dict);
    if (!new_dict){
        free(dst_board);
        MoveList_Free(&dst_board->movelist);
        return NULL;
    }

    dst_board->dict = new_dict;

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
        if (!Board_IsCheck(board))
            return NCH_GS_Draw_Stalemate;

        if (Board_IS_WHITETURN(board))
            return NCH_GS_BlackWin;
        else
            return NCH_GS_WhiteWin;
    }

    return NCH_GS_Playing;
}