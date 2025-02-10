#ifndef NCHESS_SRC_BITBOARD_H
#define NCHESS_SRC_BITBOARD_H

#include "core.h"
#include "config.h"
#include "types.h"

typedef enum{
    NCH_RS,
    NCH_BS,
}SliderType;

extern uint64 PawnAttacks[2][NCH_SQUARE_NB];               // 128 
extern uint64 KnightAttacks[NCH_SQUARE_NB];                // 64
extern uint64 KingAttacks[NCH_SQUARE_NB];                  // 64

extern uint64 BetweenTable[NCH_SQUARE_NB][NCH_SQUARE_NB];  // 4,096

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