/*
    utils.h

    This file contains utility inline functions that are used throughout the program.
*/


#ifndef NCHESS_SRC_UTILS_H
#define NCHESS_SRC_UTILS_H

#include "types.h"
#include "board.h"
#include "config.h"
#include "loops.h"
#include "bitboard.h"

#define TARGET_SIDE(side) (side == NCH_White ? NCH_Black : NCH_White)

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

NCH_STATIC_INLINE void
set_board_enp_settings(Board* board, Side side, Square enp_sqr){
    Square trg_sqr = side == NCH_White ? enp_sqr - 8 : enp_sqr + 8;
    Board_ENP_IDX(board) = enp_sqr;
    Board_ENP_MAP(board) = NCH_SQR(enp_sqr) | (bb_pawn_attacks(side, trg_sqr)
                         & Board_BB(board, NCH_OP_SIDE(side), NCH_Pawn));
    Board_ENP_TRG(board) = NCH_SQR(trg_sqr);
}


NCH_STATIC_INLINE void
init_piecetables(Board* board){
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_WHITE_PIECE(board, i) = NCH_NO_PIECE;
    }

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_BLACK_PIECE(board, i) = NCH_NO_PIECE;
    }

    int idx;
    LOOP_U64_T(Board_WHITE_PAWNS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_WHITE_KNIGHTS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_WHITE_BISHOPS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_WHITE_ROOKS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_WHITE_QUEENS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_WHITE_KING(board)){
        Board_WHITE_PIECE(board, idx) = NCH_King;
    }

    LOOP_U64_T(Board_BLACK_PAWNS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_BLACK_KNIGHTS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_BLACK_BISHOPS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_BLACK_ROOKS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_BLACK_QUEENS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_BLACK_KING(board)){
        Board_BLACK_PIECE(board, idx) = NCH_King;
    }
}

NCH_STATIC_INLINE void
reset_enpassant_variable(Board* board){
    Board_ENP_IDX(board)= 0;
    Board_ENP_MAP(board) = 0ull;
    Board_ENP_TRG(board) = 0ull;
}

NCH_STATIC_INLINE void
set_board_occupancy(Board* board){
    Board_OCC(board, NCH_White) = Board_BB(board, NCH_White, NCH_Pawn)
                                | Board_BB(board, NCH_White, NCH_Knight)
                                | Board_BB(board, NCH_White, NCH_Bishop)
                                | Board_BB(board, NCH_White, NCH_Rook)
                                | Board_BB(board, NCH_White, NCH_Queen)
                                | Board_BB(board, NCH_White, NCH_King);

    Board_OCC(board, NCH_Black) = Board_BB(board, NCH_Black, NCH_Pawn)
                                | Board_BB(board, NCH_Black, NCH_Knight)
                                | Board_BB(board, NCH_Black, NCH_Bishop)
                                | Board_BB(board, NCH_Black, NCH_Rook)
                                | Board_BB(board, NCH_Black, NCH_Queen)
                                | Board_BB(board, NCH_Black, NCH_King);

    Board_ALL_OCC(board) = Board_OCC(board, NCH_Black) 
                         | Board_OCC(board, NCH_White);
}

NCH_STATIC_INLINE int
is_valid_column(const char arg){
    return arg >= 'a' && arg <= 'h';
}

NCH_STATIC_INLINE int
is_valid_row(const char arg){
    return arg >= '1' && arg <= '8';
}

NCH_STATIC_INLINE int
is_valid_square(Square s){
    return (s >= 0) && (s < NCH_SQUARE_NB);
}

NCH_STATIC_INLINE int
char_to_col(const char c){
    return 'h' - c;
}

NCH_STATIC_INLINE int
char_to_row(const char c){
    return c - '1';
}

NCH_STATIC_INLINE Square
str_to_square(const char* sq_str){
    const char col_char = sq_str[0];
    const char row_char = sq_str[1];
    
    if (!is_valid_column(col_char) || !is_valid_row(row_char)){
        return NCH_NO_SQR;
    }

    int col = char_to_col(col_char);
    int row = char_to_row(row_char);

    return (Square)(col + 8 * row);
}

NCH_STATIC_INLINE int
is_valid_piece(Piece p){
    return p >= NCH_Pawn && p <= NCH_King;
}

NCH_STATIC_INLINE int
is_valid_side(Side s){
    return s == NCH_White || s == NCH_Black;
}

#endif