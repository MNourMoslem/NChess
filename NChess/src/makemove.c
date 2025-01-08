#include "makemove.h"
#include "board_utils.h"
#include "move.h"
#include "utils.h"
#include "movelist.h"
#include "hash.h"

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

NCH_STATIC_INLINE void
_Board_MakeMove(Board* board, Square from_, Square to_, Piece promotion, uint8 castle){
    if (!Move_IsValid(board, from_, to_)){
        return;
    }
    Square last_en_passant_idx = board->en_passant_idx;
    uint8 last_castles = board->castles;
    int last_flags = board->flags;
    int last_fifty_counter = board->fifty_counter;

    reset_every_turn_states(board);

    Piece captured_piece = make_move(board, from_, to_, promotion, castle);
    Move move = Move_New(from_, to_, castle, promotion);
    MoveList_Append(board->movelist, move,
                     Board_IS_ENPASSANT(board), Board_IS_PROMOTION(board),
                     last_en_passant_idx, captured_piece,
                     last_fifty_counter, last_castles, last_flags);

    BoardDict_Add(board->dict, board->bitboards);

    reset_castle_rigths(board);
    flip_turn(board);
    increase_counter(board);
    Board_Update(board);
}

void
Board_StepByMove(Board* board, Move move){
    if (!Board_GAME_ON(board)){
        return;
    }

    Square from_, to_;
    Piece promotion;
    uint8 castle;
    
    Move_Parse(move, &from_, &to_, &castle, &promotion);
    _Board_MakeMove(board, from_, to_, promotion, castle);
}

void
Board_Step(Board* board, char* move){
    if (!Board_GAME_ON(board)){
        return;
    }

    Square from_, to_;
    Piece promotion;
    uint8 castle;

    if (Move_ParseFromString(board, move, &from_, &to_, &promotion, &castle) != 0){
        return;
    }

    _Board_MakeMove(board, from_, to_, promotion, castle);
}

void
Board_Undo(Board* board){
    MoveNode* node = MoveList_Last(board->movelist);
    if (!node){
        return;
    }
    BoardDict_Remove(board->dict, board->bitboards);

    undo_move(board, Board_GET_OP_SIDE(board),
             node->move, MoveNode_ENPASSANT(node),
             MoveNode_PROMOTION(node), MoveNode_CAP_PIECE(node));

    if (MoveNode_ENP_SQR(node)){
        set_board_enp_settings(board, Board_GET_SIDE(board), MoveNode_ENP_SQR(node));
    }
    else{
        reset_enpassant_variable(board);
    }
    board->fifty_counter = MoveNode_FIFTY_COUNT(node);
    board->castles = MoveNode_CASTLE_FLAGS(node);
    board->flags = MoveNode_GAME_FLAGS(node);

    if (Board_IS_BLACKTURN(board)){
        board->nmoves -= 1;
    }

    MoveList_Pop(board->movelist);
    Board_Update(board);
}