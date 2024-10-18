#ifndef NCHESS_SRC_INCLUDE_BOARD_H
#define NCHESS_SRC_INCLUDE_BOARD_H

#include <math.h>
#include "types.h"

#ifdef __GNUC__
    #define NCH_CLZLL(x) __builtin_clzll(x)
    #define NCH_CTZLL(x) __builtin_ctzll(x)
#else
    #define NCH_CLZLL(x) custom_clzll(x)
    #define NCH_CTZLL(x) custom_ctzll(x)

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
#endif

#define NCH_WHITE 1
#define NCH_BLACK 0

const char NCH_PIECES[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'q', 'n', 'b', 'r', 'q', 'k'};
const char NCH_COLUMNS[8] = {'h' ,'g', 'f', 'e', 'd', 'c', 'b', 'a'};

#define NCH_CHKFLG(x, flag) ((x & flag) == flag)
#define NCH_RMVFLG(x, flag) (x &= ~flag)
#define NCH_SETFLG(x, flag) (x |= flag)

#define NCH_SQR(idx) (1ull << idx)
#define NCH_GETCOL(square) (square & 8ull)
#define NCH_GETROW(square) (square / 8ull)

#ifdef __GNUC__
    #define NCH_SQRIDX(square) NCH_CTZLL(x)
#else
    #define NCH_SQRIDX(square) (cuint64)log2((double)square)
#endif

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

#define NCH_MKMOVE(map, src_sqr, trg_sqr) NCH_RMVFLG(map, src_sqr); NCH_SETFLG(trg_sqr)

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

typedef struct{
    cuint64 W_Pawns;
    cuint64 B_Pawns;
    cuint64 W_Kinghts;
    cuint64 B_Kinghts;
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

    int count;
    cuint16 flags;
}CBoard;

#define NCH_BF_PAWNMOVED 0x0001
#define NCH_BF_PAWNMOVED2SQR 0x0002
#define NCH_BF_ENPASSANT 0x0004
#define NCH_BF_CAPTURE 0x0008
#define NCH_BF_CHECK 0x0010
#define NCH_BF_CHECKMATE 0x0020
#define NCH_BF_STALEMATE 0x0040
#define NCH_BF_THREEFOLD 0x0080
#define NCH_BF_FIFTYMOVES 0x0100
#define NCH_BF_GAMEEND 0x0200
#define NCH_BF_DRAW 0x0400
#define NCH_BF_WHITEWIN 0x0800
#define NCH_BF_WHITETURN 0x1000

#define NCH_IS_PAWNMOVED(board) NCH_CHKFLG(board->flags, NCH_BF_PAWNMOVED)
#define NCH_IS_PAWNMOVED2SQR(board) NCH_CHKFLG(board->flags, NCH_BF_PAWNMOVED2SQR)
#define NCH_IS_ENPASSANT(board) NCH_CHKFLG(board->flags, NCH_BF_ENPASSANT)
#define NCH_IS_CAPTURE(board) NCH_CHKFLG(board->flags, NCH_BF_CAPTURE)
#define NCH_IS_CHECK(board) NCH_CHKFLG(board->flags, NCH_BF_CHECK)
#define NCH_IS_CHECKMATE(board) NCH_CHKFLG(board->flags, NCH_BF_CHECKMATE)
#define NCH_IS_STALEMATE(board) NCH_CHKFLG(board->flags, NCH_BF_STALEMATE)
#define NCH_IS_THREEFOLD(board) NCH_CHKFLG(board->flags, NCH_BF_THREEFOLD)
#define NCH_IS_FIFTYMOVES(board) NCH_CHKFLG(board->flags, NCH_BF_FIFTYMOVES)
#define NCH_IS_GAMEEND(board) NCH_CHKFLG(board->flags, NCH_BF_GAMEEND)
#define NCH_IS_DRAW(board) NCH_CHKFLG(board->flags, NCH_BF_DRAW)
#define NCH_IS_WHITEWIN(board) NCH_CHKFLG(board->flags, NCH_BF_WHITEWIN)
#define NCH_IS_BLACKWIN(board) !NCH_IS_WHITEWIN(board)
#define NCH_IS_WHITETURN(board) NCH_CHKFLG(board->flags, NCH_BF_WHITETURN)
#define NCH_IS_BLACKTURN(board) !NCH_IS_WHITETURN(board)

#define NCH_TURN(board) NCH_IS_WHITETURN ? WHITE : BLACK
#define NCH_RAISE_WINNER(board, winner) winner == WHITE ? NCH_SETFLG(board->flags, NCH_BF_WHITEWIN) : NCH_RMVFLG(board->flags, NCH_BF_WHITEWIN)

#endif