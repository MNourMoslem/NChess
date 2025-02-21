#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"

typedef enum{
    MoveType_Normal,
    MoveType_Promotion,
    MoveType_Enpassant,
    MoveType_Castle,
}MoveType;

typedef uint16 Move;

#define _Move_New(from_, to_, promotion_piece, move_type)\
Move_ASSIGN_FROM(from_)\
| Move_ASSIGN_TO(to_)\
| Move_ASSIGN_PRO_PIECE(promotion_piece - 1)\
| Move_ASSIGN_TYPE(move_type)

Move
Move_FromString(const char* move_str);

void
Move_Print(Move move);

int
Move_AsString(Move move, char* dst);

void
Move_PrintAll(Move* move, int nmoves);

#define Move_ASSIGN_FROM(from_) ((from_))
#define Move_ASSIGN_TO(to_) ((to_) << 6)
#define Move_ASSIGN_PRO_PIECE(pro_piece) ((pro_piece) << 12)
#define Move_ASSIGN_TYPE(type) ((type) << 14)

#define Move_FROM(move) ((move) & 0x3F)
#define Move_TO(move) (((move) >> 6) & 0x3F)
#define Move_PRO_PIECE(move) ((((move) >> 12) & 0x3) + 1)
#define Move_TYPE(move) (((move) >> 14) & 0x3)

#endif //NCHESS_SRC_MOVE_H