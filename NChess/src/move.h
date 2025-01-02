#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"
#include "board.h"

NCH_STATIC_INLINE int
is_valid_move(Board* board, Square from_, Square to_){
    return NCH_CHKFLG(board->moves[from_], NCH_SQR(to_));
}

uint32
make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle);

void
undo_move(Board* board, Side side, uint32 move);

uint32
save_move(Square from_, Square to_, uint8 castle, Piece promotion, int is_promotion, int is_enpassant, Piece captured_piece);

void
parse_move(uint32 move, Square* from_, Square* to_, uint8* castle, Piece* promotion, int* is_promotion, int* is_enpassant, Piece* captured_piece);

#endif //NCHESS_SRC_MOVE_H