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

    MoveType_NB,

    MoveType_Null = -1,
} MoveType;

#define MoveType_IsValid(type) (type >= MoveType_Normal && type <= MoveType_Castle)

typedef uint16 Move;
#define Move_NULL 0xffff

#define Move_ASSIGN_FROM(from_) (Move)((from_))
#define Move_ASSIGN_TO(to_) (Move)((to_) << 6)
#define Move_ASSIGN_PRO_PIECE(pro_piece) (Move)((pro_piece) << 12)
#define Move_ASSIGN_TYPE(type) (Move)((type) << 14)

#define Move_REMOVE_FROM(move) (Move)(move & 0xffc0)
#define Move_REMOVE_TO(move) (Move)(move & 0xf03f)
#define Move_REMOVE_PRO_PIECE(move) (Move)(move & 0xcfff)
#define Move_REMOVE_TYPE(move) (Move)(move & 0x3fff)

#define Move_REASSAGIN_FROM(move, from_) (Move_REMOVE_FROM(move) | Move_ASSIGN_FROM(from_))
#define Move_REASSAGIN_TO(move, to_) (Move_REMOVE_TO(move) | Move_ASSIGN_TO(to_))
#define Move_REASSAGIN_PRO_PIECE(move, pro_piece) (Move_REMOVE_PRO_PIECE(move) | Move_ASSIGN_PRO_PIECE(pro_piece))
#define Move_REASSAGIN_TYPE(move, type) (Move_REMOVE_TYPE(move) | Move_ASSIGN_TYPE(type))

#define Move_FROM(move) ((move) & 0x3F)
#define Move_TO(move) (((move) >> 6) & 0x3F)
#define Move_PRO_PIECE(move) ((((move) >> 12) & 0x3) + NCH_Knight)
#define Move_TYPE(move) (((move) >> 14) & 0x3)

#define Move_SQUARES_MASK 0x0fff
#define Move_SAME_SQUARES(m1, m2) ((m1 & Move_SQUARES_MASK) == (m2 & Move_SQUARES_MASK))
#define Move_IsValidSquares(m) (is_valid_square(Move_FROM(m)) && is_valid_square(Move_TO(m)))

// A macro to create a Move. It is faster but not safe
// if the given parameters are incorrect. Use Move_New for safer usage.
#define _Move_New(from_, to_, promotion_piece, move_type) \
    (Move)(Move_ASSIGN_FROM(from_) | \
           Move_ASSIGN_TO(to_) | \
           Move_ASSIGN_PRO_PIECE(promotion_piece - NCH_Knight) | \
           Move_ASSIGN_TYPE(move_type))

#define Move_IsValid(move) ((move) != Move_NULL)
#define Move_IsNormal(move) (Move_TYPE(move) == MoveType_Normal)
#define Move_IsPromotion(move) (Move_TYPE(move) == MoveType_Promotion)
#define Move_IsEnPassant(move) (Move_TYPE(move) == MoveType_EnPassant)
#define Move_IsCastle(move) (Move_TYPE(move) == MoveType_Castle)

// Returns a new move if it is valid, and 0 if not.
Move 
Move_New(Square from_, Square to_, MoveType type, PieceType promotion_piece);

// Converts a UCI string into a Move object. The move type defaults to 
// MoveType_Normal unless it is a promotion move. 
// 
// Note: This function does not detect MoveType_Castle or MoveType_EnPassant;
// it is not responsible for determining these special move types.
//
// If the promotion piece is not a valid character ('q', 'r', 'b', 'k'), 
// it defaults to NCH_Queen.
//
// Returns: 
// - A valid Move object if the input string represents a valid move.
// - Move_NULL if the move is invalid.
Move 
Move_FromString(const char* move_str);

// Converts a UCI string into a Move object while explicitly specifying its type.
//
// Returns: 
// - A Move object constructed from the given UCI string and MoveType.
Move
Move_FromStringAndType(const char* move_str, MoveType type);

// Prints a move to the console.
void 
Move_Print(Move move);

// Converts a Move to a UCI string.
// Returns 0 on success and -1 on failure.
int 
Move_AsString(Move move, char* dst);

// Prints all moves in a given buffer.
void 
Move_PrintAll(const Move* move, int nmoves);

#endif // NCHESS_SRC_MOVE_H
