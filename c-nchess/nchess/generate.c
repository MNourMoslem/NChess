/*
    generate.c

    This file contains all functions related to move generation.
    from generating pseudo moves to generating legal moves.
*/


#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "utils.h"
#include "bitboard.h"
#include "generate.h"

#include <string.h>


// This function is used to return the squares that pieces could move to.
// If the king is not under check it would return all the squares on the board.
// If the king is under check it by one piece it would return the squares between
// the king and the attacker. If the king is under check by more than one piece
// it would return 0 which means that no piece could move except the king.
NCH_STATIC_INLINE uint64
get_allowed_squares(const Board* board){
    if (!Board_IS_CHECK(board))
        return NCH_UINT64_MAX;

    Side side = Board_SIDE(board);
    int king_idx = NCH_SQRIDX(Board_BB(board, side, NCH_King));

    uint64 attackers_map = get_checkmap(board, side, king_idx, Board_ALL_OCC(board));
    if (!attackers_map)
        return NCH_UINT64_MAX;

    if (more_then_one(attackers_map))
        return 0ULL;
    return bb_between(king_idx, NCH_SQRIDX(attackers_map));
}

// This function is used to return the squares that the pinned pieces could move to.
// If there are no pinned pieces it would return 0.
// If there are pinned pieces it would return the bitboard of the pinned pieces.
// The pinned_allowed_squares is an array of size NCH_DIR_NB (number of diractions)
// where each item in the array respresent the squares that the pinned piece could move to.
// with respect to its diractions to the king.
NCH_STATIC_INLINE uint64
get_pinned_pieces(const Board* board, uint64* pinned_allowed_squares){
    Side       side = Board_SIDE(board);
    int    king_idx = NCH_SQRIDX(Board_BB(board, side, NCH_King));
    uint64 self_occ = Board_OCC(board, side);
    uint64  all_occ = Board_ALL_OCC(board);
    int     enp_idx = Board_ENP_IDX(board);
    uint64  enp_map = Board_ENP_MAP(board);
    
    uint64 queen_like = bb_queen_attacks(king_idx, all_occ);
    uint64 around = (queen_like & self_occ); // currently playing player's pieces only
    all_occ &= ~around;
    
    // This is a special case where the king is in the same row of the
    // pawn threatening en passant. In this case if the pawn take by en passant
    // two pieces would be removed from the row. This is very tricky because
    // this is the only way to empty two squares with one move in chess.
    // when this special case hits we need to add the attacking pawn to the 
    // around bitboard and remove it from the all_occ bitboard.
    // we check if en passant is possible and the king is in the same row.
    // then we see if the queen_like scan is hitting one of the two pawns of the
    // en passant. cause they might be on the same row but they could be seprated
    // by another piece. and lastly we check if the enpassant map has two squares
    // only because if there was two pawns threatening enpassant and one taget pawn.
    // (in this case enp_map bit cound would be 3) the en passant move will not
    // discover the king on a check cause it only empties two squares and the enp_map
    // has 3.
    int special = 0;
    if (enp_idx && NCH_SAME_ROW(king_idx, enp_idx)
        && (queen_like & enp_map) && has_two_bits(enp_map))
        {
            special = 1;
            all_occ &= ~enp_map;
            around |= enp_map & self_occ;
        }

    uint64 rq = side == NCH_White ? Board_BLACK_ROOKS(board) | Board_BLACK_QUEENS(board)
                                  : Board_WHITE_ROOKS(board) | Board_WHITE_QUEENS(board);
    uint64 bq = side == NCH_White ? Board_BLACK_BISHOPS(board) | Board_BLACK_QUEENS(board)
                                  : Board_WHITE_BISHOPS(board) | Board_WHITE_QUEENS(board);

    uint64 snipers = ((bb_rook_attacks(king_idx, all_occ) & rq)
                    | (bb_bishop_attacks(king_idx, all_occ) & bq))
                    &~ queen_like; // execulde pieces that are already attacking the king
    
    if (!snipers)
        return 0ULL;
    
    uint64 pinned_pieces = 0ULL;
    uint64 line, bet;
    int idx;
    LOOP_U64_T(around){
        line = bb_line(king_idx, NCH_GET_DIRACTION(king_idx, idx));
        line &= snipers;
        if (line){
            bet = bb_between(king_idx, NCH_SQRIDX(line));
            *pinned_allowed_squares++ = bet;
            pinned_pieces |= bet & self_occ;
        }
    }
    
    if (special && (pinned_pieces & enp_map)){
        pinned_allowed_squares--;
        while (!(*pinned_allowed_squares & enp_map))
        {
            pinned_allowed_squares--;
        }

        // we set the allowed squares to be anything but the en passant target
        // and the en passant map. the reason we inlcude the en passant map as
        // well is a tricky situation. if the king is under attack by a pawn 
        // that has moved 2 squares. the allowed squares will only include the
        // square of the pawn attacking the king althoug if we could take that
        // pawn with en passant we will not be able to do it because the en passant
        // target is not included in the allowed squares. so what we do in the
        // pawn move generation is we check if the allowed squares containes the
        // the en passant map and if so we say alloewd_squares |= en passant target.
        // this want be harmful if the en passant target is 0. but if will be if 
        // it is not and if the allowed squares containes all the board squares.
        // that is why we need to exclude the en passant map from the allowed squares.
        // because the pawn move generation automatically includes the en passant target
        // if it sees that the target pawn is included in the allowed squares.
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
        *moves++ = _Move_New(idx, target, 1, MoveType_Normal);
        bb &= bb - 1;
    }
    return moves;
}

