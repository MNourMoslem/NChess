#ifndef NCHESS_SRC_CORE_H
#define NCHESS_SRC_CORE_H

#include "types.h"
#include "bit_operations.h"

typedef enum{
    NCH_White,
    NCH_Black,
    NCH_SIDES_NB
}Side;

typedef enum {
    NCH_H1 = 0, NCH_G1, NCH_F1, NCH_E1, NCH_D1, NCH_C1, NCH_B1, NCH_A1, 
        NCH_H2, NCH_G2, NCH_F2, NCH_E2, NCH_D2, NCH_C2, NCH_B2, NCH_A2, 
        NCH_H3, NCH_G3, NCH_F3, NCH_E3, NCH_D3, NCH_C3, NCH_B3, NCH_A3,
        NCH_H4, NCH_G4, NCH_F4, NCH_E4, NCH_D4, NCH_C4, NCH_B4, NCH_A4,
        NCH_H5, NCH_G5, NCH_F5, NCH_E5, NCH_D5, NCH_C5, NCH_B5, NCH_A5,
        NCH_H6, NCH_G6, NCH_F6, NCH_E6, NCH_D6, NCH_C6, NCH_B6, NCH_A6,
        NCH_H7, NCH_G7, NCH_F7, NCH_E7, NCH_D7, NCH_C7, NCH_B7, NCH_A7,
        NCH_H8, NCH_G8, NCH_F8, NCH_E8, NCH_D8, NCH_C8, NCH_B8, NCH_A8,
    NCH_SQUARE_NB
}Square;

typedef enum {
    NCH_Pawn,
    NCH_Knight,
    NCH_Bishop,
    NCH_Rook,
    NCH_Queen,
    NCH_King,

    NCH_PIECE_NB,
    NCH_NO_PIECE
}Piece;
 
#define NCH_ROW1 0x00000000000000FFULL
#define NCH_ROW2 (NCH_ROW1 << 8 * 1)
#define NCH_ROW3 (NCH_ROW1 << 8 * 2)
#define NCH_ROW4 (NCH_ROW1 << 8 * 3)
#define NCH_ROW5 (NCH_ROW1 << 8 * 4)
#define NCH_ROW6 (NCH_ROW1 << 8 * 5)
#define NCH_ROW7 (NCH_ROW1 << 8 * 6)
#define NCH_ROW8 (NCH_ROW1 << 8 * 7)

#define NCH_COL1 0x0101010101010101ULL
#define NCH_COL2 (NCH_COL1 << 1)
#define NCH_COL3 (NCH_COL1 << 2)
#define NCH_COL4 (NCH_COL1 << 3)
#define NCH_COL5 (NCH_COL1 << 4)
#define NCH_COL6 (NCH_COL1 << 5)
#define NCH_COL7 (NCH_COL1 << 6)
#define NCH_COL8 (NCH_COL1 << 7)

#define NCH_CHKFLG(flag, x) (((flag) & (x)) == (x))
#define NCH_RMVFLG(flag, x) ((flag) &= ~(x))
#define NCH_SETFLG(flag, x) ((flag) |= (x))
#define NCH_CHKUNI(flag, x) (((flag) & (x)) != 0)
#define NCH_FLPFLG(flag, x) ((flag) ^= (x))

#define NCH_NXTSQR_UP(square) (square << 8)
#define NCH_NXTSQR_UP2(square) (square << 16)
#define NCH_NXTSQR_DOWN(square) (square >> 8)
#define NCH_NXTSQR_DOWN2(square) (square >> 16)
#define NCH_NXTSQR_RIGHT(square) (square >> 1)
#define NCH_NXTSQR_LEFT(square) (square << 1)
#define NCH_NXTSQR_UPRIGHT(square) (square << 7)
#define NCH_NXTSQR_UPLEFT(square) (square << 9)
#define NCH_NXTSQR_DOWNRIGHT(square) (square >> 9)
#define NCH_NXTSQR_DOWNLEFT(square) (square >> 7)

#define NCH_NXTSQR_K_UPRIGHT(square) (square << 15)
#define NCH_NXTSQR_K_UPLEFT(square) (square << 17)
#define NCH_NXTSQR_K_DOWNRIGHT(square) (square >> 17)
#define NCH_NXTSQR_K_DOWNLEFT(square) (square >> 15)
#define NCH_NXTSQR_K_RIGHTUP(square) (square << 6)
#define NCH_NXTSQR_K_RIGHTDOWN(square) (square >> 10)
#define NCH_NXTSQR_K_LEFTUP(square) (square << 10)
#define NCH_NXTSQR_K_LEFTDOWN(square) (square >> 6)

