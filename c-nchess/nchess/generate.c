#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "utils.h"
#include "bitboard.h"
#include "generate.h"

#include <stdio.h>
#include <string.h>

NCH_STATIC_INLINE uint64
get_allowed_squares(const Board* board){
    if (!Board_IS_CHECK(board))
        return NCH_UINT64_MAX;

    Side side = Board_GET_SIDE(board);
    int king_idx = NCH_SQRIDX(Board_BB(board, side, NCH_King));

    uint64 attackers_map = get_checkmap(board, side, king_idx, Board_ALL_OCC(board));
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
    
    int enp_idx = Board_ENP_IDX(board);
    uint64 enp_map = Board_ENP_MAP(board);

    if (enp_idx && NCH_SAME_ROW(king_idx, enp_idx)
        && (around & enp_map) && has_two_bits(enp_map)){
            
            special = 1;
            all_occ &= ~enp_map;

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

    if (special && (pinned_pieces & enp_map)){
        pinned_allowed_squares--;
        while (!(*pinned_allowed_squares & enp_map))
        {
            pinned_allowed_squares--;
        }
        *pinned_allowed_squares = ~(Board_ENP_TRG(board) | enp_map);
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
generate_knight_moves(int idx, NCH_UNUSED(uint64 occ), uint64 allowed_squares, Move* moves){
    uint64 bb = bb_knight_attacks(idx) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_pawn_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    Side ply_side = Board_GET_SIDE(board);
    Side op_side = Board_GET_OP_SIDE(board);

    int could2sqr = ply_side == NCH_White ? NCH_GET_ROWIDX(idx) == 1
                                          : NCH_GET_ROWIDX(idx) == 6;

    int couldpromote = ply_side == NCH_White ? NCH_GET_ROWIDX(idx) == 6
                                             : NCH_GET_ROWIDX(idx) == 1;


    uint64 op_occ = Board_OCC(board, op_side);
    uint64 all_occ = Board_ALL_OCC(board);

    uint64 bb = bb_pawn_attacks(ply_side, idx) & (op_occ | Board_ENP_TRG(board));

    if (could2sqr){
        int trg_idx = ply_side == NCH_White ? idx + 16
                                            : idx - 16;
                                            
        uint64 twoSqrPath = bb_between(idx, trg_idx);

        bb |= (all_occ & twoSqrPath) ? twoSqrPath &~ (all_occ | NCH_ROW4 | NCH_ROW5)
                                     : twoSqrPath;

    }
    else{
        bb |= ~all_occ & (ply_side == NCH_White ? NCH_NXTSQR_UP(NCH_SQR(idx))
                                                : NCH_NXTSQR_DOWN(NCH_SQR(idx)));
    }
    
    if (allowed_squares & Board_ENP_MAP(board)){
        allowed_squares |= Board_ENP_TRG(board);
    }

    bb &= allowed_squares;

    if (!bb)
        return moves;

    int is_enpassant = (bb & Board_ENP_TRG(board)) != 0ULL;

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
        target = NCH_SQRIDX(Board_ENP_TRG(board));
        *moves++ = Move_New(idx, target, 0, 0, 1, 0);
        bb &= ~Board_ENP_TRG(board);
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
            return generate_knight_moves(idx, 0ULL, allowed_squares, moves);
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
    if (!Board_CASTLES(board) || Board_IS_CHECK(board)){
        return moves;
    }

    if (Board_IS_WHITETURN(board)){
        if (Board_IS_CASTLE_WK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F1) | NCH_SQR(NCH_G1)))
            && !get_checkmap(board, NCH_White, NCH_G1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_F1, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(NCH_E1, NCH_G1, 0, Board_CASTLE_WK, 0, 0);
        }

        if (Board_IS_CASTLE_WQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D1) | NCH_SQR(NCH_C1) | NCH_SQR(NCH_B1)))
            && !get_checkmap(board, NCH_White, NCH_D1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_C1, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(NCH_E1, NCH_C1, 0, Board_CASTLE_WQ, 0, 0);
        }
    }
    else{
        if (Board_IS_CASTLE_BK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F8) | NCH_SQR(NCH_G8)))
            && !get_checkmap(board, NCH_Black, NCH_G8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_F8, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(NCH_E8, NCH_G8, 0, Board_CASTLE_BK, 0, 0);
        }

        if (Board_IS_CASTLE_BQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D8) | NCH_SQR(NCH_C8) | NCH_SQR(NCH_B8)))
            && !get_checkmap(board, NCH_Black, NCH_D8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_C8, Board_ALL_OCC(board))){
            
            *moves++ = Move_New(NCH_E8, NCH_C8, 0, Board_CASTLE_BQ, 0, 0);
        }
    }

    return moves;
}

NCH_STATIC void*
generate_non_pinned_moves(Board* board, uint64 non_pinned_occ, uint64 allowed_squares, Move* moves){
    Side side = Board_GET_SIDE(board);
    uint64 all_occ = Board_ALL_OCC(board);
    int idx;
    while (non_pinned_occ)
    {
        idx = NCH_SQRIDX(non_pinned_occ);
        moves = generate_any_move(board, side, idx, all_occ, allowed_squares, moves);
        non_pinned_occ &= non_pinned_occ - 1;
    }
    
    return moves;
}

NCH_STATIC void*
generate_pinned_moves(Board* board, uint64 pinned_pieces, uint64 allowed_sqaures, uint64* pinned_allowed_squares, Move* moves){
    Side side = Board_GET_SIDE(board);
    uint64 all_occ = Board_ALL_OCC(board);
    int idx;
    while (pinned_pieces)
    {
        idx = NCH_SQRIDX(pinned_pieces);
        moves = generate_any_move(board, side, idx, all_occ, *pinned_allowed_squares++ & allowed_sqaures, moves);
        pinned_pieces &= pinned_pieces - 1;
    }

    return moves;
}

int
Board_GenerateLegalMoves(Board* board, Move* moves){
    uint64 pinned_allowed_square[8];
    Move* mh = moves;

    Side side = Board_GET_SIDE(board);

    uint64 self_occ = board->occupancy[side];
    
    uint64 allowed_sqaures = get_allowed_squares(board) &~ self_occ;
    uint64 pinned_pieces = get_pinned_pieces(board, pinned_allowed_square);
    uint64 not_pinned_pieces = self_occ &~ pinned_pieces;

    if (allowed_sqaures){
        moves = generate_non_pinned_moves(board, not_pinned_pieces, allowed_sqaures, moves);

        if (pinned_pieces)
            moves = generate_pinned_moves(board, pinned_pieces, allowed_sqaures, pinned_allowed_square, moves);

        moves = generate_castle_moves(board, moves);
    }

    moves = generate_king_moves(board, moves);

    return moves - mh;
}

Move*
Board_GeneratePseudoMovesMapOf(Board* board, Move* moves, int idx){
    Side side = Board_GET_SIDE(board);
    Piece p = Board_PIECE(board, side, idx);

    if (p == NCH_King){
        moves = generate_castle_moves(board, moves);
        moves = generate_king_moves(board, moves);
    }
    else{
        moves = generate_any_move(board, side, idx, Board_ALL_OCC(board), NCH_UINT64_MAX, moves);
    }
    return moves;
}