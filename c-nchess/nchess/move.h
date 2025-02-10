#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"

typedef uint32 Move;

#define Move_New(from_, to_, promotion, castle, is_enp, is_pro)\
Move_ASSIGN_FROM(from_) | Move_ASSIGN_TO(to_)\
| Move_ASSIGN_CASTLE(castle) | Move_ASSIGN_PRO_PIECE(promotion)\
| Move_ASSIGN_IS_ENP(is_enp) |Move_ASSIGN_IS_PRO(is_pro)

void
Move_Parse(Move move, Square* from_, Square* to_, uint8* castle, Piece* promotion);

int
Move_ParseFromString(char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle);

Move
Move_FromString(char* move);

void
Move_Print(Move move);

int
Move_AsString(Move move, char* dst);

void
Move_PrintAll(Move* move, int nmoves);

#define Move_ASSIGN_FROM(from_) ((from_))
#define Move_ASSIGN_TO(to_) ((to_) << 6)
#define Move_ASSIGN_CASTLE(castle) ((castle) << 12)
#define Move_ASSIGN_PRO_PIECE(pro_piece) ((pro_piece) << 16)
#define Move_ASSIGN_IS_ENP(is_enp) ((is_enp) << 20)
#define Move_ASSIGN_IS_PRO(is_pro) ((is_pro) << 21)

#define Move_FROM(move) ((move) & 0x3F)
#define Move_TO(move) (((move) >> 6) & 0x3F)
#define Move_CASTLE(move) (((move) >> 12) & 0xF)
#define Move_PRO_PIECE(move) (((move) >> 16) & 0xF)
#define Move_IS_ENP(move) (((move) >> 20) & 0x1)
#define Move_IS_PRO(move) (((move) >> 21) & 0x1)

#endif //NCHESS_SRC_MOVE_H