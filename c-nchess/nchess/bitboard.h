/*
    bitboard.h

    Contains all the bitboard tables needed for nchess starting from pawn attacks
    to magic bitboards.
    
    All tables are declared as extern and are defined in bitboard.c

    Most of the tables inspired from Stockfish mainly (you could see the old_src
    folder in the repo to see the old way of how nchess was implemented).
    You could find the Stockfish repo here:
    https://www.github.com/official-stockfish/Stockfish

    Also the Monkey King youtube channel was a great help in understanding the magic
    bitboards and how to implement them. His youtube channel called Chess Programming
    is a great resource for anyone who wants to learn how to implement a chess engine.
    the link to his channel is:
    https://www.youtube.com/@chessprogramming591
*/

#ifndef NCHESS_SRC_BITBOARD_H
#define NCHESS_SRC_BITBOARD_H

#include "core.h"
#include "config.h"
#include "types.h"

// This enum is used be Magics, ReleventSquares, and SlidersAttackMask tables
// to differentiate between rooks and bishops.
// the names NCH_RS and NCH_BS are not the best names but this is how it is
// for now.
typedef enum{
    NCH_RS,
    NCH_BS,
}SliderType;


// Attack tables for non-sliding pieces (pawns, knights, and kings)
extern uint64 PawnAttacks[2][NCH_SQUARE_NB];               // 128 
extern uint64 KnightAttacks[NCH_SQUARE_NB];                // 64
extern uint64 KingAttacks[NCH_SQUARE_NB];                  // 64

// Table representing the squares between two squares
// bitboard returned will have the bits set between the two squares
// This is helpful for move generation to detect piece attacking the king
// the bitboard includes the trg square but not the src square. This
// trick helpful to detect the possible squares pieces can move to when
// the king is in check.
extern uint64 BetweenTable[NCH_SQUARE_NB][NCH_SQUARE_NB];  // 4,096

/*
    The rest tabels are for sliding pieces (rooks and bishops).
    Would like to thank Chess Programming youtube channel for the great
    explanation on how to implement magic bitboards.
*/
extern uint64 Magics[2][NCH_SQUARE_NB];                    // 128
extern int ReleventSquares[2][NCH_SQUARE_NB];              // 128
extern uint64 SlidersAttackMask[2][NCH_SQUARE_NB];         // 128

extern uint64 RookTable[NCH_SQUARE_NB][4096];              // 262,144
extern uint64 BishopTable[NCH_SQUARE_NB][512];             // 32,768

NCH_STATIC_FINLINE uint64
bb_between(int from_, int to_){
    return BetweenTable[from_][to_];
}

NCH_STATIC_FINLINE uint64
bb_pawn_attacks(Side side, int sqr_idx){
    return PawnAttacks[side][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_knight_attacks(int sqr_idx){
    return KnightAttacks[sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_king_attacks(int sqr_idx){
    return KingAttacks[sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_rook_mask(int sqr_idx){
    return SlidersAttackMask[NCH_RS][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_bishop_mask(int sqr_idx){
    return SlidersAttackMask[NCH_BS][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_queen_mask(int sqr_idx){
    return bb_rook_mask(sqr_idx) | bb_bishop_mask(sqr_idx);
}

NCH_STATIC_FINLINE int
bb_rook_relevant(int sqr_idx){
    return ReleventSquares[NCH_RS][sqr_idx];
}

NCH_STATIC_FINLINE int
bb_bishop_relevant(int sqr_idx){
    return ReleventSquares[NCH_BS][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_rook_magic(int sqr_idx){
    return Magics[NCH_RS][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_bishop_magic(int sqr_idx){
    return Magics[NCH_BS][sqr_idx];
}

NCH_STATIC_FINLINE uint64
bb_rook_attacks(int sqr_idx, uint64 block){
    block &= bb_rook_mask(sqr_idx);
    block *= bb_rook_magic(sqr_idx);
    block >>= 64 - bb_rook_relevant(sqr_idx);
    return RookTable[sqr_idx][block];
}

NCH_STATIC_FINLINE uint64
bb_bishop_attacks(int sqr_idx, uint64 block){
    block &= bb_bishop_mask(sqr_idx);
    block *= bb_bishop_magic(sqr_idx);
    block >>= 64 - bb_bishop_relevant(sqr_idx);
    return BishopTable[sqr_idx][block];
}

NCH_STATIC_FINLINE uint64
bb_queen_attacks(int sqr_idx, uint64 block){
    return bb_rook_attacks(sqr_idx, block) | bb_bishop_attacks(sqr_idx, block);
}

void
NCH_InitBitboards();

#endif