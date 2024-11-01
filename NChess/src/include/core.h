#ifndef NCHESS_CORE_H
#define NCHESS_CORE_H

#include <math.h>
#include "types.h"
#include "hash.h"

#ifdef __GNUC__
    #define NCH_CLZLL(x) __builtin_clzll(x)
    #define NCH_CTZLL(x) __builtin_ctzll(x)
    #define NCH_POPCOUNT(x) __builtin_popcount(x)
    #define NCH_POPCOUNTLL(x) __builtin_popcountll(x)
#else
    #define NCH_CLZLL(x) custom_clzll(x)
    #define NCH_CTZLL(x) custom_ctzll(x)
    #define NCH_POPCOUNT(x) custom_popcount(x)
    #define NCH_POPCOUNTLL(x) custom_popcountll(x)

    static inline int custom_clzll(unsigned long long x) {
        int count = 0;
        while (x) {
            if (x & (1ULL << 63)) {
                break;
            }
            count++;
            x <<= 1;
        }
        return 64 - count;
    }

    static inline int custom_ctzll(unsigned long long x) {
        if (x == 0) return 64;
        int count = 0;
        while (!(x & 1)) {
            count++;
            x >>= 1;
        }
        return count;
    }

    static inline int custom_popcountll(unsigned long long x) {
        int count = 0;
        
        while (x) {
            count += x & 1;  // Check if the least significant bit is 1
            x >>= 1;         // Shift bits to the right by 1 to check the next bit
        }
        return count;
    }

    static inline int custom_popcount(unsigned long long x) {
        int count = 0;
        
        while (x) {
            count += x & 1;  // Check if the least significant bit is 1
            x >>= 1;         // Shift bits to the right by 1 to check the next bit
        }
        return count;
    }
#endif

#define NCH_WHITE 1
#define NCH_BLACK 0

const char NCH_PIECES[13] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
const char NCH_COLUMNS[8] = {'h' ,'g', 'f', 'e', 'd', 'c', 'b', 'a'};

#define NCH_NONE 0x0ull

#define NCH_H1 0x1ull        // 2^0
#define NCH_G1 0x2ull        // 2^1
#define NCH_F1 0x4ull        // 2^2
#define NCH_E1 0x8ull        // 2^3
#define NCH_D1 0x10ull       // 2^4
#define NCH_C1 0x20ull       // 2^5
#define NCH_B1 0x40ull       // 2^6
#define NCH_A1 0x80ull       // 2^7

#define NCH_H2 0x100ull      // 2^8
#define NCH_G2 0x200ull      // 2^9
#define NCH_F2 0x400ull      // 2^10
#define NCH_E2 0x800ull      // 2^11
#define NCH_D2 0x1000ull     // 2^12
#define NCH_C2 0x2000ull     // 2^13
#define NCH_B2 0x4000ull     // 2^14
#define NCH_A2 0x8000ull     // 2^15

#define NCH_H3 0x10000ull    // 2^16
#define NCH_G3 0x20000ull    // 2^17
#define NCH_F3 0x40000ull    // 2^18
#define NCH_E3 0x80000ull    // 2^19
#define NCH_D3 0x100000ull   // 2^20
#define NCH_C3 0x200000ull   // 2^21
#define NCH_B3 0x400000ull   // 2^22
#define NCH_A3 0x800000ull   // 2^23

#define NCH_H4 0x1000000ull  // 2^24
#define NCH_G4 0x2000000ull  // 2^25
#define NCH_F4 0x4000000ull  // 2^26
#define NCH_E4 0x8000000ull  // 2^27
#define NCH_D4 0x10000000ull // 2^28
#define NCH_C4 0x20000000ull // 2^29
#define NCH_B4 0x40000000ull // 2^30
#define NCH_A4 0x80000000ull // 2^31

#define NCH_H5 0x100000000ull  // 2^32
#define NCH_G5 0x200000000ull  // 2^33
#define NCH_F5 0x400000000ull  // 2^34
#define NCH_E5 0x800000000ull  // 2^35
#define NCH_D5 0x1000000000ull // 2^36
#define NCH_C5 0x2000000000ull // 2^37
#define NCH_B5 0x4000000000ull // 2^38
#define NCH_A5 0x8000000000ull // 2^39

#define NCH_H6 0x10000000000ull  // 2^40
#define NCH_G6 0x20000000000ull  // 2^41
#define NCH_F6 0x40000000000ull  // 2^42
#define NCH_E6 0x80000000000ull  // 2^43
#define NCH_D6 0x100000000000ull // 2^44
#define NCH_C6 0x200000000000ull // 2^45
#define NCH_B6 0x400000000000ull // 2^46
#define NCH_A6 0x800000000000ull // 2^47

#define NCH_H7 0x1000000000000ull  // 2^48
#define NCH_G7 0x2000000000000ull  // 2^49
#define NCH_F7 0x4000000000000ull  // 2^50
#define NCH_E7 0x8000000000000ull  // 2^51
#define NCH_D7 0x10000000000000ull // 2^52
#define NCH_C7 0x20000000000000ull // 2^53
#define NCH_B7 0x40000000000000ull // 2^54
#define NCH_A7 0x80000000000000ull // 2^55

