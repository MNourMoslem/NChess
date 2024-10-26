#ifndef NCHESS_CORE_H
#define NCHESS_CORE_H

#include <math.h>
#include "types.h"

#ifdef __GNUC__
    #define NCH_CLZLL(x) __builtin_clzll(x)
    #define NCH_CTZLL(x) __builtin_ctzll(x)
    #define NCH_POPCOUNT(x) __builtin_popcountll(x)
#else
    #define NCH_CLZLL(x) custom_clzll(x)
    #define NCH_CTZLL(x) custom_ctzll(x)
    #define NCH_POPCOUNT(x) custom_popcount(x)

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

    static inline int custom_popcount(unsigned int x) {
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

#define NCH_CHKFLG(x, flag) ((x & flag) == flag)
#define NCH_RMVFLG(x, flag) (x &= ~flag)
#define NCH_SETFLG(x, flag) (x |= flag)
#define NCH_CHKUNI(x, flag) ((x & flag) != 0)

#define NCH_SQR(idx) (1ull << idx)
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

#define NCH_ROW1 0x00000000000000FFull
#define NCH_ROW2 0x000000000000FF00ull
#define NCH_ROW3 0x0000000000FF0000ull
#define NCH_ROW4 0x00000000FF000000ull
#define NCH_ROW5 0x000000FF00000000ull
#define NCH_ROW6 0x0000FF0000000000ull
#define NCH_ROW7 0x00FF000000000000ull
#define NCH_ROW8 0xFF00000000000000ull

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

#define NCH_PROMOTION_TO_NONE 0
#define NCH_PROMOTION_TO_QUEEN 1
#define NCH_PROMOTION_TO_ROOK 2
#define NCH_PROMOTION_TO_KNIGHT 4
#define NCH_PROMOTION_TO_BISHOP 8

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

    cuint64 possible_moves[64];

    int count;
    int flags;
}CBoard;

#define NCH_B_MASKPAWNCOl 0x0000000F
#define NCH_B_PAWNMOVED 0x00000010
#define NCH_B_PAWNMOVED2SQR 0x00000020
#define NCH_B_ENPASSANT 0x00000040
#define NCH_B_CAPTURE 0x00000080
#define NCH_B_CHECK 0x00000100
#define NCH_B_CHECKMATE 0x00000200
#define NCH_B_STALEMATE 0x00000400
#define NCH_B_THREEFOLD 0x00000800
#define NCH_B_FIFTYMOVES 0x00001000
#define NCH_B_GAMEEND 0x00002000
#define NCH_B_DRAW 0x00004000
#define NCH_B_WHITEWIN 0x00008000
#define NCH_B_WHITETURN 0x00010000

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

#endif