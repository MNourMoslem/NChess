#include "generate.h"
#include "generate_utils.h"
#include "bitboard.h"
#include <string.h>
#include <stdio.h>

NCH_STATIC_INLINE void
_generate_pieces_psudo_pawns(Board* board){
    uint64 op_map, sqr;
    uint64 pawns_map;
    int idx;
    
    uint64 allowed_map = ~Board_ALL_OCC(board);
    if (Board_IS_WHITETURN(board)){
        pawns_map = Board_WHITE_PAWNS(board);
        op_map = Board_BLACK_OCC(board) | board->en_passant_trg;

        LOOP_U64_T(pawns_map){
            sqr = NCH_SQR(idx);
            board->moves[idx] = ((NCH_CHKUNI(sqr, NCH_ROW2) && NCH_CHKFLG(allowed_map, (0x010100ULL << idx))) ?
                                 (0x010100ULL << idx) : (NCH_NXTSQR_UP(sqr) & allowed_map))
                                 | (bb_pawn_attacks(NCH_White, idx) & op_map);
        }

    }
    else{
        pawns_map = Board_BLACK_PAWNS(board);
        op_map = Board_WHITE_OCC(board) | board->en_passant_trg;

        LOOP_U64_T(pawns_map){
            sqr = NCH_SQR(idx);
            board->moves[idx] = ((NCH_CHKUNI(sqr, NCH_ROW7) && NCH_CHKFLG(allowed_map, 0x0080800000000000ULL >> (63 - idx))) ?
                                 (0x0080800000000000ULL >> (63 - idx)): NCH_NXTSQR_DOWN(sqr) & allowed_map)
                                 | (bb_pawn_attacks(NCH_Black, idx) & op_map);
        }
    }
}

NCH_STATIC_INLINE void
_generate_pieces_psudo_king(Board* board){
    Side side = Board_GET_SIDE(board);
    uint64 king_sqr, op_king_sqr, allowed_squares;
    if (side == NCH_White){
        king_sqr = Board_WHITE_KING(board);
        op_king_sqr = Board_BLACK_KING(board);
        allowed_squares = ~Board_WHITE_OCC(board);
    }
    else{
        king_sqr = Board_BLACK_KING(board);
        op_king_sqr = Board_WHITE_KING(board);
        allowed_squares = ~Board_BLACK_OCC(board);
    }

    int king_idx = NCH_SQRIDX(king_sqr);
    if (king_idx >= 64)
        return;
        
    uint64 moves = bb_king_attacks(king_idx) & allowed_squares & ~bb_king_attacks(NCH_SQRIDX(op_king_sqr));
    int idx;
    LOOP_U64_T(moves){
        if (get_checkmap(board, side, idx, Board_ALL_OCC(board))){
            NCH_RMVFLG(moves, NCH_SQR(idx));
        }
    }

    board->moves[king_idx] = moves;
}

NCH_STATIC_INLINE void
_generate_pieces_psudo_others(Board* board){
    uint64 knight_map, bishop_map, rook_map, queen_map;
    uint64 allowed_squares;
    if (Board_IS_WHITETURN(board)){
        knight_map = Board_WHITE_KNIGHTS(board);
        bishop_map = Board_WHITE_BISHOPS(board);
        rook_map = Board_WHITE_ROOKS(board);
        queen_map = Board_WHITE_QUEENS(board);
    
        allowed_squares = ~Board_WHITE_OCC(board);
    }
    else{
        knight_map = Board_BLACK_KNIGHTS(board);
        bishop_map = Board_BLACK_BISHOPS(board);
        rook_map = Board_BLACK_ROOKS(board);
        queen_map = Board_BLACK_QUEENS(board);
    
        allowed_squares = ~Board_BLACK_OCC(board);
    }
    
    uint64 all_occ = Board_ALL_OCC(board);
    int idx;

    LOOP_U64_T(knight_map){
        board->moves[idx] = bb_knight_attacks(idx) & allowed_squares;
    }

    LOOP_U64_T(rook_map){
        board->moves[idx] = bb_rook_attacks(idx, all_occ) & allowed_squares;
    }

    LOOP_U64_T(bishop_map){
        board->moves[idx] = bb_bishop_attacks(idx, all_occ) & allowed_squares;
    }

    LOOP_U64_T(queen_map){
        board->moves[idx] = bb_queen_attacks(idx, all_occ) & allowed_squares;
    }
}

