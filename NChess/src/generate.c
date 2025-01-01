#include "generate.h"
#include "generate_utils.h"
#include "bitboard.h"
#include <string.h>
#include <stdio.h>

NCH_STATIC_INLINE void
_generate_pieces_psudo_pawns(Board* board, uint64 allowed_pieces){
    Side side = Board_GET_SIDE(board);
    uint64 op_map, sqr;
    uint64 pawns_map = allowed_pieces;
    int idx;
    
    uint64 allowed_map = ~Board_ALL_OCC(board);
    if (side == NCH_White){
        pawns_map &= Board_WHITE_PAWNS(board);
        op_map = Board_BLACK_OCC(board) | board->en_passant_trg;

        LOOP_U64_T(pawns_map){
            sqr = NCH_SQR(idx);
            board->moves[idx] = ((NCH_CHKUNI(sqr, NCH_ROW2) && NCH_CHKFLG(allowed_map, (0x010100ULL << idx))) ?
                                 (0x010100ULL << idx) : (NCH_NXTSQR_UP(sqr) & allowed_map))
                                 | (bb_pawn_attacks(NCH_White, idx) & op_map);
        }

    }
    else{
        pawns_map &= Board_BLACK_PAWNS(board);
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
    uint64 king_sqr = side == NCH_White ? Board_WHITE_KING(board) : Board_BLACK_KING(board);
    uint64 allowed_squares = side == NCH_White ? ~Board_WHITE_OCC(board) : ~Board_BLACK_OCC(board);
    int king_idx = NCH_SQRIDX(king_sqr);
    uint64 moves = bb_king_attacks(king_idx) & allowed_squares;
    int idx;
    LOOP_U64_T(moves){
        if (get_checkmap(board, side, idx, Board_ALL_OCC(board))){
            NCH_RMVFLG(moves, NCH_SQR(idx));
        }
    }

    board->moves[king_idx] = moves;
}

NCH_STATIC_INLINE void
_generate_pieces_psudo_others(Board* board, uint64 allowed_pieces){
    uint64 knight_map, bishop_map, rook_map, queen_map;
    uint64 allowed_squares;
    if (Board_IS_WHITETURN(board)){
        knight_map = Board_WHITE_KNIGHTS(board) & allowed_pieces;
        bishop_map = Board_WHITE_BISHOPS(board) & allowed_pieces;
        rook_map = Board_WHITE_ROOKS(board) & allowed_pieces;
        queen_map = Board_WHITE_QUEENS(board) & allowed_pieces;
    
        allowed_squares = ~Board_WHITE_OCC(board);
    }
    else{
        knight_map = Board_BLACK_KNIGHTS(board) & allowed_pieces;
        bishop_map = Board_BLACK_BISHOPS(board) & allowed_pieces;
        rook_map = Board_BLACK_ROOKS(board) & allowed_pieces;
        queen_map = Board_BLACK_QUEENS(board) & allowed_pieces;
    
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

void 
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

void
generate_moves(Board* board){
    memset(board->moves, 0ULL, sizeof(board->moves));

    uint64 allowed_square = get_allowed_squares(board);
    if (allowed_square){
        uint64 allowed_pieces = get_allowed_pieces(board);
        _generate_pieces_psudo_pawns(board, allowed_pieces);
        _generate_pieces_psudo_others(board, allowed_pieces);

        if (allowed_pieces != NCH_UINT64_MAX){
            for (int i = 0; i < NCH_SQUARE_NB; i++){
                if (board->moves[i]){
                    board->moves[i] &= allowed_square;
                }
            }
        }
    }
    _generate_pieces_psudo_king(board);
    generate_castle_moves(board);
}