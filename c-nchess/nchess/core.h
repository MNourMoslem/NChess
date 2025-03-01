/*
    core.h

    This file contains the core definitions and functions of the project.
    Main enums and constants are defined here.
    Side, PieceType, Square, Diractions enums are defined here.
    Row, column, diagonal bitboards and tabels are defined here.
    Macros for square manipulation are defined here.

    This file would be depercated in the future and all macros, tables,
    enums and stuff would be moved to board.h, types.h, and other files.
*/


#ifndef NCHESS_SRC_CORE_H
#define NCHESS_SRC_CORE_H

#include "types.h"
#include "bit_operations.h"

#include <math.h>

typedef enum{
    NCH_NO_SIDE = -1,
    NCH_White,
    NCH_Black,
    NCH_SIDES_NB,
}Side;

#define NCH_OP_SIDE(side) side ^ NCH_Black

typedef enum {
    NCH_NO_SQR = -1,

    NCH_H1 = 0, NCH_G1, NCH_F1, NCH_E1, NCH_D1, NCH_C1, NCH_B1, NCH_A1, 
        NCH_H2, NCH_G2, NCH_F2, NCH_E2, NCH_D2, NCH_C2, NCH_B2, NCH_A2, 
        NCH_H3, NCH_G3, NCH_F3, NCH_E3, NCH_D3, NCH_C3, NCH_B3, NCH_A3,
        NCH_H4, NCH_G4, NCH_F4, NCH_E4, NCH_D4, NCH_C4, NCH_B4, NCH_A4,
        NCH_H5, NCH_G5, NCH_F5, NCH_E5, NCH_D5, NCH_C5, NCH_B5, NCH_A5,
        NCH_H6, NCH_G6, NCH_F6, NCH_E6, NCH_D6, NCH_C6, NCH_B6, NCH_A6,
        NCH_H7, NCH_G7, NCH_F7, NCH_E7, NCH_D7, NCH_C7, NCH_B7, NCH_A7,
        NCH_H8, NCH_G8, NCH_F8, NCH_E8, NCH_D8, NCH_C8, NCH_B8, NCH_A8,
        
    NCH_SQUARE_NB,
}Square;

typedef enum {
    NCH_NO_PIECE_TYPE,

    NCH_Pawn,
    NCH_Knight,
    NCH_Bishop,
    NCH_Rook,
    NCH_Queen,
    NCH_King,

    NCH_PIECE_TYPE_NB,
}PieceType;

typedef enum {
    NCH_NO_PIECE,

    NCH_WPawn,
    NCH_WKnight,
    NCH_WBishop,
    NCH_WRook,
    NCH_WQueen,
    NCH_WKing,

    NCH_BPawn,
    NCH_BKnight,
    NCH_BBishop,
    NCH_BRook,
    NCH_BQueen,
    NCH_BKing,

    NCH_PIECE_NB,
}Piece;

extern const PieceType Piece2Type[NCH_PIECE_NB];
extern const Side Piece2Side[NCH_PIECE_NB];
extern const Piece PieceType2Piece[NCH_SIDES_NB][NCH_PIECE_TYPE_NB];

#define Piece_TYPE(piece) Piece2Type[piece]
#define Piece_SIDE(piece) Piece2Side[piece]
#define PieceType_PIECE(side, piece_type) PieceType2Piece[side][piece_type]

typedef enum{
    NCH_GS_Playing = 0,
    NCH_GS_WhiteWin,
    NCH_GS_BlackWin,
    NCH_GS_Draw_Stalemate,
    NCH_GS_Draw_ThreeFold,
    NCH_GS_Draw_FiftyMoves,
    NCH_GS_Draw_InsufficientMaterial
}GameState;

// rows starts from bottom to top (row '1' to '8' of the board)
#define NCH_ROW1 0x00000000000000FFULL
#define NCH_ROW2 0x000000000000FF00ULL
#define NCH_ROW3 0x0000000000FF0000ULL
#define NCH_ROW4 0x00000000FF000000ULL
#define NCH_ROW5 0x000000FF00000000ULL
#define NCH_ROW6 0x0000FF0000000000ULL
#define NCH_ROW7 0x00FF000000000000ULL
#define NCH_ROW8 0xFF00000000000000ULL

// columns starts from right to left (column 'h' to 'a' of the board)
#define NCH_COL1 0x0101010101010101ULL
#define NCH_COL2 0x0202020202020202ULL
#define NCH_COL3 0x0404040404040404ULL
#define NCH_COL4 0x0808080808080808ULL
#define NCH_COL5 0x1010101010101010ULL
#define NCH_COL6 0x2020202020202020ULL
#define NCH_COL7 0x4040404040404040ULL
#define NCH_COL8 0x8080808080808080ULL

#define NCH_CHKFLG(flag, x) (((flag) & (x)) == (x))
#define NCH_RMVFLG(flag, x) ((flag) &= ~(x))
#define NCH_SETFLG(flag, x) ((flag) |= (x))
#define NCH_CHKUNI(flag, x) (((flag) & (x)) != 0)
#define NCH_FLPFLG(flag, x) ((flag) ^= (x))

