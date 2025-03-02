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

#define Move_ASSIGN_FROM(from_) ((Move)(from_))
#define Move_ASSIGN_TO(to_) ((Move)((to_) << 6))
#define Move_ASSIGN_PRO_PIECE(pro_piece) ((Move)((pro_piece) << 12))
#define Move_ASSIGN_TYPE(type) ((Move)((type) << 14))

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
#define _Move_New(from_, to_, pro_type, move_type) \
    (Move)(Move_ASSIGN_FROM(from_) | \
           Move_ASSIGN_TO(to_) | \
           Move_ASSIGN_PRO_PIECE((pro_type) - NCH_Knight) | \
           Move_ASSIGN_TYPE(move_type))

#define Move_IsValid(move) ((move) != Move_NULL)
#define Move_IsNormal(move) (Move_TYPE(move) == MoveType_Normal)
#define Move_IsPromotion(move) (Move_TYPE(move) == MoveType_Promotion)
#define Move_IsEnPassant(move) (Move_TYPE(move) == MoveType_EnPassant)
#define Move_IsCastle(move) (Move_TYPE(move) == MoveType_Castle)

// Returns a new move based on the given parameters.  
// from_ : Source square (0-63).  
// to_ : Target square (0-63).  
// type : Move type (Normal, Promotion, EnPassant, Castle).  
// pro_piece_type : Promotion piece type (Queen, Rook, Bishop, Knight).  
//  
// All given parameters will be masked inside the function, ensuring that  
// the function always returns a valid object, though not necessarily a valid move.  
// For example, 'from_' will be processed as 'from_ & 0x3F'  
// because squares from 0 to 63 only require 6 bits.  
//  
// If the user inputs an invalid square, such as 70, it will be masked  
// (70 & 0x3F), resulting in 6 as the new from_ square.  
// However, if the input is already within the valid range (0-63),  
// masking will have no effect.
Move  
Move_New(Square from_, Square to_, MoveType type, PieceType pro_piece_type);  


// Converts a UCI string into the given destination Move object.  
// The move type defaults to MoveType_Normal unless it is a promotion move.  
//  
// Note: This function does not detect MoveType_Castle or MoveType_EnPassant;  
// it is not responsible for determining these special move types.  
// Use Move_FromStringAndType if you want to set the type manually.  
//  
// If the promotion piece is not a valid character ('q', 'r', 'b', 'k'),  
// it defaults to NCH_Queen.  
//  
// Returns 1 if the move is valid and 0 otherwise.  
int  
Move_FromString(const char* move_str, Move* dst_move);  


// Converts a UCI string into the given destination Move object with the  
// move type specified by the function.  
//  
// Returns 1 if the move is valid and 0 otherwise.  
int  
Move_FromStringAndType(const char* move_str, Move* dst_move, MoveType type);  


// Converts a Move to a UCI string.
// Returns 0 on success and -1 on failure.
int 
Move_AsString(Move move, char* dst);


// Prints a move to the console.
void 
Move_Print(Move move);


// Prints all moves in a given buffer without new line between each move.
void 
Move_PrintAll(const Move* move, int nmoves);

#endif // NCHESS_SRC_MOVE_H
