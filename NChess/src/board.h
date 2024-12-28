#ifndef NCHESS_SRC_BOARD_H
#define NCHESS_SRC_BOARD_H

#include "core.h"
#include "types.h"
#include "config.h"

#define NCH_BOARD_W_PAWNS_STARTPOS 0x000000000000FF00
#define NCH_BOARD_W_KNIGHTS_STARTPOS 0x0000000000000042
#define NCH_BOARD_W_BISHOPS_STARTPOS 0x0000000000000024
#define NCH_BOARD_W_ROOKS_STARTPOS 0x0000000000000081
#define NCH_BOARD_W_QUEEN_STARTPOS 0x0000000000000010
#define NCH_BOARD_W_KING_STARTPOS 0x0000000000000008

#define NCH_BOARD_B_PAWNS_STARTPOS 0x00FF000000000000
#define NCH_BOARD_B_KNIGHTS_STARTPOS 0x4200000000000000
#define NCH_BOARD_B_BISHOPS_STARTPOS 0x2400000000000000
#define NCH_BOARD_B_ROOKS_STARTPOS 0x8100000000000000
#define NCH_BOARD_B_QUEEN_STARTPOS 0x1000000000000000
#define NCH_BOARD_B_KING_STARTPOS 0x0800000000000000

typedef struct
{
    uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB];
    uint64 occupancy[NCH_SIDES_NB + 1];
    int flags;
}Board;

#define Board_WHITE_OCC(board) (board)->occupancy[NCH_White]
#define Board_BLACK_OCC(board) (board)->occupancy[NCH_Black]
#define Board_ALL_OCC(board) (board)->occupancy[NCH_SIDES_NB]

#define Board_WHITE_PAWNS(board) (board)->bitboards[NCH_White][NCH_Pawn]
#define Board_WHITE_KNIGHTS(board) (board)->bitboards[NCH_White][NCH_Knight]
#define Board_WHITE_BISHOPS(board) (board)->bitboards[NCH_White][NCH_Bishop]
#define Board_WHITE_ROOKS(board) (board)->bitboards[NCH_White][NCH_Rook]
#define Board_WHITE_QUEENS(board) (board)->bitboards[NCH_White][NCH_Queen]
#define Board_WHITE_KING(board) (board)->bitboards[NCH_White][NCH_King]

#define Board_BLACK_PAWNS(board) (board)->bitboards[NCH_Black][NCH_Pawn]
#define Board_BLACK_KNIGHTS(board) (board)->bitboards[NCH_Black][NCH_Knight]
#define Board_BLACK_BISHOPS(board) (board)->bitboards[NCH_Black][NCH_Bishop]
#define Board_BLACK_ROOKS(board) (board)->bitboards[NCH_Black][NCH_Rook]
#define Board_BLACK_QUEENS(board) (board)->bitboards[NCH_Black][NCH_Queen]
#define Board_BLACK_KING(board) (board)->bitboards[NCH_Black][NCH_King]

void
Board_Init(Board* board);

void
Board_InitEmpty(Board* board);

void
Board_SetSquare(Board* board, Side side, Piece ptype, int sqr_idx);

void
Board_SetBitboard(Board* board, Side side, Piece ptype, uint64 bb);

#endif