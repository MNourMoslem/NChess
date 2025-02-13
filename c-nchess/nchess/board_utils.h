/*
    board_utils.h

    Contains private functions used by nchess.Board and other helper functions  
    used by various components.  

    All functions in this file are used a couple of times by specific functions,  
    which is why all functions are inlined.
*/

#ifndef NCHESS_SRC_BOARD_UTILS_H
#define NCHESS_SRC_BOARD_UTILS_H

#include "core.h"
#include "board.h"
#include "types.h"
#include "config.h"
#include "generate_utils.h"

/*
    The flip_turn and reset functions run every time a move is made on the board,  
    whether it is making a move or undoing one.  

    The update_check function also runs every step, but it is also used separately  
    when the board is intilized.
*/

NCH_STATIC_INLINE void
flip_turn(Board* board){
    NCH_FLPFLG(board->flags, Board_TURN);
}

NCH_STATIC_INLINE void
reset_every_turn_states(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK 
                           | Board_CAPTURE | Board_PAWNMOVED 
                           | Board_ENPASSANT | Board_PROMOTION);
}

NCH_STATIC_INLINE void
reset_castle_rights(Board* board){
    if (NCH_CHKFLG(board->castles, Board_CASTLE_WK) &&
        !NCH_CHKFLG(Board_WHITE_OCC(board), (NCH_SQR(NCH_E1) | NCH_SQR(NCH_H1))))
    {
        NCH_RMVFLG(board->castles, Board_CASTLE_WK);
    }
    if (NCH_CHKFLG(board->castles, Board_CASTLE_WQ) && 
        !NCH_CHKFLG(Board_WHITE_OCC(board), (NCH_SQR(NCH_E1) | NCH_SQR(NCH_A1))))
    {
        NCH_RMVFLG(board->castles, Board_CASTLE_WQ);
    }
    if (NCH_CHKFLG(board->castles, Board_CASTLE_BK) &&
        !NCH_CHKFLG(Board_BLACK_OCC(board), (NCH_SQR(NCH_E8) | NCH_SQR(NCH_H8))))
    {
        NCH_RMVFLG(board->castles, Board_CASTLE_BK);
    }
    if (NCH_CHKFLG(board->castles, Board_CASTLE_BQ) &&
        !NCH_CHKFLG(Board_BLACK_OCC(board), (NCH_SQR(NCH_E8) | NCH_SQR(NCH_A8))))
    {
        NCH_RMVFLG(board->castles, Board_CASTLE_BQ);
    }
}

NCH_STATIC_FINLINE void
update_check(Board* board){
    uint64 check_map = get_checkmap(
        board,
        Board_GET_SIDE(board),
        NCH_SQRIDX(Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
        Board_ALL_OCC(board)
    );

    if (check_map)
        NCH_SETFLG(board->flags, more_then_one(check_map) ? Board_CHECK | Board_DOUBLECHECK : Board_CHECK);
}

#endif