#define NCH_H8 0x100000000000000ull  // 2^56
#define NCH_G8 0x200000000000000ull  // 2^57
#define NCH_F8 0x400000000000000ull  // 2^58
#define NCH_E8 0x800000000000000ull  // 2^59
#define NCH_D8 0x1000000000000000ull // 2^60
#define NCH_C8 0x2000000000000000ull // 2^61
#define NCH_B8 0x4000000000000000ull // 2^62
#define NCH_A8 0x8000000000000000ull // 2^63


#define NCH_ROW1 0x00000000000000FFull
#define NCH_ROW2 0x000000000000FF00ull
#define NCH_ROW3 0x0000000000FF0000ull
#define NCH_ROW4 0x00000000FF000000ull
#define NCH_ROW5 0x000000FF00000000ull
#define NCH_ROW6 0x0000FF0000000000ull
#define NCH_ROW7 0x00FF000000000000ull
#define NCH_ROW8 0xFF00000000000000ull
#define NCH_ROW_LAST 0xFF000000000000FFull
#define NCH_ROW_MID 0x000000FFFF000000ull
#define NCH_ROW_START 0x00FF00000000FF00ull

#define NCH_COL1 0x0101010101010101ull
#define NCH_COL2 0x0202020202020202ull
#define NCH_COL3 0x0404040404040404ull
#define NCH_COL4 0x0808080808080808ull
#define NCH_COL5 0x1010101010101010ull
#define NCH_COL6 0x2020202020202020ull
#define NCH_COL7 0x4040404040404040ull
#define NCH_COL8 0x8080808080808080ull

#define NCH_WHITE_PAWNS_START_POS 0x000000000000FF00ull
#define NCH_BLACK_PAWNS_START_POS 0x00FF000000000000ull

#define NCH_WHITE_KNIGHTS_START_POS 0x0000000000000042ull
#define NCH_BLACK_KNIGHTS_START_POS 0x4200000000000000ull

#define NCH_WHITE_BISHOPS_START_POS 0x0000000000000024ull
#define NCH_BLACK_BISHOPS_START_POS 0x2400000000000000ull

#define NCH_WHITE_ROOKS_START_POS 0x0000000000000081ull
#define NCH_BLACK_ROOKS_START_POS 0x8100000000000000ull

#define NCH_WHITE_QUEEN_START_POS 0x0000000000000010ull
#define NCH_BLACK_QUEEN_START_POS 0x1000000000000000ull

#define NCH_WHITE_KING_START_POS 0x0000000000000008ull
#define NCH_BLACK_KING_START_POS 0x0800000000000000ull

typedef enum{
    NCH_NoSM,
    NCH_OO,
    NCH_OOO,
    NCH_Promote2Queen,
    NCH_Promote2Rook,
    NCH_Promote2Knight,
    NCH_Promote2Bishop,
}NCH_SMoves;

#define NCH_CHKFLG(x, flag) ((x & flag) == flag)
#define NCH_RMVFLG(x, flag) (x &= ~flag)
#define NCH_SETFLG(x, flag) (x |= flag)
#define NCH_CHKUNI(x, flag) ((x & flag) != 0)

#define NCH_SQR(idx) (1ull << (idx))
#ifdef __GNUC__
    #define NCH_SQRIDX(square) NCH_CTZLL(square)
#else
    #define NCH_SQRIDX(square) (cuint64)log2((double)square)
#endif
#define NCH_GETCOL(square) (NCH_SQRIDX(square) % 8ull)
#define NCH_GETROW(square) (NCH_SQRIDX(square) / 8ull)

#define NCH_NXTSQR_UP(square) (square << 8)
#define NCH_NXTSQR_DOWN(square) (square >> 8)
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

#define NCH_MKMOVE(map, src_sqr, trg_sqr) NCH_RMVFLG(map, src_sqr); NCH_SETFLG(map, trg_sqr);

typedef struct{
    cuint64 W_Pawns;
    cuint64 B_Pawns;
    cuint64 W_Knights;
    cuint64 B_Knights;
    cuint64 W_Bishops;
    cuint64 B_Bishops;
    cuint64 W_Rooks;
    cuint64 B_Rooks;
    cuint64 W_Queens;
    cuint64 B_Queens;
    cuint64 W_King;
    cuint64 B_King;

    cuint64 White_Map;
    cuint64 Black_Map;
    cuint64 All_Map;

    int flags;
    cuint8 castle_flags;

    cuint64 possible_moves[64];

    _NCH_Ht* GameDict;

    int fifty_count;
    int move_count;
}CBoard;

#define NCH_CF_WHITE_OO (cuint8)1
#define NCH_CF_WHITE_OOO (cuint8)2
#define NCH_CF_BLACK_OO (cuint8)4
#define NCH_CF_BLACK_OOO (cuint8)8
#define NCH_CF_COULD_WHITE_OO (cuint8)16
#define NCH_CF_COULD_WHITE_OOO  (cuint8)32
#define NCH_CF_COULD_BLACK_OO (cuint8)64
#define NCH_CF_COULD_BLACK_OOO (cuint8)128