NCH_STATIC_INLINE void*
generate_queen_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    uint64 occ = Board_ALL_OCC(board);
    uint64 bb = bb_queen_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_rook_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    uint64 occ = Board_ALL_OCC(board);
    uint64 bb = bb_rook_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_bishop_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    uint64 occ = Board_ALL_OCC(board);
    uint64 bb = bb_bishop_attacks(idx, occ) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_knight_moves(NCH_UNUSED(Board* board), int idx, uint64 allowed_squares, Move* moves){
    uint64 bb = bb_knight_attacks(idx) & allowed_squares;
    return bb_to_moves(bb, idx, moves);
}

NCH_STATIC_INLINE void*
generate_pawn_moves(Board* board, int idx, uint64 allowed_squares, Move* moves){
    Side ply_side = Board_SIDE(board);
    Side op_side = NCH_OP_SIDE(ply_side);

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
    
    // here is the tricky part we explained in get_pinned_pieces function.
    // if the targeted pawn is in the allowed squares we automatically include
    // the en passant target in the allowed squares.
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

            *moves++ = _Move_New(idx, target, NCH_Queen, MoveType_Promotion);
            *moves++ = _Move_New(idx, target, NCH_Rook, MoveType_Promotion);
            *moves++ = _Move_New(idx, target, NCH_Bishop, MoveType_Promotion);
            *moves++ = _Move_New(idx, target, NCH_Knight, MoveType_Promotion);
        
            bb &= bb - 1;
        }
        return moves;
    }

    if (is_enpassant){
        target = NCH_SQRIDX(Board_ENP_TRG(board));
        *moves++ = _Move_New(idx, target, 1, MoveType_EnPassant);
        bb &= ~Board_ENP_TRG(board);
    }

    while (bb)
    {
        target = NCH_SQRIDX(bb);
        *moves++ = _Move_New(idx, target, 1, MoveType_Normal);
        bb &= bb - 1;
    }

    return moves;
}

typedef void* (*MoveGenFunction) (Board* board, int idx, uint64 allowed_squares, Move* moves);

NCH_STATIC MoveGenFunction MoveGenFunctionTable[] = {
    generate_pawn_moves,
    generate_knight_moves,
    generate_bishop_moves,
    generate_rook_moves,
    generate_queen_moves,
};

// generate any move for a piece on the board except the king.
// that is why it is not a safe function and it is only used in the
// in this file.
NCH_STATIC_INLINE void*
generate_any_move(Board* board, Side side, int idx, uint64 allowed_squares, Move* moves){
    Piece p = Board_PIECE(board, side, idx);
    MoveGenFunction func = MoveGenFunctionTable[p];
    return func(board, idx, allowed_squares, moves);
}

