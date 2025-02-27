/*
    makemove.c

    This file contains declarations of all makemove.h functions.
*/

#include "makemove.h"
#include "move.h"
#include "utils.h"
#include "movelist.h"
#include "hash.h"
#include "generate.h"
#include "board_utils.h"

#include <stdlib.h>
#include <stdio.h>

NCH_STATIC_FINLINE void
set_piece(Board* board, Side side, Square sqr, Piece p){
    uint64 sqr_bb = NCH_SQR(sqr);
    Board_BB(board, p) |= sqr_bb;
    Board_OCC(board, side) |= sqr_bb;
    Board_PIECE(board, sqr) = p;
}

NCH_STATIC_FINLINE void
remove_piece(Board* board, Side side, Square sqr){
    uint64 sqr_bb = NCH_SQR(sqr);
    Piece p = Board_PIECE(board, sqr);
    Board_BB(board, p) &= ~sqr_bb;
    Board_OCC(board, side) &= ~sqr_bb;
    Board_PIECE(board, sqr) = NCH_NO_PIECE;
}

NCH_STATIC_FINLINE void
move_piece(Board* board, Side side, Square from_, Square to_){
    uint64 move_bb = NCH_SQR(from_) | NCH_SQR(to_);
    Piece p = Board_PIECE(board, from_);
    Board_BB(board, p) ^= move_bb;
    Board_OCC(board, side) ^= move_bb;
    Board_PIECE(board, from_) = NCH_NO_PIECE;
    Board_PIECE(board, to_) = p;
}

// makes a move on the board.
// it only modifies bitboard, piecetable and occupancy.
NCH_STATIC_FINLINE Piece
make_move(Board* board, Square from_, Square to_,
         MoveType move_type, PieceType pro_type)
{
    Piece  moveing_piece = Board_PIECE(board, from_);
    Piece captured_piece = Board_PIECE(board, to_);
    Side            side = Piece_SIDE(moveing_piece);
    Side         op_side = Piece_SIDE(captured_piece);

    move_piece(board, side, from_, to_);
    
    if (captured_piece != NCH_NO_PIECE){
        Board_BB(board, captured_piece) &= ~NCH_SQR(to_);
        Board_OCC(board, op_side) &= ~NCH_SQR(to_);
    }
    
    if (move_type != MoveType_Normal){
        if (move_type == MoveType_Castle){
            Square rook_from = Board_CASTLE_SQUARES(board, to_);
            Square rook_to = Board_CASTLE_SQUARES(board, rook_from);
            move_piece(board, side, rook_from, rook_to);
        }
        else if (move_type == MoveType_EnPassant){
            Square trg_sqr = side == NCH_White ? to_ - 8
                                               : to_ + 8;
                                               
            remove_piece(board, NCH_OP_SIDE(side), trg_sqr);
        }
        else{
            Piece pro_piece = PieceType_PIECE(side, pro_type);
            Piece      pawn = PieceType_PIECE(side, NCH_Pawn);
            
            Board_BB(board, pawn) &= ~NCH_SQR(to_);
            Board_BB(board, pro_piece) |= NCH_SQR(to_);
            Board_PIECE(board, to_) = pro_piece;
        }
    }
    
    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);
    
    return captured_piece;
}

// makes a move and also modifies board info.
NCH_STATIC_FINLINE Piece
move_and_set_flags(Board* board, Move move){    
    Square       from_ = Move_FROM(move);
    Square         to_ = Move_TO(move);
    MoveType      type = Move_TYPE(move);
    PieceType pro_type = Move_PRO_PIECE(move);

    PieceType moving_piece_type = Piece_TYPE(Board_PIECE(board, from_));
    Piece captured = make_move(board, from_, to_, type, pro_type);

    if (moving_piece_type == NCH_Pawn){
        NCH_SETFLG(Board_FLAGS(board), Board_PAWNMOVED);

        if (to_ - from_ == 16 || from_ - to_ == 16){
            set_board_enp_settings(board, Board_SIDE(board), to_);
        }

        if (type == MoveType_EnPassant){
            NCH_SETFLG(Board_FLAGS(board), Board_CAPTURE);
            return captured;
        }
    }
    if (captured != NCH_NO_PIECE){
        NCH_SETFLG(Board_FLAGS(board), Board_CAPTURE);
    }
    
    return captured;
}


