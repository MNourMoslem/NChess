#include "makemove.h"
#include "board_utils.h"
#include "move.h"
#include "utils.h"
#include "movelist.h"
#include "hash.h"
#include "generate.h"

#include <stdlib.h>

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
play_pawn_move(Board* board, Side side, Move move){
    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);

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
        make_promotion(board, side, to_, Move_PRO_PIECE(move));
    }
    return captured_piece;
}

NCH_STATIC_INLINE Piece
play_move(Board* board, Side side, Move move){
    if (is_pawn_move(board, side, Move_FROM(move))){
        return play_pawn_move(board, side, move);
    }

    move_piece(board, side, Move_FROM(move), Move_TO(move));
    Piece captured_piece = capture_piece_if_possible(board, TARGET_SIDE(side), Move_TO(move));
    reset_enpassant_variable(board);
    return captured_piece;
}

void
play_castle_move(Board* board, Side side, int king_side){
    if (side == NCH_White){
        if (king_side){
            move_piece(board, NCH_White, NCH_E1, NCH_G1);
            move_piece(board, NCH_White, NCH_H1, NCH_F1);
        }
        else{
            move_piece(board, NCH_White, NCH_E1, NCH_C1);
            move_piece(board, NCH_White, NCH_A1, NCH_D1);
        }
    }
    else{
        if (king_side){
            move_piece(board, NCH_Black, NCH_E8, NCH_G8);
            move_piece(board, NCH_Black, NCH_H8, NCH_F8);
        }
        else{
            move_piece(board, NCH_Black, NCH_E8, NCH_C8);
            move_piece(board, NCH_Black, NCH_A8, NCH_D8);
        }
    }
    reset_enpassant_variable(board);
}

Piece
make_move(Board* board, Move move){
    Piece captured_piece;
    if (Move_CASTLE(move)){
        play_castle_move(board, Board_GET_SIDE(board),
                         NCH_CHKUNI(Move_CASTLE(move), Board_CASTLE_WK | Board_CASTLE_BK));
        captured_piece = NCH_NO_PIECE;
    }
    else{
        captured_piece = play_move(board, Board_GET_SIDE(board), move);
    }
    set_board_occupancy(board);
    return captured_piece;
}

void
undo_move(Board* board, Side side, Move move, int is_enpassant,
         int is_promotion, Piece last_captured_piece){
    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);
    
    if (Move_CASTLE(move)){
        if (NCH_CHKUNI(Move_CASTLE(move), Board_CASTLE_WK | Board_CASTLE_BK)){
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

NCH_STATIC_INLINE void
increase_counter(Board* board){
    if (Board_IS_WHITETURN(board)){
        board->nmoves += 1;
    }

    if (NCH_CHKUNI(board->flags, Board_PAWNMOVED | Board_CAPTURE | Board_CHECK | Board_DOUBLECHECK)){
        board->fifty_counter = 0;
    }
    else{
        board->fifty_counter += 1;
    }
}

int 
Board_IsMoveLegal(Board* board, Move move){
    if (!board->generated){
        generate_moves(board);
    }

    return (board->piecetables[Board_GET_SIDE(board)][Move_FROM(move)] != NCH_NO_PIECE
            && board->moves[Move_FROM(move)] & Move_TO(move))
            ||
            (board->castle_moves & Move_CASTLE(move)); 
}

void
_Board_MakeMove(Board* board, Move move){
    MoveList_Append(board->movelist, move,
                     board->en_passant_idx, board->captured_piece,
                     board->fifty_counter, board->castles, board->flags);
    
    reset_every_turn_states(board);
    Piece captured_piece = make_move(board, move);
    board->captured_piece = captured_piece;

    BoardDict_Add(board->dict, board->bitboards);

    reset_castle_rigths(board);
    flip_turn(board);
    increase_counter(board);
    Board_Update(board);

    board->generated = 0;
}

void
Board_StepByMove(Board* board, Move move){
    if (Board_IsMoveLegal(board, move))
        _Board_MakeMove(board, move);
}

void
Board_Step(Board* board, char* move){
    Move m = Move_FromString(move);
    if (Board_IsMoveLegal(board, m))
        _Board_MakeMove(board, m);
}

void
Board_Undo(Board* board){
    MoveNode* node = MoveList_Last(board->movelist);
    if (!node){
        return;
    }
    BoardDict_Remove(board->dict, board->bitboards);

    undo_move(board, Board_GET_OP_SIDE(board),
             node->move, Board_IS_ENPASSANT(board),
             Board_IS_PROMOTION(board), board->captured_piece);

    if (MoveNode_ENP_SQR(node)){
        set_board_enp_settings(board, Board_GET_SIDE(board), MoveNode_ENP_SQR(node));
    }
    else{
        reset_enpassant_variable(board);
    }
    board->fifty_counter = MoveNode_FIFTY_COUNT(node);
    board->castles = MoveNode_CASTLE_FLAGS(node);
    board->flags = MoveNode_GAME_FLAGS(node);
    board->captured_piece = MoveNode_CAP_PIECE(node);

    if (Board_IS_BLACKTURN(board)){
        board->nmoves -= 1;
    }

    MoveList_Pop(board->movelist);
    Board_Update(board);

    board->generated = 0;
}