#ifndef NCHESS_SRC_GENERATE_UTILS_H
#define NCHESS_SRC_GENERATE_UTILS_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"

#include "generate_utils.h"
#include "bitboard.h"

NCH_STATIC_INLINE uint64
get_checkmap(Board* board, Side side, int king_idx, uint64 all_occ){
    uint64 occupancy;
    if (side == NCH_White){
        occupancy = all_occ & ~Board_WHITE_KING(board);

        return    (bb_rook_attacks(king_idx, occupancy)   & Board_BLACK_ROOKS(board)  ) 
                | (bb_bishop_attacks(king_idx, occupancy) & Board_BLACK_BISHOPS(board))
                | (bb_queen_attacks(king_idx, occupancy)  & Board_BLACK_QUEENS(board) )
                | (bb_knight_attacks(king_idx)            & Board_BLACK_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_White, king_idx)   & Board_BLACK_PAWNS(board)  );
    }
    else{
        occupancy = all_occ & ~Board_BLACK_KING(board);

        return    (bb_rook_attacks(king_idx, occupancy)   & Board_WHITE_ROOKS(board)  ) 
                | (bb_bishop_attacks(king_idx, occupancy) & Board_WHITE_BISHOPS(board))
                | (bb_queen_attacks(king_idx, occupancy)  & Board_WHITE_QUEENS(board) )
                | (bb_knight_attacks(king_idx)            & Board_WHITE_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_Black, king_idx)   & Board_WHITE_PAWNS(board)  );
    }
}

NCH_STATIC_INLINE uint64
get_allowed_squares(Board* board){
    if (!Board_IS_CHECK(board)){
        return NCH_UINT64_MAX;
    }
    int king_idx = Board_IS_WHITETURN(board) ? NCH_SQRIDX(Board_WHITE_KING(board)) : 
                                               NCH_SQRIDX(Board_BLACK_KING(board)) ;

    uint64 attackers_map = get_checkmap(board, Board_GET_SIDE(board), king_idx, Board_ALL_OCC(board));
    if (!attackers_map){
        return NCH_UINT64_MAX;
    }

    if (count_bits(attackers_map) == 1){
        return bb_between(king_idx, NCH_SQRIDX(attackers_map));
    }
    else{
        return 0ULL;
    }
}


#endif