const cuint8 NCH_CF_COULD_OO = NCH_CF_COULD_WHITE_OO | NCH_CF_COULD_BLACK_OO;
const cuint8 NCH_CF_COULD_OOO = NCH_CF_COULD_WHITE_OOO | NCH_CF_COULD_BLACK_OOO;

const cuint8 NCH_CF_MASK_COULDCASTLE = NCH_CF_COULD_OO | NCH_CF_COULD_OOO;
#define NCH_CF_RESET_COULDCASLTE(board) NCH_RMVFLG(board->castle_flags, NCH_CF_MASK_COULDCASTLE)

const cuint64 NCH_KING_CASTLE_SQUARES = NCH_G1 | NCH_C1 | NCH_G8 | NCH_C8 ; 

#define NCH_B_MASKPAWNCOl 0x0000000F
#define NCH_B_PAWNMOVED 0x00000010
#define NCH_B_PAWNMOVED2SQR 0x00000020
#define NCH_B_ENPASSANT 0x00000040
#define NCH_B_CAPTURE 0x00000080
#define NCH_B_CHECK 0x00000100
#define NCH_B_CHECKMATE 0x00000800
#define NCH_B_STALEMATE 0x00001000
#define NCH_B_THREEFOLD 0x00002000
#define NCH_B_FIFTYMOVES 0x00004000
#define NCH_B_GAMEEND 0x00008000
#define NCH_B_DRAW 0x00010000
#define NCH_B_WHITEWIN 0x00020000
#define NCH_B_WHITETURN 0x00040000

#define NCH_B_IS_PAWNMOVED(board) NCH_CHKFLG(board->flags, NCH_B_PAWNMOVED)
#define NCH_B_IS_PAWNMOVED2SQR(board) NCH_CHKFLG(board->flags, NCH_B_PAWNMOVED2SQR)
#define NCH_B_IS_ENPASSANT(board) NCH_CHKFLG(board->flags, NCH_B_ENPASSANT)
#define NCH_B_IS_CAPTURE(board) NCH_CHKFLG(board->flags, NCH_B_CAPTURE)
#define NCH_B_IS_CHECK(board) NCH_CHKFLG(board->flags, NCH_B_CHECK)
#define NCH_B_IS_CHECKMATE(board) NCH_CHKFLG(board->flags, NCH_B_CHECKMATE)
#define NCH_B_IS_STALEMATE(board) NCH_CHKFLG(board->flags, NCH_B_STALEMATE)
#define NCH_B_IS_THREEFOLD(board) NCH_CHKFLG(board->flags, NCH_B_THREEFOLD)
#define NCH_B_IS_FIFTYMOVES(board) NCH_CHKFLG(board->flags, NCH_B_FIFTYMOVES)
#define NCH_B_IS_GAMEEND(board) NCH_CHKFLG(board->flags, NCH_B_GAMEEND)
#define NCH_B_IS_DRAW(board) NCH_CHKFLG(board->flags, NCH_B_DRAW)
#define NCH_B_IS_WHITEWIN(board) NCH_CHKFLG(board->flags, NCH_B_WHITEWIN)
#define NCH_B_IS_BLACKWIN(board) !NCH_B_IS_WHITEWIN(board)
#define NCH_B_IS_WHITETURN(board) NCH_CHKFLG(board->flags, NCH_B_WHITETURN)
#define NCH_B_IS_BLACKTURN(board) !NCH_B_IS_WHITETURN(board)

#define _NCH_B_SET_PAWNCOL(borad, col) NCH_SETFLG(board->flags, col)
#define NCH_B_GET_PAWNCOL(borad) (board->flags & NCH_B_MASKPAWNCOl)

#define NCH_B_GET_WHITEMAP(board) (board->W_Pawns | board->W_Knights | board->W_Bishops | board->W_Rooks | board->W_Queens | board->W_King)
#define NCH_B_GET_BLACKMAP(board) (board->B_Pawns | board->B_Knights | board->B_Bishops | board->B_Rooks | board->B_Queens | board->B_King)

#define NCH_B_TURN(board) NCH_B_IS_WHITETURN(board) ? NCH_WHITE : NCH_BLACK
#define NCH_B_RAISE_WINNER(board, winner) winner == WHITE ? NCH_SETFLG(board->flags, NCH_B_WHITEWIN) : NCH_RMVFLG(board->flags, NCH_B_WHITEWIN)
#define NCH_B_SET_WHITETURN(board) NCH_SETFLG(board->flags, NCH_B_WHITETURN)
#define NCH_B_SET_BLACKTURN(board) NCH_RMVFLG(board->flags, NCH_B_WHITETURN)

#define NCH_B_STRING_SIZE 73

const int NCH_B_MASK_GAMEACTIONS = NCH_B_MASKPAWNCOl | NCH_B_PAWNMOVED | NCH_B_PAWNMOVED2SQR
                                 | NCH_B_ENPASSANT | NCH_B_CAPTURE | NCH_B_CHECK;

#define NCH_B_RESET_GAMEACTIONS(board) NCH_RMVFLG(board->flags, NCH_B_MASK_GAMEACTIONS);

#endif