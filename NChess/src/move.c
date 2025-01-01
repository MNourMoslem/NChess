#include "move.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>

NCH_STATIC_INLINE void
reset_enpassant_variable(Board* board){
    board->en_passant_idx = 0;
    board->en_passant_map = 0ull;
    board->en_passant_trg = 0ull;
}

NCH_STATIC_INLINE void*
get_target_map(Board* board, Side side, Square sqr){
    return board->piecetables[side][sqr] != NCH_NO_PIECE ? 
            &board->bitboards[side][board->piecetables[side][sqr]] :
            NULL;
}

NCH_STATIC_INLINE void
set_piece(Board* board, Side side, Square sqr, Piece piece){
    NCH_SETFLG(board->bitboards[side][piece], NCH_SQR(sqr));
    board->piecetables[side][sqr] = piece;
    NCH_SETFLG(board->occupancy[side], NCH_SQR(sqr));
    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);
}

NCH_STATIC_INLINE void
remove_piece(Board* board, Side side, Square sqr){
    uint64* piece_map = get_target_map(board, side, sqr);
    NCH_RMVFLG(*piece_map, NCH_SQR(sqr));
    board->piecetables[side][sqr] = NCH_NO_PIECE;
    NCH_RMVFLG(board->occupancy[side], NCH_SQR(sqr));
    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);
}

NCH_STATIC_INLINE void
move_piece(Board* board, Side side, Square from_, Square to_){
    uint64* piece_map = get_target_map(board, side, from_);
    NCH_RMVFLG(*piece_map, NCH_SQR(from_));
    NCH_SETFLG(*piece_map, NCH_SQR(to_));

    board->piecetables[side][to_] = board->piecetables[side][from_];
    board->piecetables[side][from_] = NCH_NO_PIECE;
    
    NCH_RMVFLG(board->occupancy[side], NCH_SQR(from_));
    NCH_SETFLG(board->occupancy[side], NCH_SQR(to_));
    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);
}

NCH_STATIC_INLINE void
capture_piece_if_possible(Board* board, Side trg_side, Square sqr){
    if (board->piecetables[trg_side][sqr] != NCH_NO_PIECE){
        remove_piece(board, trg_side, sqr);
        NCH_SETFLG(board->flags, Board_CAPTURE);
    }
}

void
make_promotion(Board* board, Side side, uint64 sqr, Piece promotion){
    if (promotion <= NCH_Pawn || promotion >= NCH_King){
        promotion = NCH_Queen;
    }

    remove_piece(board, side, sqr);
    set_piece(board, side, sqr, promotion);
}

NCH_STATIC_INLINE int
is_pawn_move(Board* board, Side side, Square from_){
    return board->piecetables[side][from_] == NCH_Pawn;
}

NCH_STATIC_INLINE void 
play_pawn_move(Board* board, Side side, Square from_, Square to_, Piece promotion){
    move_piece(board, side, from_, to_);
    if (NCH_SQR(to_) == board->en_passant_trg){
       to_ = side == NCH_White ? to_ - 8 : to_ + 8; 
    }

    Side trg_side = side == NCH_White ? NCH_Black : NCH_White;
    capture_piece_if_possible(board, trg_side, to_);
    NCH_SETFLG(board->flags, Board_PAWNMOVED);

    if (to_ - from_ == 16 || from_ - to_ == 16){
        board->en_passant_idx = to_;
        board->en_passant_map = NCH_SQR(to_) | (((NCH_NXTSQR_RIGHT(NCH_SQR(to_)) & 0x7f7f7f7f7f7f7f7f)
                                                |(NCH_NXTSQR_LEFT(NCH_SQR(to_)) & 0xfefefefefefefefe))
                                                & board->bitboards[trg_side][NCH_Pawn]);
        board->en_passant_trg = side == NCH_White ? NCH_NXTSQR_DOWN(NCH_SQR(to_))
                                                  : NCH_NXTSQR_UP(NCH_SQR(to_));
    }
    else{
        reset_enpassant_variable(board);
    }

    if (to_ <= NCH_A1 || to_ >= NCH_H8){
        make_promotion(board, side, to_, promotion);
    }
}

NCH_STATIC void
play_move(Board* board, Side side, Square from_, Square to_, Piece promotion){
    if (is_pawn_move(board, side, from_)){
        return play_pawn_move(board, side, from_, to_, promotion);
    }

    Side trg_side = side == NCH_White ? NCH_Black : NCH_White;
    move_piece(board, side, from_, to_);
    capture_piece_if_possible(board, trg_side, to_);
    reset_enpassant_variable(board);
}

void
play_castle_move(Board* board, Side side, Square from_, Square to_, int king_side){
    if (king_side){
        move_piece(board, side, from_, to_);
        move_piece(board, side, from_ - 3, to_ + 1);
    }
    else{
        move_piece(board, side, from_, to_);
        move_piece(board, side, from_ + 1, to_ - 1);
    }
}

void make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle){    
    if (castle){
        play_castle_move(board, Board_GET_SIDE(board), from_, to_, NCH_CHKUNI(castle, Board_CASTLE_WK | Board_CASTLE_BK));
    }
    else{
        play_move(board, Board_GET_SIDE(board), from_, to_, promotion);
    }
}   
