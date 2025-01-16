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

