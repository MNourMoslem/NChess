#include "board.h"
#include "config.h"
#include "io.h"

NCH_STATIC_INLINE void
_set_board_occupancy(Board* board){
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

    _set_board_occupancy(board);
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

    _set_board_occupancy(board);
}



void
Board_SetSquare(Board* board, Side side, Piece ptype, int sqr_idx){
    board->bitboards[side][ptype] |= NCH_SQR(sqr_idx);
    _set_board_occupancy(board);
}

void
Board_SetBitboard(Board* board, Side side, Piece ptype, uint64 bb){
    board->bitboards[side][ptype] = bb;
    _set_board_occupancy(board);
}