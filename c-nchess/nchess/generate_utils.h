#ifndef NCHESS_SRC_GENERATE_UTILS_H
#define NCHESS_SRC_GENERATE_UTILS_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"

#include "generate_utils.h"
#include "bitboard.h"

#include <stdio.h>

NCH_STATIC_INLINE uint64
get_checkmap(const Board* board, Side side, int king_idx, uint64 all_occ){
    uint64 occupancy;
    if (side == NCH_White){
        occupancy = all_occ & ~Board_WHITE_KING(board);

        return    (bb_rook_attacks(king_idx, occupancy)   & (Board_BLACK_ROOKS(board)   | Board_BLACK_QUEENS(board))) 
                | (bb_bishop_attacks(king_idx, occupancy) & (Board_BLACK_BISHOPS(board) | Board_BLACK_QUEENS(board)))
                | (bb_knight_attacks(king_idx)            & Board_BLACK_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_White, king_idx)   & Board_BLACK_PAWNS(board)  );
    }
    else{
        occupancy = all_occ & ~Board_BLACK_KING(board);

        return    (bb_rook_attacks(king_idx, occupancy)   & (Board_WHITE_ROOKS(board)   | Board_WHITE_QUEENS(board))) 
                | (bb_bishop_attacks(king_idx, occupancy) & (Board_WHITE_BISHOPS(board) | Board_WHITE_QUEENS(board)))
                | (bb_knight_attacks(king_idx)            & Board_WHITE_KNIGHTS(board))
                | (bb_pawn_attacks(NCH_Black, king_idx)   & Board_WHITE_PAWNS(board)  );
    }
}

NCH_STATIC_INLINE uint64
get_allowed_squares(const Board* board){
    if (!Board_IS_CHECK(board))
        return NCH_UINT64_MAX;

    int king_idx = Board_IS_WHITETURN(board) ? NCH_SQRIDX(Board_WHITE_KING(board)) : 
                                               NCH_SQRIDX(Board_BLACK_KING(board)) ;

    uint64 attackers_map = get_checkmap(board, Board_GET_SIDE(board), king_idx, Board_ALL_OCC(board));
    if (!attackers_map)
        return NCH_UINT64_MAX;

    if (more_then_one(attackers_map))
        return 0ULL;
    return bb_between(king_idx, NCH_SQRIDX(attackers_map));
}

NCH_STATIC_INLINE uint64
get_pinned_pieces(const Board* board, uint64* pinned_allowed_squares){
    int king_idx = Board_IS_WHITETURN(board) ? NCH_SQRIDX(Board_WHITE_KING(board)) : 
                                               NCH_SQRIDX(Board_BLACK_KING(board)) ;

    uint64 self_occ = Board_IS_WHITETURN(board) ? Board_WHITE_OCC(board)
                                                : Board_BLACK_OCC(board);
    uint64 all_occ = Board_ALL_OCC(board);

    uint64 around = bb_queen_attacks(king_idx, all_occ);
    all_occ &= ~(around & self_occ);

    int special = 0;
    if (board->en_passant_idx && NCH_SAME_ROW(king_idx, board->en_passant_idx)
        && (around & board->en_passant_map) && has_two_bits(board->en_passant_map)){
            
            special = 1;
            all_occ &= ~board->en_passant_map;

        }


    around = get_checkmap(board, Board_GET_SIDE(board), king_idx, all_occ);
    if (!around)
        return 0ULL;

    uint64 pinned_pieces;
    uint64 between;
    int idx;
    if (!more_then_one(around)){
        idx = NCH_SQRIDX(around);
        between = bb_between(king_idx, idx);
        pinned_pieces = between & self_occ;
        *pinned_allowed_squares++ = between;
    }
    else{
        uint64 map[NCH_DIR_NB];

        pinned_pieces = 0ULL;
        int pinned_idx, dir;
        uint64 pinned_map;
        while (around)
        {
            idx = NCH_SQRIDX(around);
            between = bb_between(king_idx, idx);

            pinned_map = between & self_occ;
            if (pinned_map){
                pinned_idx = NCH_SQRIDX(pinned_map);
                
                dir = NCH_GET_DIRACTION(king_idx, pinned_idx);

                pinned_pieces |= pinned_map;
                map[dir] = between;
            }
            around &= around - 1;
        }

        uint64 cpy = pinned_pieces;
        while (cpy)
        {
            idx = NCH_SQRIDX(cpy);
            dir = NCH_GET_DIRACTION(king_idx, idx);

            *pinned_allowed_squares++ = map[dir];
            cpy &= cpy - 1;
        }
    }

    if (special && (pinned_pieces & board->en_passant_map)){
        pinned_allowed_squares--;
        while (!(*pinned_allowed_squares & board->en_passant_map))
        {
            pinned_allowed_squares--;
        }
        *pinned_allowed_squares = ~(board->en_passant_trg | board->en_passant_map);
    }

    return pinned_pieces;
}

NCH_STATIC_INLINE void*
bb_to_moves(uint64 bb, int idx, Move* moves){
    int target;
    while (bb)
    {
        target = NCH_SQRIDX(bb);
        *moves++ = Move_New(idx, target, 0, 0, 0, 0);
        bb &= bb - 1;
    }
    return moves;
}

