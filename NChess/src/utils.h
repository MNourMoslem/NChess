#ifndef NCHESS_SRC_UTILS_H
#define NCHESS_SRC_UTILS_H

#include "types.h"
#include "board.h"
#include "config.h"

#define TARGET_SIDE(side) (side == NCH_White ? NCH_Black : NCH_White)

void
set_board_enp_settings(Board* board, Side side, Square enp_sqr);

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