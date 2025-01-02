#include "move.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>

uint32
save_move(Square from_, Square to_, uint8 castle, Piece promotion, int is_promotion, int is_enpassant, Piece captured_piece){
    uint32 move = 0u;
    move |= from_;
    move |= to_ << 6;
    move |= castle << 12;
    move |= promotion << 16;
    move |= is_promotion << 20;
    move |= is_enpassant << 21;
    move |= captured_piece << 22;
    return move;
}

void
parse_move(uint32 move, Square* from_, Square* to_, uint8* castle, Piece* promotion, int* is_promotion, int* is_enpassant, Piece* captured_piece){
    *from_ = move & 0x3F;
    *to_ = (move >> 6) & 0x3F;
    *castle = (move >> 12) & 0xF;
    *promotion = (move >> 16) & 0xF;
    *is_promotion = (move >> 20) & 0x1;
    *is_enpassant = (move >> 21) & 0x1;
    *captured_piece = (move >> 22) & 0xF;
}

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

NCH_STATIC_INLINE Piece
capture_piece_if_possible(Board* board, Side trg_side, Square sqr){
    if (board->piecetables[trg_side][sqr] != NCH_NO_PIECE){
        Piece captured_piece = board->piecetables[trg_side][sqr];
        remove_piece(board, trg_side, sqr);
        NCH_SETFLG(board->flags, Board_CAPTURE);
        return captured_piece;
    }
    return NCH_NO_PIECE;
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

NCH_STATIC_INLINE Piece 
play_pawn_move(Board* board, Side side, Square from_, Square to_, Piece promotion, int* is_promotion, int* is_enpassant){
    move_piece(board, side, from_, to_);
    if (NCH_SQR(to_) == board->en_passant_trg){
       to_ = side == NCH_White ? to_ - 8 : to_ + 8; 
       *is_enpassant = 1;
    }
    else{
        *is_enpassant = 0;
    }

    Side trg_side = side == NCH_White ? NCH_Black : NCH_White;
    Piece captured_piece = capture_piece_if_possible(board, trg_side, to_);
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
        *is_promotion = 1;
    }
    else{
        *is_promotion = 0;
    }

    return captured_piece;
}

NCH_STATIC Piece
play_move(Board* board, Side side, Square from_, Square to_, Piece promotion, int* is_promotion, int* is_enpassant){
    if (is_pawn_move(board, side, from_)){
        return play_pawn_move(board, side, from_, to_, promotion, is_promotion, is_enpassant);
    }
    *is_promotion = 0;
    *is_enpassant = 0;

    Side trg_side = side == NCH_White ? NCH_Black : NCH_White;
    move_piece(board, side, from_, to_);
    Piece captured_piece = capture_piece_if_possible(board, trg_side, to_);
    reset_enpassant_variable(board);
    return captured_piece;
}

void
play_castle_move(Board* board, Side side, Square from_, Square to_, int king_side){
    if (king_side){
        move_piece(board, side, from_, to_);
        move_piece(board, side, from_ - 3, to_ + 1);
    }
    else{
        move_piece(board, side, from_, to_);
        move_piece(board, side, from_ + 4, to_ - 1);
    }
    reset_enpassant_variable(board);
}

uint32
make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle){    
    Piece captured_piece;
    int is_promotion, is_enpassant;
    if (castle){
        play_castle_move(board, Board_GET_SIDE(board), from_, to_, NCH_CHKUNI(castle, Board_CASTLE_WK | Board_CASTLE_BK));
        captured_piece = NCH_NO_PIECE;
        is_promotion = 0;
        is_enpassant = 0;
    }
    else{
        captured_piece = play_move(board, Board_GET_SIDE(board), from_, to_, promotion, &is_promotion, &is_enpassant);
    }
    return save_move(from_, to_, castle, promotion, is_promotion, is_enpassant, captured_piece);
}   

void
undo_move(Board* board, Side side, uint32 move){
    Square from_;
    Square to_;
    uint8 castle;
    Piece promotion;
    int is_promotion;
    int is_enpassant;
    Piece captured_piece;
    parse_move(move, &from_, &to_, &castle, &promotion, &is_promotion, &is_enpassant, &captured_piece);

    if (castle){
        int king_side = NCH_CHKUNI(castle, Board_CASTLE_WK | Board_CASTLE_BK);
        if (king_side){
            move_piece(board, side, to_, from_);
            move_piece(board, side, to_ + 1, from_ - 3);
        }
        else{
            move_piece(board, side, to_, from_);
            move_piece(board, side,  to_ - 1, from_ + 4);
        }
    }
    else if (is_promotion){
        remove_piece(board, side, to_);
        set_piece(board, side, from_, NCH_Pawn);
    }
    else{
        move_piece(board, side, to_, from_);
    }
    
    if (is_enpassant){
        printf("it is enpassant!!!!!\n");
        set_piece(board, side == NCH_White ? NCH_Black : NCH_White, side == NCH_White ? to_ - 8 : to_ + 8, captured_piece);
    }
    else if(captured_piece != NCH_NO_PIECE){
        set_piece(board, side == NCH_White ? NCH_Black : NCH_White, to_, captured_piece);
    }
}