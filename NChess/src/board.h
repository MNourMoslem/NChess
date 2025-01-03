#ifndef NCHESS_SRC_BOARD_H
#define NCHESS_SRC_BOARD_H

#include "core.h"
#include "types.h"
#include "config.h"
#include "movelist.h"

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
    Piece piecetables[NCH_SIDES_NB][NCH_SQUARE_NB];

    uint64 castles;
    int flags;

    Square en_passant_idx;
    uint64 en_passant_map;
    uint64 en_passant_trg;

    uint64 moves[NCH_SQUARE_NB];

    uint32 last_move;

    MoveList* movelist;

    int nmoves;
    int fifty_counter;
}Board;

#define Board_WHITE_OCC(board) (board)->occupancy[NCH_White]
#define Board_BLACK_OCC(board) (board)->occupancy[NCH_Black]
#define Board_ALL_OCC(board) (board)->occupancy[NCH_SIDES_NB]

#define Board_WHITE_TABLE(board) (board)->piecetables[NCH_White]
#define Board_BLACK_TABLE(board) (board)->piecetables[NCH_Black]

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

#define Board_WHITE_PIECE(board, idx) (board)->piecetables[NCH_White][idx]
#define Board_BLACK_PIECE(board, idx) (board)->piecetables[NCH_Black][idx]

#define Board_PAWNMOVED (1 << 0)
#define Board_ENPASSANT (1 << 1)
#define Board_CAPTURE (1 << 2)
#define Board_CHECK (1 << 3)
#define Board_DOUBLECHECK (1 << 4)
#define Board_CHECKMATE (1 << 5)
#define Board_STALEMATE (1 << 6)
#define Board_THREEFOLD (1 << 7)
#define Board_FIFTYMOVES (1 << 8)
#define Board_GAMEEND (1 << 9)
#define Board_DRAW (1 << 10)
#define Board_WIN (1 << 11)
#define Board_TURN (1 << 12)

#define Board_IS_PAWNMOVED(board) NCH_CHKFLG(board->flags, Board_PAWNMOVED)
#define Board_IS_DOUBLECHECK(board) NCH_CHKFLG(board->flags, Board_DOUBLECHECK)
#define Board_IS_ENPASSANT(board) NCH_CHKFLG(board->flags, Board_ENPASSANT)
#define Board_IS_CAPTURE(board) NCH_CHKFLG(board->flags, Board_CAPTURE)
#define Board_IS_CHECK(board) NCH_CHKFLG(board->flags, Board_CHECK)
#define Board_IS_CHECKMATE(board) NCH_CHKFLG(board->flags, Board_CHECKMATE)
#define Board_IS_STALEMATE(board) NCH_CHKFLG(board->flags, Board_STALEMATE)
#define Board_IS_THREEFOLD(board) NCH_CHKFLG(board->flags, Board_THREEFOLD)
#define Board_IS_FIFTYMOVES(board) NCH_CHKFLG(board->flags, Board_FIFTYMOVES)
#define Board_IS_GAMEEND(board) NCH_CHKFLG(board->flags, Board_GAMEEND)
#define Board_IS_DRAW(board) NCH_CHKFLG(board->flags, Board_DRAW)
#define Board_IS_WHITEWIN(board) NCH_CHKFLG(board->flags, Board_WIN)
#define Board_IS_BLACKWIN(board) !Board_IS_WHITEWIN(board)
#define Board_IS_WHITETURN(board) NCH_CHKFLG(board->flags, Board_TURN)
#define Board_IS_BLACKTURN(board) !Board_IS_WHITETURN(board)

#define Board_GET_SIDE(board) (Board_IS_WHITETURN(board) ? NCH_White : NCH_Black)

#define Board_CASTLE_WK 1
#define Board_CASTLE_WQ 2
#define Board_CASTLE_BK 4
#define Board_CASTLE_BQ 8

#define Board_IS_CASTLE_WK(board) NCH_CHKFLG(board->castles, Board_CASTLE_WK)
#define Board_IS_CASTLE_WQ(board) NCH_CHKFLG(board->castles, Board_CASTLE_WQ)
#define Board_IS_CASTLE_BK(board) NCH_CHKFLG(board->castles, Board_CASTLE_BK)
#define Board_IS_CASTLE_BQ(board) NCH_CHKFLG(board->castles, Board_CASTLE_BQ)

Board*
Board_New();

void
Board_Free(Board* board);

void
Board_Init(Board* board);

void
Board_InitEmpty(Board* board);

void
Board_SetSquare(Board* board, Side side, Piece ptype, int sqr_idx);

void
Board_SetBitboard(Board* board, Side side, Piece ptype, uint64 bb);

int
Board_IsCheck(Board* board);

void
Board_Update(Board* board);

void
Board_Step(Board* board, char* move);

void
Board_Undo(Board* board);

#endif