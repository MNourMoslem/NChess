#include "generate.h"
#include "generate_utils.h"
#include "bitboard.h"
#include <string.h>
#include <stdio.h>

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