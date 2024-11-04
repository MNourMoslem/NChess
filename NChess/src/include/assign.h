#ifndef NCHESS_ASSIGN_H
#define NCHESS_ASSIGN_H

#include "core.h"
#include "types.h"
#include "loops.h"

#include <stdio.h>

#define _NCH_ASSAIGN_BOARD_MAPS(board)  board->White_Map = NCH_B_GET_WHITEMAP(board);\
                                        board->Black_Map = NCH_B_GET_BLACKMAP(board);\
                                        board->All_Map = board->White_Map | board->Black_Map;

#define _NCH_SET_POSSIBLE_MOVES_FOR_PAWN(pawns, possible_moves, all_map, op_map, updown_func, updown2_func, rattack_func, lattack_func, start_row, enpassant_row) \
_NCH_MAP_LOOP(pawns){\
    possible_moves[move.idx] =    (rattack_func(move.square) & 0x7f7f7f7f7f7f7f7f & op_map)\
                                | (lattack_func(move.square) & 0xfefefefefefefefe & op_map)\
                                | (updown_func(move.square) & ~all_map)\
                                | (NCH_CHKFLG(start_row, move.square) && !NCH_CHKFLG(all_map, updown_func(move.square)) ?\
                                    updown2_func(move.square) : 0ull);\
\
    if (board->enpassant_sqr != 0ull && NCH_CHKFLG(enpassant_row, move.square)){\
        if (NCH_CHKFLG(NCH_NXTSQR_RIGHT(board->enpassant_sqr) & 0x7f7f7f7f7f7f7f7f, move.square)){\
            possible_moves[move.idx] = lattack_func(move.square);\
        }\
        else if (NCH_CHKFLG(NCH_NXTSQR_LEFT(board->enpassant_sqr) & 0xfefefefefefefefe, move.square)){\
            possible_moves[move.idx] = rattack_func(move.square);\
        }\
    }\
}

#define _NCH_ASSGIN_PIECE_TO_BOARD_STRING(piece_map, piece_char, piece_char_idx, board_idx, idx)\
piece_char = NCH_PIECES[piece_char_idx++];\
_NCH_MAP_LOOP(piece_map) {idx = 63 - move.idx; board_idx = idx + (idx / 8); board_str[board_idx] = piece_char;}\


#define _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, map_name, current_idx)\
_NCH_MAP_LOOP_NAME_SPECEFIC(board->map_name, piece){\
    _NCH_MAP_LOOP(possible_moves[piece.idx]) {\
        copy_board = *board;\
        _CBoard_Step(&copy_board, turn, &copy_board.map_name, piece.square, move.square, NCH_NoSM);\
        result_boards[current_idx++] = copy_board;\
    }\
}


#define _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(board, possible_moves, turn, copy_board, map_name, current_idx, last_row, promotions)\
_NCH_MAP_LOOP_NAME_SPECEFIC(board->map_name, piece){\
    _NCH_MAP_LOOP(possible_moves[piece.idx]) {\
        if (NCH_CHKFLG(last_row, move.square)){\
            for (int i = 0; i < 4; i++){\
                copy_board = *board;\
                _CBoard_Step(&copy_board, turn, &copy_board.map_name, piece.square, move.square, promotions[i]);\
                result_boards[current_idx++] = copy_board;\
            }\
        }\
        else{\
            copy_board = *board;\
            _CBoard_Step(&copy_board, turn, &copy_board.map_name, piece.square, move.square, NCH_NoSM);\
            result_boards[current_idx++] = copy_board;\
        }\
    }\
}

#endif