NCH_STATIC_INLINE void*
generate_queen_moves(int idx, uint64 occ, uint64 allowed_squares, Move* moves){
    uint64 bb = bb_queen_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_rook_moves(int idx, uint64 occ, uint64 allowed_squares, Move* moves){
    uint64 bb = bb_rook_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_bishop_moves(int idx, uint64 occ, uint64 allowed_squares, Move* moves){
    uint64 bb = bb_bishop_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_knight_moves(int idx, uint64 allowed_squares, Move* moves){
    uint64 bb = bb_knight_attacks(idx) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_pawn_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    int could2sqr = Board_IS_WHITETURN(board) ? NCH_GET_ROWIDX(idx) == 1
                                              : NCH_GET_ROWIDX(idx) == 6;

    int couldpromote = Board_IS_WHITETURN(board) ? NCH_GET_ROWIDX(idx) == 6
                                                 : NCH_GET_ROWIDX(idx) == 1;


    uint64 op_occ = board->occupancy[Board_GET_OP_SIDE(board)];
    uint64 all_occ = Board_ALL_OCC(board);

    uint64 bb = bb_pawn_attacks(Board_GET_SIDE(board), idx) & (op_occ | board->en_passant_trg);

    if (Board_IS_WHITETURN(board)){
        if (could2sqr && NCH_CHKFLG(~all_occ & (NCH_ROW3 | NCH_ROW4), 0x10100ULL << idx))
            bb |= 0x10100ULL << idx;
        else
            bb |= NCH_NXTSQR_UP(NCH_SQR(idx)) &~ all_occ;
    }
    else{
        if (could2sqr && NCH_CHKFLG(~all_occ & (NCH_ROW5 | NCH_ROW6), 0x0080800000000000ULL >> (63 - idx)))
            bb |= 0x0080800000000000ULL >> (63 - idx);
        else
            bb |= NCH_NXTSQR_DOWN(NCH_SQR(idx)) &~ all_occ;
    }

    if (board->en_passant_idx && allowed_squares != NCH_UINT64_MAX && allowed_squares & board->en_passant_map){
        allowed_squares |= board->en_passant_trg;    
    }

    bb &= allowed_squares;

    if (!bb)
        return moves;

    int is_enpassant = (bb & board->en_passant_trg) != 0ULL;

    int target;
    
    if (couldpromote){
        while (bb)
        {
            target = NCH_SQRIDX(bb);

            *moves++ = Move_New(idx, target, NCH_Queen, 0, 0, 1);
            *moves++ = Move_New(idx, target, NCH_Rook, 0, 0, 1);
            *moves++ = Move_New(idx, target, NCH_Bishop, 0, 0, 1);
            *moves++ = Move_New(idx, target, NCH_Knight, 0, 0, 1);
        
            bb &= bb - 1;
        }
        return moves;
    }

    if (is_enpassant){
        target = NCH_SQRIDX(board->en_passant_trg);
        *moves++ = Move_New(idx, target, 0, 0, 1, 0);
        bb &= ~board->en_passant_trg;
    }

    while (bb)
    {
        target = NCH_SQRIDX(bb);
        *moves++ = Move_New(idx, target, 0, 0, 0, 0);
        bb &= bb - 1;
    }

    return moves;
}

NCH_STATIC_INLINE void*
generate_any_move(Board* board, Side side, int idx, uint64 occ, uint64 allowed_squares, Move* moves){
    switch (board->piecetables[side][idx])
        {
        case NCH_Queen:
            return generate_queen_moves(idx, occ, allowed_squares, moves);
            break;

        case NCH_Rook:
            return generate_rook_moves(idx, occ, allowed_squares, moves);
            break;

        case NCH_Bishop:
            return generate_bishop_moves(idx, occ, allowed_squares, moves);
            break;
        
        case NCH_Knight:
            return generate_knight_moves(idx, allowed_squares, moves);
            break;
        
        case NCH_Pawn:
            return generate_pawn_moves(board, idx, allowed_squares, moves);

        default:
            break;
        }

    return moves;
}

NCH_STATIC_INLINE void*
generate_king_moves(Board* board, Move* moves){
    Side side = Board_GET_SIDE(board);

    int king_idx = NCH_SQRIDX(board->bitboards[side][NCH_King]);
    if (king_idx >= 64)
        return moves;
        
    uint64 bb =  bb_king_attacks(king_idx)
              &  ~board->occupancy[side]
              &  ~bb_king_attacks(NCH_SQRIDX(board->bitboards[Board_GET_OP_SIDE(board)][NCH_King]));
    int target;
    while (bb)
    {
        target = NCH_SQRIDX(bb);
        if (!get_checkmap(board, side, target, Board_ALL_OCC(board)))
            *moves++ = Move_New(king_idx, target, 0, 0, 0, 0);
        bb &= bb - 1;
    }

    return moves;
}

NCH_STATIC_INLINE void*
generate_castle_moves(Board* board, Move* moves){
    if (!board->castles || Board_IS_CHECK(board)){
        return moves;
    }

    if (Board_IS_WHITETURN(board)){
        if (Board_IS_CASTLE_WK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F1) | NCH_SQR(NCH_G1)))
            && !get_checkmap(board, NCH_White, NCH_G1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_F1, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(0, 0, 0, Board_CASTLE_WK, 0, 0);
        }

        if (Board_IS_CASTLE_WQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D1) | NCH_SQR(NCH_C1) | NCH_SQR(NCH_B1)))
            && !get_checkmap(board, NCH_White, NCH_D1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_C1, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(0, 0, 0, Board_CASTLE_WQ, 0, 0);
        }
    }
    else{
        if (Board_IS_CASTLE_BK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F8) | NCH_SQR(NCH_G8)))
            && !get_checkmap(board, NCH_Black, NCH_G8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_F8, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(0, 0, 0, Board_CASTLE_BK, 0, 0);
        }

        if (Board_IS_CASTLE_BQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D8) | NCH_SQR(NCH_C8) | NCH_SQR(NCH_B8)))
            && !get_checkmap(board, NCH_Black, NCH_D8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_C8, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(0, 0, 0, Board_CASTLE_BQ, 0, 0);
        }
    }

    return moves;
}

#endif