#ifndef NCHESS_SRC_UTILS_H
#define NCHESS_SRC_UTILS_H

#include "types.h"
#include "board.h"
#include "config.h"
#include "loops.h"

#define TARGET_SIDE(side) (side == NCH_White ? NCH_Black : NCH_White)

NCH_STATIC_INLINE void
set_board_enp_settings(Board* board, Side side, Square enp_sqr){
    uint64 sqr = NCH_SQR(enp_sqr);
    board->en_passant_idx = enp_sqr;
    board->en_passant_map = sqr | (((NCH_NXTSQR_RIGHT(sqr) & 0x7f7f7f7f7f7f7f7f)
                                | (NCH_NXTSQR_LEFT(sqr) & 0xfefefefefefefefe))
                                & board->bitboards[TARGET_SIDE(side)][NCH_Pawn]);
    board->en_passant_trg = side == NCH_White ? NCH_NXTSQR_DOWN(sqr)
                                              : NCH_NXTSQR_UP(sqr);
}


NCH_STATIC_INLINE void
init_piecetables(Board* board){
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_WHITE_PIECE(board, i) = NCH_NO_PIECE;
    }

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_BLACK_PIECE(board, i) = NCH_NO_PIECE;
    }

    int idx;
    LOOP_U64_T(Board_WHITE_PAWNS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_WHITE_KNIGHTS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_WHITE_BISHOPS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_WHITE_ROOKS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_WHITE_QUEENS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_WHITE_KING(board)){
        Board_WHITE_PIECE(board, idx) = NCH_King;
    }

    LOOP_U64_T(Board_BLACK_PAWNS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_BLACK_KNIGHTS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_BLACK_BISHOPS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_BLACK_ROOKS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_BLACK_QUEENS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_BLACK_KING(board)){
        Board_BLACK_PIECE(board, idx) = NCH_King;
    }
}

NCH_STATIC_INLINE void
reset_enpassant_variable(Board* board){
    board->en_passant_idx = 0;
    board->en_passant_map = 0ull;
    board->en_passant_trg = 0ull;
}

NCH_STATIC_INLINE void
set_board_occupancy(Board* board){
    board->occupancy[NCH_White] = board->bitboards[NCH_White][NCH_Pawn]
                                | board->bitboards[NCH_White][NCH_Knight]
                                | board->bitboards[NCH_White][NCH_Bishop]
                                | board->bitboards[NCH_White][NCH_Rook]
                                | board->bitboards[NCH_White][NCH_Queen]
                                | board->bitboards[NCH_White][NCH_King];

    board->occupancy[NCH_Black] = board->bitboards[NCH_Black][NCH_Pawn]
                                | board->bitboards[NCH_Black][NCH_Knight]
                                | board->bitboards[NCH_Black][NCH_Bishop]
                                | board->bitboards[NCH_Black][NCH_Rook]
                                | board->bitboards[NCH_Black][NCH_Queen]
                                | board->bitboards[NCH_Black][NCH_King];

    board->occupancy[NCH_SIDES_NB] = board->occupancy[NCH_Black] 
                                   | board->occupancy[NCH_White];
}


#endif