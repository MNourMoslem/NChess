#include "move.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

NCH_STATIC_INLINE int
is_valid_column(char arg){
    return arg >= 'a' && arg <= 'h';
}

NCH_STATIC_INLINE int
is_valid_row(char arg){
    return arg >= '1' && arg <= '8';
}

void
Move_Parse(Move move, Square* from_, Square* to_, uint8* castle, Piece* promotion){
    *from_ = Move_FROM(move);
    *to_ = Move_TO(move);
    *castle = Move_CASTLE(move);
    *promotion = Move_PRO_PIECE(move);
}

Move
Move_New(Square from_, Square to_, uint8 castle, Piece promotion){
    Move move;
    move = Move_ASSIGN_FROM(from_) | Move_ASSIGN_TO(to_) 
         | Move_ASSIGN_CASTLE(castle) | Move_ASSIGN_PRO_PIECE(promotion) ;
    return move;
}

int
Move_ParseFromString(Board* board, char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle){
    int len = strlen(arg);
    if (len > 5 || len < 4){
        return -1;
    }

    if (!is_valid_column(arg[0]) || !is_valid_row(arg[1])
         || !is_valid_column(arg[2]) || !is_valid_row(arg[3]))
    {
        return -1;
    }

    *from_ = ('h' - arg[0]) + 8 * (arg[1] - '1');
    *to_ = ('h' - arg[2]) + 8 * (arg[3] - '1');

    if (len == 5){
        if (arg[4] == 'q'){
            *promotion = NCH_Queen;
        }
        else if (arg[4] == 'r'){
            *promotion = NCH_Rook;
        }
        else if (arg[4] == 'b'){
            *promotion = NCH_Bishop;
        }
        else if (arg[4] == 'n'){
            *promotion = NCH_Knight;
        }
        else{
            return -1;
        }
    }
    else{
        *promotion = 0;
    }
    
    *castle = 0;
    if (Board_IS_WHITETURN(board)){
        if (*from_ == NCH_E1 && Board_WHITE_KING(board) == NCH_SQR(NCH_E1)){
            if (*to_ == NCH_G1){
                *castle |= Board_CASTLE_WK;
            }
            else if (*to_ == NCH_C1){
                *castle |= Board_CASTLE_WQ;
            }
        }
    }
    else{
        if (*from_ == NCH_E8 && Board_BLACK_KING(board) == NCH_SQR(NCH_E8)){
            if (*to_ == NCH_G8){
                *castle = Board_CASTLE_BK;
            }
            else if (*to_ == NCH_C8){
                *castle = Board_CASTLE_BQ;
            }
        }
    }

    return 0;
}

Move
Move_FromString(Board* board, char* move){
    Square from_, to_;
    Piece promotion;
    uint8 castle;
    Move_ParseFromString(board, move, &from_, &to_, &promotion, &castle);
    return Move_New(from_, to_, castle, promotion);
}

void
Move_Print(Move move){
    Square from_, to_;
    Piece promotion;
    uint8 castle;

    Move_Parse(move, &from_, &to_, &castle, &promotion);

    printf("from: %i\n", from_);
    printf("to: %i\n", to_);
    printf("castle: %s\n", NCH_CHKFLG(castle, Board_CASTLE_WK | Board_CASTLE_BK) ? "king"
                        : NCH_CHKFLG(castle, Board_CASTLE_WQ | Board_CASTLE_WQ) ? "queen" : "none");
    printf("promotion piece: %i\n", promotion);
}

NCH_STATIC_INLINE void*
get_target_map(Board* board, Side side, Square sqr){
    return board->piecetables[side][sqr] != NCH_NO_PIECE ? 
            &board->bitboards[side][board->piecetables[side][sqr]] :
            NULL;
}

NCH_STATIC_INLINE void
set_piece(Board* board, Side side, Square sqr, Piece piece){
    board->bitboards[side][piece] |= NCH_SQR(sqr);
    board->piecetables[side][sqr] = piece;
}

NCH_STATIC_INLINE void
remove_piece(Board* board, Side side, Square sqr){
    uint64* piece_map = get_target_map(board, side, sqr);
    if (piece_map){
        *piece_map &= ~NCH_SQR(sqr);
        board->piecetables[side][sqr] = NCH_NO_PIECE;
    }
}

NCH_STATIC_INLINE void
move_piece(Board* board, Side side, Square from_, Square to_){
    set_piece(board, side, to_, board->piecetables[side][from_]);
    remove_piece(board, side, from_);
}

