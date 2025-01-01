#include "generate_utils.h"
#include "bitboard.h"

uint64
get_checkmap(Board* board, Side side, int king_idx, uint64 all_occ){
    uint64 king_sqr, occupancy;
    if (side == NCH_White){
        king_sqr = Board_WHITE_KING(board);
        occupancy = all_occ & ~king_sqr;

         return (bb_rook_attacks(king_idx, occupancy) & Board_BLACK_ROOKS(board)) 
                | (bb_bishop_attacks(king_idx, occupancy) & Board_BLACK_BISHOPS(board))
                | (bb_queen_attacks(king_idx, occupancy) & Board_BLACK_QUEENS(board))
                | (bb_knight_attacks(king_idx) & Board_BLACK_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_White, king_idx) & Board_BLACK_PAWNS(board));
    }
    else{
        king_sqr = Board_BLACK_KING(board);
        occupancy = all_occ & ~king_sqr;

        return  (bb_rook_attacks(king_idx, occupancy) & Board_WHITE_ROOKS(board)) 
                | (bb_bishop_attacks(king_idx, occupancy) & Board_WHITE_BISHOPS(board))
                | (bb_queen_attacks(king_idx, occupancy) & Board_WHITE_QUEENS(board))
                | (bb_knight_attacks(king_idx) & Board_WHITE_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_Black, king_idx) & Board_WHITE_PAWNS(board));
    }
}

uint64
get_allowed_pieces(Board* board){
    uint64 king_sqr = Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board);
    int king_idx = NCH_SQRIDX(king_sqr);

    uint64 all_occ = Board_ALL_OCC(board);
    uint64 queen_attack = bb_queen_attacks(king_idx, all_occ);

    NCH_RMVFLG(all_occ, queen_attack);
    if (Board_IS_ENPASSANT(board) && !NCH_CHKFLG(all_occ, board->en_passant_map)){
        NCH_RMVFLG(all_occ, board->en_passant_map);
    }
    
    queen_attack = bb_queen_attacks(king_idx, all_occ);

    uint64 attackers_map = get_checkmap(board, Board_GET_SIDE(board), king_idx, all_occ);
    if (!attackers_map){
        return NCH_UINT64_MAX;
    }


    uint64 not_allowed;
    if (count_bits(attackers_map) == 1){
        not_allowed = bb_between(king_idx, NCH_SQRIDX(attackers_map));
    }
    else{
        not_allowed = bb_between(king_idx, NCH_SQRIDX(get_ls1b(attackers_map)))
                    | bb_between(king_idx, NCH_SQRIDX(get_ts1b(attackers_map)));
    }

    return NCH_UINT64_MAX & ~not_allowed;
}

uint64
get_allowed_squares(Board* board){
    if (!Board_IS_CHECK(board)){
        return NCH_UINT64_MAX;
    }
    int king_idx = Board_IS_WHITETURN(board) ? NCH_SQRIDX(Board_WHITE_KING(board)) : 
                                               NCH_SQRIDX(Board_BLACK_KING(board));

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