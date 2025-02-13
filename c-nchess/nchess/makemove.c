#include "makemove.h"
#include "board_utils.h"
#include "move.h"
#include "utils.h"
#include "movelist.h"
#include "hash.h"
#include "generate.h"
#include "generate_utils.h"
#include "board_utils.h"

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
    }

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
    return capture_piece_if_possible(board, TARGET_SIDE(side), to_);
}

NCH_STATIC_INLINE Piece
play_move(Board* board, Side side, Move move){
    if (is_pawn_move(board, side, Move_FROM(move))){
        return play_pawn_move(board, side, move);
    }

    reset_enpassant_variable(board);
    move_piece(board, side, Move_FROM(move), Move_TO(move));
    return capture_piece_if_possible(board, TARGET_SIDE(side), Move_TO(move));
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
undo_move(Board* board, Side side, Move move, Piece last_captured_piece){
    if (Move_CASTLE(move)){
        if (side == NCH_White){
            if (Move_CASTLE(move) & Board_CASTLE_WK){
                move_piece(board, NCH_White, NCH_G1, NCH_E1);
                move_piece(board, NCH_White, NCH_F1, NCH_H1);
            }
            else{
                move_piece(board, NCH_White, NCH_C1, NCH_E1);
                move_piece(board, NCH_White, NCH_D1, NCH_A1);
            }
        }
        else{
            if (Move_CASTLE(move) & Board_CASTLE_BK){
                move_piece(board, NCH_Black, NCH_G8, NCH_E8);
                move_piece(board, NCH_Black, NCH_F8, NCH_H8);
            }
            else{
                move_piece(board, NCH_Black, NCH_C8, NCH_E8);
                move_piece(board, NCH_Black, NCH_D8, NCH_A8);
            }
        }
    }
    else{
        Square from_ = Move_FROM(move);
        Square to_ = Move_TO(move);

        if (Move_IS_PRO(move)){
            remove_piece(board, side, to_);
            set_piece(board, side, from_, NCH_Pawn);
        }
        else{
            move_piece(board, side, to_, from_);
        }
        
        if (Move_IS_ENP(move)){
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
    board->nmoves += 1;

    if (NCH_CHKUNI(board->flags, Board_PAWNMOVED | Board_CAPTURE | Board_CHECK | Board_DOUBLECHECK)){
        board->fifty_counter = 0;
    }
    else{
        board->fifty_counter += 1;
    }
}

int 
Board_IsMoveLegal(Board* board, Move move){
    Piece p = board->piecetables[Board_GET_SIDE(board)][Move_FROM(move)];
    if (p == NCH_NO_PIECE)
        return 0;

    uint64 allowed_squares = Board_IS_WHITETURN(board) ? ~Board_WHITE_OCC(board) : ~Board_BLACK_OCC(board);
    
    Move pseudo_moves[30];
    Move* tail;

    if (Move_CASTLE(move)){
        tail = generate_castle_moves(board, pseudo_moves);
    }
    else if (p == NCH_King){
        tail = generate_king_moves(board, pseudo_moves);
    }
    else{
        tail = generate_any_move(board, Board_GET_SIDE(board), Move_FROM(move), Board_ALL_OCC(board), allowed_squares, pseudo_moves);
    }

    if (tail == pseudo_moves)
        return 0;

    int len = tail - pseudo_moves;
    int available = 0;
    Move ps;
    for (int i = 0; i < len; i++){
        ps = pseudo_moves[i] &~ (Move_ASSIGN_IS_ENP(1) | Move_ASSIGN_IS_PRO(1));
        if (move == ps){
            move = ps;
            available = 1;
            break;
        }
    }

    if (!available)
        return 0;

    Piece captured = make_move(board, move);
    int is_check = Board_IsCheck(board);
    undo_move(board, Board_GET_SIDE(board), move, captured);

    return !is_check;
}

void
_Board_MakeMove(Board* board, Move move){
    MoveList_Append(&board->movelist, move,
                     board->en_passant_idx, board->captured_piece,
                     board->fifty_counter, board->castles, board->flags);
    
    reset_every_turn_states(board);
    board->captured_piece = make_move(board, move);

    BoardDict_Add(&board->dict, board->bitboards);

    reset_castle_rigths(board);
    flip_turn(board);
    increase_counter(board);
    update_check(board);
}

int
Board_StepByMove(Board* board, Move move){
    if (Board_IsMoveLegal(board, move)){
        _Board_MakeMove(board, move);
        return 1;
    }
    return 0;
}

int
Board_Step(Board* board, char* move){
    Move m = Move_FromString(move);
    if (m && Board_IsMoveLegal(board, m)){
        _Board_MakeMove(board, m);
        return 1;
    }
    return 0;
}

void
Board_Undo(Board* board){
    MoveNode* node = MoveList_Last(&board->movelist);
    if (!node)
        return;

    BoardDict_Remove(&board->dict, board->bitboards);

    undo_move(board, Board_GET_OP_SIDE(board), node->move, board->captured_piece);

    if (MoveNode_ENP_SQR(node)){
        set_board_enp_settings(board, Board_GET_SIDE(board), MoveNode_ENP_SQR(node));
    }
    else{
        reset_enpassant_variable(board);
    }
    board->fifty_counter  = MoveNode_FIFTY_COUNT(node);
    board->castles        = MoveNode_CASTLE_FLAGS(node);
    board->flags          = MoveNode_GAME_FLAGS(node);
    board->captured_piece = MoveNode_CAP_PIECE(node);
    board->nmoves -= 1;

    MoveList_Pop(&board->movelist);
}

int
Board_GetMovesOf(Board* board, Square s, Move* moves){
    Piece p = Board_ON_SQUARE(board, s);
    if (p == NCH_NO_PIECE)
        return 0;

    Move pseudo_moves[30];
    Move* tail = pseudo_moves;

    if (p == NCH_King){
        tail = generate_castle_moves(board, tail);
        tail = generate_king_moves(board, tail);
    }
    else{
        uint64 allowed_squares = Board_IS_WHITETURN(board) ? ~Board_WHITE_OCC(board) : ~Board_BLACK_OCC(board);
        tail = generate_any_move(board, Board_GET_SIDE(board),
                                s, Board_ALL_OCC(board),
                                allowed_squares, pseudo_moves);
    }

    int len = (int)(tail - pseudo_moves);
    int nmoves = 0;
    for (int i = 0; i < len; i++){
        Piece captured = make_move(board, pseudo_moves[i]);
        int is_check = Board_IsCheck(board);
        if (!is_check){
            moves[nmoves] = pseudo_moves[i];
            nmoves++;
        }

        undo_move(board, Board_GET_SIDE(board), pseudo_moves[i], captured);
    }

    return nmoves;
}