void
make_promotion(Board* board, Side side, uint64 sqr, Piece promotion){
    if (promotion <= NCH_Pawn || promotion >= NCH_King){
        promotion = NCH_Queen;
    }

    remove_piece(board, side, sqr);
    set_piece(board, side, sqr, promotion);
    NCH_SETFLG(board->flags, Board_PROMOTION);
}

NCH_STATIC_INLINE int
is_pawn_move(Board* board, Side side, Square from_){
    return board->piecetables[side][from_] == NCH_Pawn;
}

NCH_STATIC_INLINE Piece
capture_piece_if_possible(Board* board, Side trg_side, Square sqr){
    uint64* piece_map = get_target_map(board, trg_side, sqr);
    if (piece_map){
        *piece_map &= ~NCH_SQR(sqr);
        Piece captured_piece = board->piecetables[trg_side][sqr];
        board->piecetables[trg_side][sqr] = NCH_NO_PIECE;
        NCH_SETFLG(board->flags, Board_CAPTURE);
        return captured_piece;
    }    
    return NCH_NO_PIECE;
}

NCH_STATIC_INLINE Piece 
play_pawn_move(Board* board, Side side, Square from_, Square to_, Piece promotion){
    move_piece(board, side, from_, to_);
    if (NCH_SQR(to_) == board->en_passant_trg){
       to_ = side == NCH_White ? to_ - 8 : to_ + 8; 
        NCH_SETFLG(board->flags, Board_ENPASSANT);
    }

    Piece captured_piece = capture_piece_if_possible(board, TARGET_SIDE(side), to_);
    NCH_SETFLG(board->flags, Board_PAWNMOVED);

    if (to_ - from_ == 16 || from_ - to_ == 16){
        set_board_enp_settings(board, side, to_);
    }
    else{
        reset_enpassant_variable(board);
    }

    if (to_ <= NCH_A1 || to_ >= NCH_H8){
        make_promotion(board, side, to_, promotion);
    }
    return captured_piece;
}

NCH_STATIC Piece
play_move(Board* board, Side side, Square from_, Square to_, Piece promotion){
    if (is_pawn_move(board, side, from_)){
        return play_pawn_move(board, side, from_, to_, promotion);
    }

    move_piece(board, side, from_, to_);
    Piece captured_piece = capture_piece_if_possible(board, TARGET_SIDE(side), to_);
    reset_enpassant_variable(board);
    return captured_piece;
}

void
play_castle_move(Board* board, Side side, Square from_, Square to_, int king_side){
    move_piece(board, side, from_, to_);
    if (king_side){
        move_piece(board, side, from_ - 3, to_ + 1);
    }
    else{
        move_piece(board, side, from_ + 4, to_ - 1);
    }
    reset_enpassant_variable(board);
}

Piece
make_move(Board* board, Square from_, Square to_, Piece promotion, uint8 castle){
    Piece captured_piece;
    if (castle){
        play_castle_move(board, Board_GET_SIDE(board), from_, to_,
                         NCH_CHKUNI(castle, Board_CASTLE_WK | Board_CASTLE_BK));
        captured_piece = NCH_NO_PIECE;
    }
    else{
        captured_piece = play_move(board, Board_GET_SIDE(board), from_, to_, promotion);
    }
    set_board_occupancy(board);
    return captured_piece;
}

void
undo_move(Board* board, Side side, Move move, int is_enpassant,
         int is_promotion, Piece last_captured_piece){
    Square from_;
    Square to_;
    uint8 castle;
    Piece promotion;
    Move_Parse(move, &from_, &to_, &castle, &promotion);
    
    if (castle){
        if (NCH_CHKUNI(castle, Board_CASTLE_WK | Board_CASTLE_BK)){
            move_piece(board, side, to_, from_);
            move_piece(board, side, to_ + 1, from_ - 3);
        }
        else{
            move_piece(board, side, to_, from_);
            move_piece(board, side,  to_ - 1, from_ + 4);
        }
    }
    else{
        if (is_promotion){
            remove_piece(board, side, to_);
            set_piece(board, side, from_, NCH_Pawn);
        }
        else{
            move_piece(board, side, to_, from_);
        }
        
        if (is_enpassant){
            set_piece(board, TARGET_SIDE(side), side == NCH_White ? to_ - 8 : to_ + 8, NCH_Pawn);
        }
        else if(last_captured_piece != NCH_NO_PIECE){
            set_piece(board, TARGET_SIDE(side), to_, last_captured_piece);
        }
    }


    set_board_occupancy(board);
}