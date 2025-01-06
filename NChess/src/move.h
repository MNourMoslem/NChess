#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"
#include "board.h"

NCH_STATIC_INLINE int
Move_IsValid(Board* board, Square from_, Square to_){
    return NCH_CHKFLG(board->moves[from_], NCH_SQR(to_));
}

Piece
make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle);

void
undo_move(Board* board, Side side, Move move, int is_enpassant,
         int is_promotion, Piece last_captured_piece);

Move
Move_New(Square from_, Square to_, uint8 castle, Piece promotion);

void
Move_Parse(Move move, Square* from_, Square* to_, uint8* castle, Piece* promotion);

int
Move_ParseFromString(Board* board, char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle);

Move
Move_FromString(Board* board, char* move);

void
Move_Print(Move move);

#endif //NCHESS_SRC_MOVE_H