static const uint64 NCH_DIAGONAL_MAIN[15] = {
    0x0000000000000001ull,
    0x0000000000000101ull,
    0x0000000000010204ull,
    0x0000000001020408ull,
    0x0000000102040810ull,
    0x0000010204081020ull,
    0x0001020408102040ull,
    0x0102040810204080ull,
    0x0204081020408000ull,
    0x0408102040800000ull,
    0x0810204080000000ull,
    0x1020408000000000ull,
    0x2040800000000000ull,
    0x4080000000000000ull,
    0x8000000000000000ull,
};

static const int NCH_DIAGONAL_MAIN_IDX[64] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    1, 2, 3, 4, 5, 6, 7, 8,
    2, 3, 4, 5, 6, 7, 8, 9,
    3, 4, 5, 6, 7, 8, 9, 10,
    4, 5, 6, 7, 8, 9, 10, 11,
    5, 6, 7, 8, 9, 10, 11, 12,
    6, 7, 8, 9, 10, 11, 12, 13,
    7, 8, 9, 10, 11, 12, 13, 14
};

static const uint64 NCH_DIAGONAL_ANTI[15] = {
    0x0000000000000080ull,
    0x0000000000008040ull,
    0x0000000000804020ull,
    0x0000000080402010ull,
    0x0000008040201008ull,
    
    0x0000804020100804ull,
    
    0x0080402010080402ull,
    0x8040201008040201ull,
    0x4020100804020100ull,
    0x2010080402010000ull,
    0x1008040201000000ull,
    0x0804020100000000ull,
    0x0402010000000000ull,
    0x0201000000000000ull,
    0x0100000000000000ull,
};

static const int NCH_DIAGONAL_ANTI_IDX[64] = {
    7, 6, 5, 4, 3, 2, 1, 0,
    8, 7, 6, 5, 4, 3, 2, 1,
    9, 8, 7, 6, 5, 4, 3, 2,
    10, 9, 8, 7, 6, 5, 4, 3,
    11, 10, 9, 8, 7, 6, 5, 4,
    12, 11, 10, 9, 8, 7, 6, 5,
    13, 12, 11, 10, 9, 8, 7, 6,
    14, 13, 12, 11, 10, 9, 8, 7
};

#define NCH_SQR(idx) (1ULL << (idx))
#define NCH_SQRIDX(square) count_tbits(square)
#define NCH_GET_COLIDX(idx) ((idx) % 8ull)
#define NCH_GET_ROWIDX(idx) ((idx) / 8ull)
#define NCH_GET_COL(idx) (NCH_COL1 << (NCH_GET_COLIDX(idx)))
#define NCH_GET_ROW(idx) (NCH_ROW1 << (NCH_GET_ROWIDX(idx) * 8))
#define NCH_GET_DIGMAIN(idx) NCH_DIAGONAL_MAIN[NCH_DIAGONAL_MAIN_IDX[idx]]
#define NCH_GET_DIGANTI(idx) NCH_DIAGONAL_ANTI[NCH_DIAGONAL_ANTI_IDX[idx]]

#define NCH_SAME_COL(idx1, idx2) (NCH_GET_COLIDX(idx1) == NCH_GET_COLIDX(idx2))
#define NCH_SAME_ROW(idx1, idx2) (NCH_GET_ROWIDX(idx1) == NCH_GET_ROWIDX(idx2))
#define NCH_SAME_MAIN_DG(idx1, idx2) (NCH_DIAGONAL_MAIN_IDX[idx1] == NCH_DIAGONAL_MAIN_IDX[idx2])
#define NCH_SAME_ANTI_DG(idx1, idx2) (NCH_DIAGONAL_ANTI_IDX[idx1] == NCH_DIAGONAL_ANTI_IDX[idx2])

typedef uint32 Move;

#define Move_ASSIGN_FROM(from_) (from_)
#define Move_ASSIGN_TO(to_) (to_ << 6)
#define Move_ASSIGN_CASTLE(castle) (castle << 12)
#define Move_ASSIGN_PRO_PIECE(pro_piece) (pro_piece << 16)

#define Move_FROM(move) (move & 0x3F)
#define Move_TO(move) ((move >> 6) & 0x3F)
#define Move_CASTLE(move) ((move >> 12) & 0xF)
#define Move_PRO_PIECE(move) ((move >> 16) & 0xF)

#endif