/*
    move.h

    This file contains the typedef of Move. It also contains function
    declarations related to Move, such as creating and printing moves, etc.
*/

#ifndef NCHESS_SRC_MOVE_H
#define NCHESS_SRC_MOVE_H

#include "core.h"
#include "types.h"
#include "config.h"

typedef enum {
    MoveType_Normal,
    MoveType_Promotion,
    MoveType_EnPassant,
    MoveType_Castle,
} MoveType;

typedef uint16 Move;

#define Move_ASSIGN_FROM(from_) ((from_))
#define Move_ASSIGN_TO(to_) ((to_) << 6)
#define Move_ASSIGN_PRO_PIECE(pro_piece) ((pro_piece) << 12)
#define Move_ASSIGN_TYPE(type) ((type) << 14)

#define Move_FROM(move) ((move) & 0x3F)
#define Move_TO(move) (((move) >> 6) & 0x3F)
#define Move_PRO_PIECE(move) ((((move) >> 12) & 0x3) + 1)
#define Move_TYPE(move) (((move) >> 14) & 0x3)

// A macro to create a Move. It is faster but not safe
// if the given parameters are incorrect. Use Move_New for safer usage.
#define _Move_New(from_, to_, promotion_piece, move_type) \
    Move_ASSIGN_FROM(from_) | \
    Move_ASSIGN_TO(to_) | \
    Move_ASSIGN_PRO_PIECE(promotion_piece - 1) | \
    Move_ASSIGN_TYPE(move_type)


// Returns a new move if it is valid, and 0 if not.
Move 
Move_New(Square from_, Square to_, MoveType type, Piece promotion_piece);

// Returns a move from a UCI string. The move type is MoveType_Normal
// by default unless it is a promotion move.
// In other words, MoveType_Castle and MoveType_EnPassant cannot be
// detected, and it is not the responsibility of this function.
// Also if promotion piece is not a valid char (q, r, b, k) the piece 
// would be set to NCH_Queen by default.
// Returns 0 if the move is not valid.
Move 
Move_FromString(const char* move_str);

// Prints a move to the console.
void 
Move_Print(Move move);

// Converts a Move to a UCI string.
// Returns 0 on success and -1 on failure.
int 
Move_AsString(Move move, char* dst);

// Prints all moves in a given buffer.
void 
Move_PrintAll(Move* move, int nmoves);

#endif // NCHESS_SRC_MOVE_H
