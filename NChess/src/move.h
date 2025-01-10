#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"

typedef uint32 Move;

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

void
Move_PrintAll(Move* move, int nmoves);

#define Move_ASSIGN_FROM(from_) ((from_))
#define Move_ASSIGN_TO(to_) ((to_) << 6)
#define Move_ASSIGN_CASTLE(castle) ((castle) << 12)
#define Move_ASSIGN_PRO_PIECE(pro_piece) ((pro_piece) << 16)

#define Move_FROM(move) ((move) & 0x3F)
#define Move_TO(move) (((move) >> 6) & 0x3F)
#define Move_CASTLE(move) (((move) >> 12) & 0xF)
#define Move_PRO_PIECE(move) (((move) >> 16) & 0xF)

#endif //NCHESS_SRC_MOVE_H