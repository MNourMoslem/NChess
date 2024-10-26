#ifndef NCHESS_ASSIGN_H
#define NCHESS_ASSIGN_H

#include "core.h"
#include "types.h"
#include "loops.h"

#include <stdio.h>

#define _NCH_VISION_WHILEASSIGN(current, square, move_func, vmap, block_map)\
current = square;\
while(1){\
    current = move_func(current);\
    vmap |= current;\
\
    if (NCH_CHKFLG(block_map, current)){\
        break;\
    }\
}

#define _NCH_VISION_WHILEASSIGN_SIDES(current, square, move_func, vmap, block_map, block_col, block_temp)\
if (!NCH_CHKFLG(block_col, square)){\
    block_temp = block_map | block_col;\
    _NCH_VISION_WHILEASSIGN(current, square, move_func, vmap, block_temp)\
}

#define _NCH_VISION_WHILEASSIGN_SIDES_BISHOP(current, square, move_func_up, move_func_down, vmap, block_map, block_col, block_temp)\
if (!NCH_CHKFLG(block_col, square)){\
    block_temp = block_map | block_col;\
    _NCH_VISION_WHILEASSIGN(current, square, move_func_up, vmap, block_temp)\
    \
    _NCH_VISION_WHILEASSIGN(current, square, move_func_down, vmap, block_temp)\
}

#define _NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, capture_sqr, block_map, op_map, king_effect_map, piece_map)\
if (!NCH_CHKFLG(block_map, current)){\
    if (NCH_CHKFLG(king_effect_map, square) && (_CBoard_MoveAndCheck(board, turn, piece_map, square, current, capture_sqr, op_map) == 1)){\
            current = 0ull;\
    }\
    board->possible_moves[idx] |= current;\
}


#define _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, move_func, square, last_square, current, capture_sqr, block_map, op_map, king_effect_map, piece_map)\
current = move_func(last_square);\
_NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, capture_sqr, block_map, op_map, king_effect_map, piece_map)


#define _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, move_func, square, current, capture_sqr, block_map, op_map, stop_map, king_effect_map, piece_map)\
current = square;\
while(1){\
    _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, move_func, square, current, current, current, block_map, op_map, king_effect_map, piece_map) \
    if (NCH_CHKFLG(stop_map, current)){\
        break;\
    }\
}


#define _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES(board, turn, idx, move_func, square, current, capture_sqr, block_col, block_map, block_temp, op_map, king_effect_map, piece_map)\
if (!NCH_CHKFLG(block_col, square)){\
    block_temp = block_col | block_map;\
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, move_func, square, current, capture_sqr, block_map, op_map, block_temp, king_effect_map, piece_map)\
}


#define _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES_BISHOP(board, turn, idx, move_func_up, move_func_down, square, current, capture_sqr, block_col, block_map, block_temp, op_map, king_effect_map, piece_map)\
if (!NCH_CHKFLG(block_col, square)){\
    block_temp = block_col | block_map;\
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, move_func_up, square, current, capture_sqr, block_map, op_map, block_temp, king_effect_map, piece_map)\
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, move_func_down, square, current, capture_sqr, block_map, op_map, block_temp, king_effect_map, piece_map)\
}


#define _NCH_POSSIBLEMOVES_PAWN(board, piece_map, turn, idx, sqaure, king_effect_map, updown_func, rightattack_func, leftattack_func, first_row, enpassant_row)\
current = updown_func(square);\
_NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, current, all_map, op_map, king_effect_map, piece_map)\
\
if (NCH_CHKFLG(first_row, square)){\
    current = updown_func(current);\
    _NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, current, all_map, op_map, king_effect_map, piece_map)\
}\
\
if (NCH_CHKFLG(enpassant_row, square) && NCH_B_IS_PAWNMOVED2SQR(board)){\
    int trg_col = NCH_B_GET_PAWNCOL(board);\
    int col = NCH_GETCOL(square);\
\
    if (col == (trg_col - 1)){\
        current = rightattack_func(square);\
        _NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, NCH_NXTSQR_RIGHT(square), all_map, op_map, king_effect_map, piece_map)\
        return;\
    }\
\
    if (col == (trg_col + 1)){\
        current = leftattack_func(square);\
        _NCH_POSSIBLEMOVES_ASSIGN(board, turn, idx, square, current, NCH_NXTSQR_LEFT(square), all_map, op_map, king_effect_map, piece_map)\
    }\
}


#define _NCH_SET_POSSIBLEMOVE(func, board, piece_map, turn, king_effect_map, ply_map, op_map)\
if (piece_map != 0){\
    _NCH_MAP_LOOP(piece_map){\
        func(board, &piece_map, turn, move.idx, move.square, king_effect_map, ply_map, op_map);\
    }\
}


#define _NCH_ASSGIN_PIECE_TO_BOARD_STRING(piece_map, piece_char, piece_char_idx, board_idx, idx)\
piece_char = NCH_PIECES[piece_char_idx++];\
_NCH_MAP_LOOP(piece_map) {idx = 63 - move.idx; board_idx = idx + (idx / 8); board_str[board_idx] = piece_char;}\


#endif