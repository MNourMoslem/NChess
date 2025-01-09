#include "generate.h"
#include "generate_utils.h"
#include "bitboard.h"
#include <string.h>
#include <stdio.h>

NCH_STATIC_FINLINE void
generate_pieces_psudo_pawns(Board* board, uint64 allowed_squares){
    uint64 op_map;
    int idx;
    if (Board_IS_WHITETURN(board)){
        op_map = (Board_BLACK_OCC(board) | board->en_passant_trg);
        LOOP_U64_T(Board_WHITE_PAWNS(board)){
            board->moves[idx] = (
                                    ((NCH_CHKFLG(~Board_ALL_OCC(board) & (NCH_ROW3 | NCH_ROW4), (0x010100ULL << idx))) ?
                                    (0x010100ULL << idx) : (NCH_NXTSQR_UP(NCH_SQR(idx)) & ~Board_ALL_OCC(board)))
                                 
                                    | (
                                        bb_pawn_attacks(NCH_White, idx) 
                                        & op_map
                                      )
                                ) & allowed_squares;
        }

    }
    else{
        op_map = (Board_WHITE_OCC(board) | board->en_passant_trg);
        LOOP_U64_T(Board_BLACK_PAWNS(board)){
            board->moves[idx] = (
                                    ((NCH_CHKFLG(~Board_ALL_OCC(board) & (NCH_ROW6 | NCH_ROW5), 0x0080800000000000ULL >> (63 - idx))) ?
                                    (0x0080800000000000ULL >> (63 - idx)): NCH_NXTSQR_DOWN(NCH_SQR(idx)) & ~Board_ALL_OCC(board))

                                    | (
                                        bb_pawn_attacks(NCH_Black, idx) 
                                        & op_map
                                      )
                                ) & allowed_squares;
        }
    }
}

NCH_STATIC_FINLINE void
generate_pieces_psudo_king(Board* board){
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

NCH_STATIC_FINLINE void
generate_pieces_psudo_others(Board* board, uint64 allowed_squares){
    Side side = Board_GET_SIDE(board);
    allowed_squares &= ~board->occupancy[side];
    int idx;

    LOOP_U64_T(board->bitboards[side][NCH_Knight]){
        board->moves[idx] = bb_knight_attacks(idx) & allowed_squares;
    }

    LOOP_U64_T(board->bitboards[side][NCH_Rook]){
        board->moves[idx] = bb_rook_attacks(idx, Board_ALL_OCC(board)) & allowed_squares;
    }

    LOOP_U64_T(board->bitboards[side][NCH_Bishop]){
        board->moves[idx] = bb_bishop_attacks(idx, Board_ALL_OCC(board)) & allowed_squares;
    }

    LOOP_U64_T(board->bitboards[side][NCH_Queen]){
        board->moves[idx] = bb_queen_attacks(idx, Board_ALL_OCC(board)) & allowed_squares;
    }
}

NCH_STATIC_FINLINE void 
generate_castle_moves(Board* board){
    if (Board_IS_CHECK(board) || !board->castles){
        return;
    }

    if (Board_IS_WHITETURN(board)){
        if (Board_IS_CASTLE_WK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F1) | NCH_SQR(NCH_G1)))
            && !get_checkmap(board, NCH_White, NCH_G1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_F1, Board_ALL_OCC(board))){
            
            board->castle_moves |= Board_CASTLE_WK;
        }

        if (Board_IS_CASTLE_WQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D1) | NCH_SQR(NCH_C1) | NCH_SQR(NCH_B1)))
            && !get_checkmap(board, NCH_White, NCH_D1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_C1, Board_ALL_OCC(board))){
            
            board->castle_moves |= Board_CASTLE_WQ;
        }
    }
    else{
        if (Board_IS_CASTLE_BK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F8) | NCH_SQR(NCH_G8)))
            && !get_checkmap(board, NCH_Black, NCH_G8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_F8, Board_ALL_OCC(board))){
            
            board->castle_moves |= Board_CASTLE_BK;
        }

        if (Board_IS_CASTLE_BQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D8) | NCH_SQR(NCH_C8) | NCH_SQR(NCH_B8)))
            && !get_checkmap(board, NCH_Black, NCH_D8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_C8, Board_ALL_OCC(board))){
            
            board->castle_moves |= Board_CASTLE_BQ;
        }
    }
}

NCH_STATIC_FINLINE void
execlude_pinned_pieces_unlegal_moves(Board *board){
    Side side = Board_GET_SIDE(board);
    uint64 king_sqr = side == NCH_White ? Board_WHITE_KING(board) : Board_BLACK_KING(board);
    int king_idx = NCH_SQRIDX(king_sqr);

    uint64 all_occ = Board_ALL_OCC(board) &~ king_sqr;
    uint64 self_occ = side == NCH_White ? Board_WHITE_OCC(board) : Board_BLACK_OCC(board);
    uint64 queen_attack = bb_queen_attacks(king_idx, all_occ);

    int special_case = 0;
    uint64 special_map;
    if (board->en_passant_idx && NCH_SAME_ROW(king_idx, board->en_passant_idx)
        && NCH_CHKUNI(queen_attack, board->en_passant_map) && count_bits(board->en_passant_map) == 2){

        queen_attack = (queen_attack & self_occ) | board->en_passant_map;
        special_case = NCH_SQRIDX(board->en_passant_map &~ NCH_SQR(board->en_passant_idx));
        special_map = board->moves[special_case];
    }
    else{
        queen_attack &= self_occ;
    }
    NCH_RMVFLG(all_occ, queen_attack);

    uint64 attackmap = get_checkmap(board, side, king_idx, all_occ);
    if (!attackmap){
        return;
    }

    int idx, trg_idx;
    uint64 between, btable = 0ull;
    LOOP_U64_T(attackmap){
        between = bb_between(king_idx, idx);
        btable |= between;
        trg_idx = NCH_SQRIDX(between & self_occ);
        if (trg_idx < 64)
            board->moves[trg_idx] &= between;
    }

    if (special_case && NCH_CHKFLG(btable, NCH_SQR(special_case)))
        board->moves[special_case] = special_map &~ board->en_passant_trg;
    
}

void
generate_moves(Board* board){
    memset(board->moves, 0ULL, sizeof(board->moves));
    board->castle_moves = 0;

    uint64 allowed_squares = get_allowed_squares(board);
    if (allowed_squares){
        generate_pieces_psudo_pawns(board, allowed_squares);
        generate_pieces_psudo_others(board, allowed_squares);

        if (allowed_squares != NCH_UINT64_MAX && board->en_passant_idx 
            && NCH_CHKFLG(allowed_squares, NCH_SQR(board->en_passant_idx))){
                int idx;
                LOOP_U64_T(board->en_passant_map & (~NCH_SQR(board->en_passant_idx))){
                    board->moves[idx] |= board->en_passant_trg;
                }
            }

        execlude_pinned_pieces_unlegal_moves(board);
        generate_castle_moves(board);
    }

    generate_pieces_psudo_king(board);
}