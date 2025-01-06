#include "utils.h"

void
set_board_enp_settings(Board* board, Side side, Square enp_sqr){
    uint64 sqr = NCH_SQR(enp_sqr);
    board->en_passant_idx = enp_sqr;
    board->en_passant_map = sqr | (((NCH_NXTSQR_RIGHT(sqr) & 0x7f7f7f7f7f7f7f7f)
                                | (NCH_NXTSQR_LEFT(sqr) & 0xfefefefefefefefe))
                                & board->bitboards[TARGET_SIDE(side)][NCH_Pawn]);
    board->en_passant_trg = side == NCH_White ? NCH_NXTSQR_DOWN(sqr)
                                              : NCH_NXTSQR_UP(sqr);
}