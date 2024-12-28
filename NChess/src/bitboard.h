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
extern uint64 RelativeSquares[2][NCH_SQUARE_NB];           // 128
extern uint64 SlidersAttackMask[2][NCH_SQUARE_NB];         // 128

extern uint64 RookTable[NCH_SQUARE_NB][4096];              // 262,144
extern uint64 BishopTable[NCH_SQUARE_NB][512];             // 32,768

NCH_STATIC_INLINE uint64
bb_between(int from_, int to_){
    return BetweenTable[from_][to_];
}

NCH_STATIC_INLINE uint64
bb_pawn_attacks(Side side, int sqr_idx){
    return PawnAttacks[side][sqr_idx];
}

NCH_STATIC_INLINE uint64
bb_knight_attacks(int sqr_idx){
    return KnightAttacks[sqr_idx];
}

NCH_STATIC_INLINE uint64
bb_king_attacks(int sqr_idx){
    return KingAttacks[sqr_idx];
}

NCH_STATIC_INLINE uint64
bb_rook_mask(int sqr_idx){
    return SlidersAttackMask[NCH_RS][sqr_idx];
}

NCH_STATIC_INLINE uint64
bb_bishop_mask(int sqr_idx){
    return SlidersAttackMask[NCH_BS][sqr_idx];
}

void
InitBitboards();

#endif