NCH_STATIC_INLINE void*
generate_king_moves(Board* board, Move* moves){
    Side side = Board_SIDE(board);
    int king_idx = NCH_SQRIDX(Board_BB(board, side, NCH_King));

    // if there is no king on the board for some reason we don't want to crash.
    if (king_idx >= 64)
        return moves;
        
    uint64 bb =  bb_king_attacks(king_idx)
              &  ~Board_OCC(board, side)
              &  ~bb_king_attacks(NCH_SQRIDX(Board_BB(board, NCH_OP_SIDE(side), NCH_King)));
    int target;
    while (bb)
    {
        target = NCH_SQRIDX(bb);
        if (!get_checkmap(board, side, target, Board_ALL_OCC(board)))
            *moves++ = _Move_New(king_idx, target, 1, MoveType_Normal);
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
            
            *moves++ = _Move_New(NCH_E1, NCH_G1, 1, MoveType_Castle);
        }

        if (Board_IS_CASTLE_WQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D1) | NCH_SQR(NCH_C1) | NCH_SQR(NCH_B1)))
            && !get_checkmap(board, NCH_White, NCH_D1, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_White, NCH_C1, Board_ALL_OCC(board))){
            
            *moves++ = _Move_New(NCH_E1, NCH_C1, 1, MoveType_Castle);
        }
    }
    else{
        if (Board_IS_CASTLE_BK(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_F8) | NCH_SQR(NCH_G8)))
            && !get_checkmap(board, NCH_Black, NCH_G8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_F8, Board_ALL_OCC(board))){
            
            *moves++ = _Move_New(NCH_E8, NCH_G8, 1, MoveType_Castle);
        }

        if (Board_IS_CASTLE_BQ(board) && !NCH_CHKUNI(Board_ALL_OCC(board), (NCH_SQR(NCH_D8) | NCH_SQR(NCH_C8) | NCH_SQR(NCH_B8)))
            && !get_checkmap(board, NCH_Black, NCH_D8, Board_ALL_OCC(board)) 
            && !get_checkmap(board, NCH_Black, NCH_C8, Board_ALL_OCC(board))){
            
            *moves++ = _Move_New(NCH_E8, NCH_C8, 1, MoveType_Castle);
        }
    }

    return moves;
}

int
Board_GenerateLegalMoves(Board* board, Move* moves){
    uint64 pinned_allowed_square[8];
    Move* mh = moves;

    Side side = Board_SIDE(board);
    uint64 self_occ = Board_OCC(board, side);
    
    uint64 allowed_squares = get_allowed_squares(board) &~ self_occ;
    uint64 pinned_pieces = get_pinned_pieces(board, pinned_allowed_square);
    uint64 not_pinned_pieces = self_occ &~ (pinned_pieces | Board_BB(board, side, NCH_King));

    if (allowed_squares){
        int idx;
        while (not_pinned_pieces)
        {
            idx = NCH_SQRIDX(not_pinned_pieces);
            moves = generate_any_move(board, side, idx, allowed_squares, moves);
            not_pinned_pieces &= not_pinned_pieces - 1;
        }    

        int i = 0;
        while (pinned_pieces)
        {
            idx = NCH_SQRIDX(pinned_pieces);
            moves = generate_any_move(board, side, idx, pinned_allowed_square[i++] & allowed_squares, moves);
            pinned_pieces &= pinned_pieces - 1;
        }

        moves = generate_castle_moves(board, moves);
    }

    moves = generate_king_moves(board, moves);

    return moves - mh;
}

Move*
Board_GeneratePseudoMovesOf(Board* board, Move* moves, Square sqr){
    Side side = Board_SIDE(board);
    Piece p = Board_PIECE(board, side, sqr);

    if (p == NCH_NO_PIECE)
        return moves;

    if (p == NCH_King){
        moves = generate_castle_moves(board, moves);
        moves = generate_king_moves(board, moves);
    }
    else{
        moves = generate_any_move(board, side, sqr, NCH_UINT64_MAX, moves);
    }
    return moves;
}