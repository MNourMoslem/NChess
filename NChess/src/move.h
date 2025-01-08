#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"

Move
Move_New(Square from_, Square to_, uint8 castle, Piece promotion);

void
Move_Parse(Move move, Square* from_, Square* to_, uint8* castle, Piece* promotion);

int
Move_ParseFromString(char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle);

Move
Move_FromString(char* move);

void
Move_Print(Move move);

void
Move_AsString(Move move, char* dst);

#endif //NCHESS_SRC_MOVE_H