// undo a move from the board.
// it only modifies bitboard, piecetable and occupancy.
NCH_STATIC_FINLINE void
undo_move(Board* board, Move move, Piece captured_piece){
    Square        from_ = Move_FROM(move);
    Square          to_ = Move_TO(move);
    MoveType       type = Move_TYPE(move);
    Piece moveing_piece = Board_PIECE(board, to_);
    Side           side = Piece_SIDE(moveing_piece);
    Side        op_side = Piece_SIDE(captured_piece);

    move_piece(board, side, to_, from_);

    if (type != MoveType_Normal){
        if (type == MoveType_Castle){
            Square rook_from = Board_CASTLE_SQUARES(board, to_);
            Square rook_to = Board_CASTLE_SQUARES(board, rook_from);
            move_piece(board, side, rook_to, rook_from);
        }
        else if (type == MoveType_EnPassant){
            Square trg_sqr = side == NCH_White ? to_ - 8
                                               : to_ + 8;

            Piece pawn = PieceType_PIECE(NCH_OP_SIDE(side), NCH_Pawn);
            set_piece(board, NCH_OP_SIDE(side), trg_sqr, pawn);
        }
        else{
            Piece pawn = PieceType_PIECE(side, NCH_Pawn);
            Board_BB(board, moveing_piece) &= ~NCH_SQR(from_);
            Board_BB(board, pawn) |= NCH_SQR(from_);
            Board_PIECE(board, from_) = pawn;
        }
    }

    if (captured_piece != NCH_NO_PIECE){
        set_piece(board, op_side, to_, captured_piece);
    }

    Board_ALL_OCC(board) = Board_WHITE_OCC(board) | Board_BLACK_OCC(board);
}

NCH_STATIC_INLINE int
is_move_legal(Board* board, Move move){
    Piece captured_piece = make_move(board, Move_FROM(move), Move_TO(move),
                                     Move_TYPE(move), Move_PRO_PIECE(move));
    int is_check = Board_IsCheck(board);
    undo_move(board, move, captured_piece);
    return !is_check;
}

Move 
Board_IsMoveLegal(Board* board, Move move){
    Move pseudo_moves[30];
    int n = Board_GeneratePseudoMovesOf(board, pseudo_moves, Move_FROM(move));

    Move ps;
    while(n > 0){
        ps = pseudo_moves[--n];
        if (Move_SAME_SQUARES(ps, move)){
            move = Move_REASSAGIN_TYPE(move, Move_TYPE(ps));
            if (is_move_legal(board, move))
                return move;
            break;
        }
    }
    
    return Move_NULL;
}

void
_Board_MakeMove(Board* board, Move move){
    MoveList_Append(&Board_MOVELIST(board), move, Board_INFO(board));

    Board_FLAGS(board) = 0;
    Board_ENP_MAP(board) = 0;
    Board_ENP_IDX(board) = 0;
    Board_ENP_TRG(board) = 0;

    Board_CAP_PIECE(board) = move_and_set_flags(board, move);
    
    BoardDict_Add(&Board_DICT(board), Board_BBS_PTR(board));

    reset_castle_rights(board);
    Board_NMOVES(board)++;
    Board_FIFTY_COUNTER(board) = NCH_CHKUNI(Board_FLAGS(board), Board_PAWNMOVED | Board_CAPTURE) 
                                ? 0
                                : Board_FIFTY_COUNTER(board) + 1;

    Board_SIDE(board) = NCH_OP_SIDE(Board_SIDE(board));
    update_check(board);
}

int
Board_StepByMove(Board* board, Move move){
    move = Board_IsMoveLegal(board, move);
    if (move == Move_NULL)
        return 0;

    _Board_MakeMove(board, move);
    return 1;
}

int
Board_Step(Board* board, char* move){
    Move m = Move_FromString(move);
    if (m == Move_NULL)
        return 0;

    m = Board_IsMoveLegal(board, m);
    if (m == Move_NULL)
        return 0;
    
    _Board_MakeMove(board, m);
    return 1;
}

void
Board_Undo(Board* board){
    MoveNode* node = MoveList_Last(&Board_MOVELIST(board));
    if (!node)
        return;

    BoardDict_Remove(&Board_DICT(board), Board_BBS_PTR(board));
    undo_move(board, node->move, Board_CAP_PIECE(board));

    Board_INFO(board) = node->pos_info;
    Board_NMOVES(board)--;

    MoveList_Pop(&Board_MOVELIST(board));
}

int
Board_GetMovesOf(Board* board, Square s, Move* moves){
    Move pseudo_moves[30];
    int n = Board_GeneratePseudoMovesOf(board, pseudo_moves, s);
    int nmoves = 0;
    for (int i = 0; i < n; i++){
        if (is_move_legal(board, pseudo_moves[i])){
            moves[nmoves++] = pseudo_moves[i];
        }
    }

    return nmoves;
}