// square manipulation macros. square here is a bitboard
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

extern const int NCH_ROW_TABLE[64];         // stores the row index of the square
extern const int NCH_COL_TABLE[64];         // stores the column index of the square
extern const uint64 NCH_DIAGONAL_MAIN[15];  // stores the main diagonal bitboards
extern const int NCH_DIAGONAL_MAIN_IDX[64]; // stores the index of the main diagonal
extern const uint64 NCH_DIAGONAL_ANTI[15];  // stores the anti diagonal bitboards
extern const int NCH_DIAGONAL_ANTI_IDX[64]; // stores the index of the anti diagonal
/*
    Note:

    row and columns could be calculated bt mod and division operations
    but it is much faster to use a table to get the row and column.
    same thing for diagonals they could be found throw mod and division
    but it is much faster to use a table to get the diagonal index.
*/

typedef enum{
    NCH_Up = 0,
    NCH_Down,
    NCH_Right,
    NCH_Left,
    NCH_UpRight,
    NCH_UpLeft,
    NCH_DownRight,
    NCH_DownLeft,

    NCH_DIR_NB,

    NCH_NO_DIR
}Diractions; // the type name must be replaced with Diraction.

// a table of size NCH_SQUARE_NB * NCH_SQUARE_NB that stores the direction
// from one square to another. if the source square and the target are not
// on the same row, column or diagonal the value would be NCH_NO_DIR
extern Diractions NCH_DIRACTION_TABLE[NCH_SQUARE_NB][NCH_SQUARE_NB];


// macros ends with IDX returns the index and other macros return bitboards.
// macro naming is awful and would be changed in the future.
#define NCH_SQR(idx) (1ULL << (idx))
#define NCH_SQRIDX(square) count_last_zeros(square)
#define NCH_GET_COLIDX(idx) NCH_COL_TABLE[idx]
#define NCH_GET_ROWIDX(idx) NCH_ROW_TABLE[idx]
#define NCH_GET_COL(idx) (NCH_COL1 << (NCH_GET_COLIDX(idx)))
#define NCH_GET_ROW(idx) (NCH_ROW1 << (NCH_GET_ROWIDX(idx) * 8))
#define NCH_GET_DIGMAINIDX(idx) NCH_DIAGONAL_MAIN_IDX[idx]
#define NCH_GET_DIGANTIIDX(idx) NCH_DIAGONAL_ANTI_IDX[idx]
#define NCH_GET_DIGMAIN(idx) NCH_DIAGONAL_MAIN[NCH_GET_DIGMAINIDX(idx)]
#define NCH_GET_DIGANTI(idx) NCH_DIAGONAL_ANTI[NCH_GET_DIGANTIIDX(idx)]
#define NCH_GET_DIRACTION(from, to) NCH_DIRACTION_TABLE[from][to]

#define NCH_SAME_COL(idx1, idx2) (NCH_GET_COLIDX(idx1) == NCH_GET_COLIDX(idx2))
#define NCH_SAME_ROW(idx1, idx2) (NCH_GET_ROWIDX(idx1) == NCH_GET_ROWIDX(idx2))
#define NCH_SAME_MAIN_DG(idx1, idx2) (NCH_DIAGONAL_MAIN_IDX[idx1] == NCH_DIAGONAL_MAIN_IDX[idx2])
#define NCH_SAME_ANTI_DG(idx1, idx2) (NCH_DIAGONAL_ANTI_IDX[idx1] == NCH_DIAGONAL_ANTI_IDX[idx2])

#define NCH_SQR_DISTANCE(idx1, idx2) (abs(NCH_GET_COLIDX(idx1) - NCH_GET_COLIDX(idx2)) +\
                                      abs(NCH_GET_ROWIDX(idx1) - NCH_GET_ROWIDX(idx2)))

#define NCH_SQR_MIRROR_V(idx) (idx ^ 0x38)
#define NCH_SQR_MIRROR_H(idx) (idx ^ 0x07)
#define NCH_SQR_SAME_COLOR(idx1, idx2)\
 ((NCH_GET_DIGMAINIDX(idx1) & 1) == (1 & NCH_GET_DIGMAINIDX(idx2)))
// if the main diagonal index is even then the square is white
// if the main diagonal index is odd then the square is black
// if both squares are on the same color both indexes would be even or odd
// we check if the number is even by anding it with 1.


typedef struct
{
    uint8 castles; // castle rights
    int flags;     // board flags
    
    // These variables are used to store the information realted to en passant
    // The idx is the square of the pawn that moved twice and not the square that
    // the enemy pawn is attacking. The name is misleading and would be changed later.
    // The map is a bitboard where the target pawn and the attacker pawns are set to 1.
    // The trg is a bitboard where the target square the attacker would go to is set to 1.
    Square en_passant_idx;
    uint64 en_passant_map;
    uint64 en_passant_trg;

    int fifty_counter; // counter for fifty moves rule

    Piece captured_piece; // last captured piece. used for undoing moves

    Side side; // side to play
}PositionInfo;


// this function is used to initialize the all the core tables
void
NCH_InitTables();

#endif