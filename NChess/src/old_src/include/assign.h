#ifndef NCHESS_SRC_INCLUDE_ASSIGN_H
#define NCHESS_SRC_INCLUDE_ASSIGN_H

#include "core.h"
#include "types.h"
#include "loops.h"

#include <stdio.h>

#define _NCH_ASSAIGN_BOARD_MAPS(board)  board->White_Map = NCH_B_GET_WHITEMAP(board);\
                                        board->Black_Map = NCH_B_GET_BLACKMAP(board);\
                                        board->All_Map = board->White_Map | board->Black_Map;



#define _NCH_ASSGIN_PIECE_TO_BOARD_STRING(piece_map, piece_char, piece_char_idx, board_idx, idx)\
piece_char = NCH_PIECES[piece_char_idx++];\
_NCH_MAP_LOOP(piece_map) {idx = 63 - move.idx; board_idx = idx + (idx / 8); board_str[board_idx] = piece_char;}\


#define _NCH_STEP_ALL_POSSIBLE_MOVES(map)\
_NCH_MAP_LOOP_NAME_SPECEFIC(board->map, piece){\
    _NCH_MAP_LOOP_NOIDX(pmoves[piece.idx]){\
        boards[idx] = *board;\
        _CBoard_Step(boards + idx, &boards[idx].map, piece.square, move.square, NCH_NoSM);\
        idx++;\
    }\
}


#define _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(map, last_row)\
_NCH_MAP_LOOP_NAME_SPECEFIC(board->map, piece){\
    _NCH_MAP_LOOP_NOIDX(pmoves[piece.idx]){\
        if (NCH_CHKFLG(last_row, move.square)){\
            boards[idx] = *board;\
            _CBoard_Step(boards + idx, &boards[idx++].map, piece.square, move.square, NCH_Promote2Queen);\
            boards[idx] = *board;\
            _CBoard_Step(boards + idx, &boards[idx++].map, piece.square, move.square, NCH_Promote2Rook);\
            boards[idx] = *board;\
            _CBoard_Step(boards + idx, &boards[idx++].map, piece.square, move.square, NCH_Promote2Knight);\
            boards[idx] = *board;\
            _CBoard_Step(boards + idx, &boards[idx++].map, piece.square, move.square, NCH_Promote2Bishop);\
        }\
        else{\
            boards[idx] = *board;\
            _CBoard_Step(boards + idx, &boards[idx].map, piece.square, move.square, NCH_NoSM);\
            idx++;\
        }\
    }\
}

#endif