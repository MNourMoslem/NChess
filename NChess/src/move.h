#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"
#include "board.h"

void
make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle);

NCH_STATIC_INLINE int
is_valid_move(Board* board, Square from_, Square to_){
    return NCH_CHKFLG(board->moves[from_], NCH_SQR(to_));
}

#endif //NCHESS_SRC_MOVE_H