NCH_STATIC_INLINE void 
generate_castle_moves(Board* board){
    if (Board_IS_CHECK(board)){
        return;
    }

    if (Board_IS_WHITETURN(board)){
        if (Board_IS_CASTLE_WK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F1) | NCH_SQR(NCH_G1)))
            && !get_checkmap(board, NCH_White, NCH_G1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_F1, Board_ALL_OCC(board))){
            
            board->moves[NCH_E1] |= NCH_SQR(NCH_G1);
        }

        if (Board_IS_CASTLE_WQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D1) | NCH_SQR(NCH_C1) | NCH_SQR(NCH_B1)))
            && !get_checkmap(board, NCH_White, NCH_D1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_C1, Board_ALL_OCC(board))){
            
            board->moves[NCH_E1] |= NCH_SQR(NCH_C1);
        }
    }
    else{
        if (Board_IS_CASTLE_BK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), 0x0600000000000000)
            && !get_checkmap(board, NCH_Black, NCH_G8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_F8, Board_ALL_OCC(board))){
            
            board->moves[NCH_E8] |= NCH_SQR(NCH_G8);
        }

        if (Board_IS_CASTLE_BQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), 0x7000000000000000)
            && !get_checkmap(board, NCH_Black, NCH_D8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_C8, Board_ALL_OCC(board))){
            
            board->moves[NCH_E8] |= NCH_SQR(NCH_C8);
        }
    }
}

NCH_STATIC_INLINE void
execlude_pinned_pieces_unlegal_moves(Board *board){
    uint64 king_sqr = Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board);
    int king_idx = NCH_SQRIDX(king_sqr);

    uint64 all_occ = Board_ALL_OCC(board);
    uint64 queen_attack = bb_queen_attacks(king_idx, all_occ);

    NCH_RMVFLG(all_occ, queen_attack);
    if (board->en_passant_idx && NCH_SAME_ROW(king_idx, board->en_passant_idx) 
        && !NCH_CHKFLG(all_occ, board->en_passant_map)){

        NCH_RMVFLG(all_occ, board->en_passant_map);
    }
    
    queen_attack = bb_queen_attacks(king_idx, all_occ);

    uint64 attackers_map = get_checkmap(board, Board_GET_SIDE(board), king_idx, all_occ);
    if (!attackers_map){
        return;
    }

    uint64 self_map = Board_IS_WHITETURN(board) ? Board_WHITE_OCC(board) : Board_BLACK_OCC(board);
    uint64 between;
    int idx, piece_idx;
    LOOP_U64_T(attackers_map){
        between = bb_between(king_idx, idx);
        piece_idx = NCH_SQRIDX(self_map & between);
        if (piece_idx < 64)
            board->moves[piece_idx] &= between;
    }
}

void
generate_moves(Board* board){
    memset(board->moves, 0ULL, sizeof(board->moves));

    uint64 allowed_squares = get_allowed_squares(board);
    if (allowed_squares){
        _generate_pieces_psudo_pawns(board);
        _generate_pieces_psudo_others(board);
        execlude_pinned_pieces_unlegal_moves(board);

        if (allowed_squares != NCH_UINT64_MAX){
            for (int i = 0; i < NCH_SQUARE_NB; i++){
                if (board->moves[i]){
                    board->moves[i] &= allowed_squares;
                }
            }
        }
    }
    _generate_pieces_psudo_king(board);
    generate_castle_moves(board);
}