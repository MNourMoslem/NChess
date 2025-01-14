#ifndef NCHESS_SRC_BOARD_UTILS_H
#define NCHESS_SRC_BOARD_UTILS_H

#include "core.h"
#include "board.h"
#include "types.h"
#include "config.h"

NCH_STATIC_INLINE void
end_game_by_draw(Board* board, int state){
    NCH_SETFLG(board->flags, Board_GAMEEND | Board_DRAW | state);
}

NCH_STATIC_INLINE void
end_game_by_wl(Board* board){
    NCH_SETFLG(board->flags, Board_GAMEEND | (Board_IS_WHITETURN(board) ? 0 : Board_WIN));
}

NCH_STATIC_INLINE int
at_least_one_move(Board* board){
    return 1;
}

NCH_STATIC_INLINE void
flip_turn(Board* board){
    NCH_FLPFLG(board->flags, Board_TURN);
}

NCH_STATIC_INLINE void
reset_state_flags(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK);
}

NCH_STATIC_INLINE void
reset_every_turn_states(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK 
                           | Board_CAPTURE | Board_PAWNMOVED 
                           | Board_ENPASSANT | Board_PROMOTION);
}

NCH_STATIC_INLINE void
reset_castle_rigths(Board* board){
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

#endif