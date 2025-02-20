#include "makemove.h"
#include "board_utils.h"
#include "move.h"
#include "utils.h"
#include "movelist.h"
#include "hash.h"
#include "generate.h"
#include "board_utils.h"

#include <stdlib.h>
#include <stdio.h>

NCH_STATIC_INLINE void
set_piece(Board* board, Side side, Square sqr, Piece p){
    uint64 sqr_bb = NCH_SQR(sqr);
    Board_BB(board, side, p) |= sqr_bb;
    Board_OCC(board, side) |= sqr_bb;
    Board_PIECE(board, side, sqr) = p;
}

NCH_STATIC_INLINE void
remove_piece(Board* board, Side side, Square sqr){
    uint64 sqr_bb = NCH_SQR(sqr);
    Piece p = Board_PIECE(board, side, sqr);
    Board_BB(board, side, p) &= ~sqr_bb;
    Board_OCC(board, side) &= ~sqr_bb;
    Board_PIECE(board, side, sqr) = NCH_NO_PIECE;
}


NCH_STATIC_INLINE void
move_piece(Board* board, Side side, Square from_, Square to_){
    uint64 move_bb = NCH_SQR(from_) | NCH_SQR(to_);
    Piece p = Board_PIECE(board, side, from_);
    Board_BB(board, side, p) ^= move_bb;
    Board_OCC(board, side) ^= move_bb;
    Board_PIECE(board, side, from_) = NCH_NO_PIECE;
    Board_PIECE(board, side, to_) = p;
}

NCH_STATIC_INLINE Piece
make_move(Board* board, Square from_, Square to_,
         MoveType move_type, Piece promotion_piece)
{
    Side side = Board_GET_SIDE(board);

    move_piece(board, side, from_, to_);
    Piece moving_piece = Board_PIECE(board, side, from_);
    Side op_side = side ^ NCH_Black;

    Piece captured_piece = Board_PIECE(board, op_side, to_);
    if (captured_piece != NCH_NO_PIECE){
        Board_PIECE(board, op_side, to_) = NCH_NO_PIECE;
        Board_OCC(board, op_side) &=  ~NCH_SQR(to_);

        NCH_SETFLG(board->info.flags, Board_CAPTURE);
    }

    if (moving_piece == NCH_Pawn){
        NCH_SETFLG(board->info.flags, Board_PAWNMOVED);
    }

    if (move_type != MoveType_Normal){
        if (move_type == MoveType_Castle){
            Square rook_from = Board_CASTLE_SQUARES(board, to_);
            Square rook_to = Board_CASTLE_SQUARES(board, rook_from);
            move_piece(board, side, rook_from, rook_to);
        }
        else if (move_type == MoveType_Enpassant){
            Square trg_sqr = side == NCH_White ? to_ - 8
                                               : to_ + 8;
    
            remove_piece(board, op_side, trg_sqr);

            NCH_SETFLG(board->info.flags, Board_CAPTURE);
        }
        else{
            Board_BB(board, side, NCH_Pawn) &= ~NCH_SQR(to_);
            Board_BB(board, side, promotion_piece) |= NCH_SQR(to_);
            Board_PIECE(board, side, to_) = promotion_piece;
        }
    }

    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);

    return captured_piece;
}

NCH_STATIC_INLINE Piece
make_move_by_move(Board* board, Move move){
    return make_move(board, Move_FROM(move), Move_TO(move), Move_TYPE(move), Move_PRO_PIECE(move));
}

NCH_STATIC_INLINE void
undo_move(Board* board, Move move){
    Side side = Board_GET_SIDE(board);
    Piece captured_piece = Board_CAP_PIECE(board);

    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);
    MoveType type = Move_TYPE(move);

    Side op_side = side ^ NCH_Black;

    move_piece(board, side, to_, from_);

    if (type != MoveType_Normal){
        if (type == MoveType_Castle){
            Square rook_from = Board_CASTLE_SQUARES(board, to_);
            Square rook_to = Board_CASTLE_SQUARES(board, rook_from);
            move_piece(board, side, rook_to, rook_from);
        }
        else if (type == MoveType_Enpassant){
            Square trg_sqr = side == NCH_White ? to_ - 8
                                               : to_ + 8;

            set_piece(board, op_side, trg_sqr, NCH_Pawn);
        }
        else{
            Piece promotion_piece = Move_PRO_PIECE(move);

            Board_BB(board, side, promotion_piece) &= ~NCH_SQR(from_);
            Board_BB(board, side, NCH_Pawn) |= NCH_SQR(from_);
            Board_PIECE(board, side, from_) = NCH_Pawn;
        }
    }

    if (captured_piece != NCH_NO_PIECE){
        set_piece(board, op_side, to_, captured_piece);
    }
}

NCH_STATIC_INLINE void
increase_counter(Board* board){
    board->nmoves++;

    if (NCH_CHKUNI(board->info.flags, Board_PAWNMOVED | Board_CAPTURE | Board_CHECK | Board_DOUBLECHECK)){
        board->info.fifty_counter = 0;
    }
    else{
        board->info.fifty_counter += 1;
    }
}

int 
Board_IsMoveLegal(Board* board, Move move){
    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);
    Piece promotion_piece = Move_PRO_PIECE(move);

    if (!is_valid_square(from_) || !is_valid_square(to_))
        return 0;

    Move pseudo_moves[30];
    Move* tail;
    tail = Board_GeneratePseudoMovesMapOf(board, pseudo_moves, from_);
    if (tail == pseudo_moves)
    return 0;
    
    int len = tail - pseudo_moves;
    int available = 0;
    Move ps;
    MoveType type;
    for (int i = 0; i < len; i++){
        ps = pseudo_moves[i];
        if (Move_FROM(ps) == from_ && Move_TO(ps) == to_){
            available = 1;
            type = Move_TYPE(ps);
            if (type == MoveType_Promotion){
                if (promotion_piece <= NCH_Pawn && promotion_piece >= NCH_King){
                    promotion_piece = NCH_Queen;
                }
            }
            break;
        }
    }
    
    if (!available)
        return 0;
    
    move = Move_New(from_, to_, promotion_piece, type);
    make_move_by_move(board, move);
    int is_check = Board_IsCheck(board);
    undo_move(board, move);

    if (is_check)
        return 0;
    return move;
}

void
_Board_MakeMove(Board* board, Move move){
    MoveList_Append(&board->movelist, move, board->info);
    
    reset_every_turn_states(board);
    Board_CAP_PIECE(board) = make_move_by_move(board, move);

    BoardDict_Add(&board->dict, board->bitboards);

    reset_castle_rights(board);
    flip_turn(board);
    increase_counter(board);
    update_check(board);
}

int
Board_StepByMove(Board* board, Move move){
    move = Board_IsMoveLegal(board, move);
    if (move){
        _Board_MakeMove(board, move);
        return 1;
    }
    return 0;
}

int
Board_Step(Board* board, char* move){
    Move m = Move_FromString(move);
    m = Board_IsMoveLegal(board, m);
    if (m){
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
    undo_move(board, node->move);

    board->info = node->pos_info;
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

    tail = Board_GeneratePseudoMovesMapOf(board, pseudo_moves, s);

    int len = (int)(tail - pseudo_moves);
    int nmoves = 0;
    for (int i = 0; i < len; i++){
        make_move_by_move(board, pseudo_moves[i]);
        int is_check = Board_IsCheck(board);
        if (!is_check){
            moves[nmoves] = pseudo_moves[i];
            nmoves++;
        }

        undo_move(board, pseudo_moves[i]);
    }

    return nmoves;
}