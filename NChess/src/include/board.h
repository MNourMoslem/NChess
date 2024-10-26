#ifndef NCHESS_BOARD_H
#define NCHESS_BOARD_H

#include "assign.h"
#include "types.h"
#include "core.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define _CBoard_CLEAN_POSSIBLEMOVES(board) memset(board->possible_moves, 0, sizeof(board->possible_moves))

void _CBoard_PiecesMovesAsString(CBoard* board, cuint64* piece_map, int* move_idx, char moves_str[][8], char piece_char, cuint64 op_map){
    int N = NCH_POPCOUNT(*piece_map);
    int* indices = malloc(N * sizeof(int));
    cuint64* moves = malloc(N * sizeof(cuint64));
    int j = 0;

    _NCH_MAP_LOOP(*piece_map){
        indices[j] = move.idx;
        moves[j] = board->possible_moves[move.idx];
        j++;
    }

    int idx = *move_idx;
    int n_moves = 0;
    for (int i = 0, current; i < N; i++){
        _NCH_MAP_LOOP(moves[i]) {
            current = 0;
            moves_str[idx][current++] = piece_char;

            if (NCH_CHKFLG(op_map, move.square)){
                moves_str[idx][current++] = 'x';
            }

            moves_str[idx][current++] = 'h' - (indices[i] % 8);
            moves_str[idx][current++] = (indices[i] / 8) + '1';
            moves_str[idx][current++] = '-';

            moves_str[idx][current++] = 'h' - (move.idx % 8);
            moves_str[idx][current++] = (move.idx / 8) + '1';
            moves_str[idx][current] = '\0';
            idx++;
            n_moves++;
        }
    }
    *move_idx += n_moves;

    free(indices);
    free(moves);
}

void _CBoard_PiecesMovesAsString_Pawn(CBoard* board, cuint64* piece_map, int* move_idx, char moves_str[][8], char piece_char, cuint64 op_map, cuint64 last_row){
    int N = NCH_POPCOUNT(*piece_map);
    int* indices = malloc(N * sizeof(int));
    cuint64* moves = malloc(N * sizeof(cuint64));
    int j = 0;

    _NCH_MAP_LOOP(*piece_map){
        indices[j] = move.idx;
        moves[j] = board->possible_moves[move.idx];
        j++;
    }

    int idx = *move_idx;
    int n_moves = 0;
    for (int i = 0, current; i < N; i++){
        _NCH_MAP_LOOP(moves[i]) {
            current = 0;

            moves_str[idx][current++] = 'h' - (indices[i] % 8);
            moves_str[idx][current++] = (indices[i] / 8) + '1';

            if (NCH_CHKFLG(op_map, move.square)){
                moves_str[idx][current++] = 'x';
            }else{
                moves_str[idx][current++] = '-';
            }

            moves_str[idx][current++] = 'h' - (move.idx % 8);
            moves_str[idx][current++] = (move.idx / 8) + '1';

            if (NCH_CHKFLG(last_row, move.square)){
                for (int i = 1, last_idx = idx; i < 5; i++){
                    memcpy(moves_str[idx] ,moves_str[last_idx], sizeof(char) * 5);

                    moves_str[idx][6] = '=';
                    moves_str[idx][7] = NCH_PIECES[i];
                    moves_str[idx][8] = '\0';
                    idx++;
                    n_moves++;
                }
                continue;
            }

            moves_str[idx][current] = '\0';
            idx++;
            n_moves++;
        }
    }
    *move_idx += n_moves;

    free(indices);
    free(moves);
}

void CBoard_PossibleMovesAsString(CBoard* board, char moves_str[][8]){
    int move_idx = 0;
    int piece_char_idx = 0;

    _CBoard_PiecesMovesAsString_Pawn(board, &board->W_Pawns, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map, NCH_ROW8);
    _CBoard_PiecesMovesAsString(board, &board->W_Knights, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
    _CBoard_PiecesMovesAsString(board, &board->W_Bishops, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
    _CBoard_PiecesMovesAsString(board, &board->W_Rooks, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
    _CBoard_PiecesMovesAsString(board, &board->W_Queens, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
    _CBoard_PiecesMovesAsString(board, &board->W_King, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);

    piece_char_idx = 0;
    _CBoard_PiecesMovesAsString_Pawn(board, &board->B_Pawns, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map, NCH_ROW1);
    _CBoard_PiecesMovesAsString(board, &board->B_Knights, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
    _CBoard_PiecesMovesAsString(board, &board->B_Bishops, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
    _CBoard_PiecesMovesAsString(board, &board->B_Rooks, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
    _CBoard_PiecesMovesAsString(board, &board->B_Queens, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
    _CBoard_PiecesMovesAsString(board, &board->B_King, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
}

int CBoard_NumberPossibleMoves(CBoard* board){
    int count = 0;
    for (int i = 0; i < 64; i++){
        count += NCH_POPCOUNT(board->possible_moves[i]);
    }

    if (NCH_B_IS_WHITETURN(board)){
        _NCH_MAP_LOOP(board->W_Pawns){
            if (NCH_CHKUNI(NCH_ROW8 ,board->possible_moves[move.idx])){
                count += NCH_POPCOUNT(board->possible_moves[move.idx]) * 4;
            }
        }
    }
    else{
        _NCH_MAP_LOOP(board->B_Pawns){
            if (NCH_CHKUNI(NCH_ROW1 ,board->possible_moves[move.idx])){
                count += NCH_POPCOUNT(board->possible_moves[move.idx]) * 4;
            }
        }
    }
    return count;
}

void CBoard_AsString(CBoard* board, char board_str[NCH_B_STRING_SIZE]){
    int board_idx = 0, idx;
    
    char piece_char;
    int piece_char_idx = 0;

    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_Pawns, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_Knights, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_Bishops, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_Rooks, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_Queens, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->W_King, piece_char, piece_char_idx, board_idx, idx)
    
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_Pawns, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_Knights, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_Bishops, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_Rooks, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_Queens, piece_char, piece_char_idx, board_idx, idx)
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(board->B_King, piece_char, piece_char_idx, board_idx, idx)

    cuint64 map_reverse = ~(board->White_Map | board->Black_Map);
    _NCH_ASSGIN_PIECE_TO_BOARD_STRING(map_reverse, piece_char, piece_char_idx, board_idx, idx)

    for (int i = 8; i < 72; i+=9){
        board_str[i] = '\n';
    }

    board_str[72] = '\0';
}

void CBoard_Print(CBoard* board){
    char board_str[NCH_B_STRING_SIZE];

    CBoard_AsString(board, board_str);

    printf(board_str);
}

cuint64 CBoard_KnightVision(cuint64 square, cuint64 block_map){
    cuint64 vmap = 0ull;
    int col = NCH_GETCOL(square);

    if (col < 6){
        vmap |= NCH_NXTSQR_K_UPLEFT(square);
        vmap |= NCH_NXTSQR_K_DOWNLEFT(square);
        vmap |= NCH_NXTSQR_K_LEFTUP(square);
        vmap |= NCH_NXTSQR_K_LEFTDOWN(square);
    }
    else if (col < 7){
        vmap |= NCH_NXTSQR_K_UPLEFT(square);
        vmap |= NCH_NXTSQR_K_DOWNLEFT(square);    
    }
    
    if (col > 1){
        vmap |= NCH_NXTSQR_K_UPRIGHT(square);
        vmap |= NCH_NXTSQR_K_DOWNRIGHT(square);
        vmap |= NCH_NXTSQR_K_RIGHTUP(square);
        vmap |= NCH_NXTSQR_K_RIGHTDOWN(square);
    }
    else if (col > 0){
        vmap |= NCH_NXTSQR_K_UPRIGHT(square);
        vmap |= NCH_NXTSQR_K_DOWNRIGHT(square);    
    }

    NCH_RMVFLG(vmap, block_map);
    return vmap;
}

cuint64 CBoard_RookVision(cuint64 square, cuint64 block_map){
    cuint64 vmap = 0ull;
    cuint64 current, temp;

    _NCH_VISION_WHILEASSIGN(current, square, NCH_NXTSQR_UP, vmap, block_map)
    _NCH_VISION_WHILEASSIGN(current, square, NCH_NXTSQR_DOWN, vmap, block_map)
    _NCH_VISION_WHILEASSIGN_SIDES(current, square, NCH_NXTSQR_RIGHT, vmap, block_map, NCH_COL1, temp)    
    _NCH_VISION_WHILEASSIGN_SIDES(current, square, NCH_NXTSQR_LEFT, vmap, block_map, NCH_COL8, temp)    

    return vmap;
}

cuint64 CBoard_BishopVision(cuint64 square, cuint64 block_map){
    cuint64 vmap = 0ull;
    cuint64 current, temp;

    _NCH_VISION_WHILEASSIGN_SIDES_BISHOP(current, square, NCH_NXTSQR_UPRIGHT, NCH_NXTSQR_DOWNRIGHT, vmap, block_map, NCH_COL1, temp)
    _NCH_VISION_WHILEASSIGN_SIDES_BISHOP(current, square, NCH_NXTSQR_UPLEFT, NCH_NXTSQR_DOWNLEFT, vmap, block_map, NCH_COL8, temp)

    return vmap;
}

cuint64 CBoard_KingVision(cuint64 square ,cuint64 block_map){
    cuint64 vmap = 0ull;

    if (!NCH_CHKFLG(NCH_COL1, square)){
        vmap |= NCH_NXTSQR_UPRIGHT(square);
        vmap |= NCH_NXTSQR_RIGHT(square);
        vmap |= NCH_NXTSQR_DOWNRIGHT(square);
    }    
    if (!NCH_CHKFLG(NCH_COL8, square)){
        vmap |= NCH_NXTSQR_UPLEFT(square);
        vmap |= NCH_NXTSQR_LEFT(square);
        vmap |= NCH_NXTSQR_DOWNLEFT(square);
    }
    vmap |= NCH_NXTSQR_UP(square);
    vmap |= NCH_NXTSQR_DOWN(square);
    
    NCH_RMVFLG(vmap, block_map);

    return vmap;
}

cuint64 CBoard_QueenVision(cuint64 square, cuint64 block_map){
    return CBoard_RookVision(square, block_map) | CBoard_BishopVision(square, block_map);
}

cuint64 CBoard_PawnAttackVision_White(cuint64 square, cuint64 block_map){
    return (NCH_CHKFLG(NCH_COL1, square) ? 0ull : NCH_NXTSQR_UPRIGHT(square))
        | (NCH_CHKFLG(NCH_COL8, square) ? 0ull : NCH_NXTSQR_UPLEFT(square))
        & ~block_map;
}

cuint64 CBoard_PawnAttackVision_Black(cuint64 square, cuint64 block_map){
    return (NCH_CHKFLG(NCH_COL1, square) ? 0ull : NCH_NXTSQR_DOWNRIGHT(square))
        | (NCH_CHKFLG(NCH_COL8, square) ? 0ull : NCH_NXTSQR_DOWNLEFT(square))
        & ~block_map;
}

int _CBoard_Check_IsCheck(CBoard* board, int turn){
    cuint64 square;
    cuint64 knight_like_map;
    cuint64 rook_like_map;
    cuint64 bishop_like_map;
    cuint64 pawnattack_like_map;

    if (turn == NCH_WHITE){
        square = board->W_King;
        knight_like_map = CBoard_KnightVision(square, 0ull);
        rook_like_map = CBoard_RookVision(square, 0ull);
        bishop_like_map = CBoard_BishopVision(square, 0ull);
        pawnattack_like_map = CBoard_PawnAttackVision_White(square, 0ull);

        if (NCH_CHKUNI(rook_like_map, board->B_Rooks) || NCH_CHKUNI(rook_like_map, board->B_Queens) ||
            NCH_CHKUNI(bishop_like_map, board->B_Bishops) || NCH_CHKUNI(bishop_like_map, board->B_Queens) ||
            NCH_CHKUNI(knight_like_map, board->B_Knights) || NCH_CHKUNI(pawnattack_like_map, board->B_Pawns))
            {
                return 1;
            }

        return 0;
    }
    else{
        square = board->B_King;
        knight_like_map = CBoard_KnightVision(square, 0ull);
        rook_like_map = CBoard_RookVision(square, 0ull);
        bishop_like_map = CBoard_BishopVision(square, 0ull);
        pawnattack_like_map = CBoard_PawnAttackVision_Black(square, 0ull);
    
        if (NCH_CHKUNI(rook_like_map, board->W_Rooks) || NCH_CHKUNI(rook_like_map, board->W_Queens) ||
            NCH_CHKUNI(bishop_like_map, board->W_Bishops) || NCH_CHKUNI(bishop_like_map, board->W_Queens) ||
            NCH_CHKUNI(knight_like_map, board->W_Knights) || NCH_CHKUNI(pawnattack_like_map, board->W_Pawns))
            {
                return 1;
            }

        return 0;    
    }
}

int _CBoard_CaptureIfValid(CBoard* board, int turn, cuint64 capture_sqr, cuint64 op_map){
    if (NCH_CHKFLG(op_map, capture_sqr)){
        cuint64 temp = ~capture_sqr;
        if (turn == NCH_WHITE){
            board->B_Pawns &= temp;
            board->B_Knights &= temp;
            board->B_Bishops &= temp;
            board->B_Rooks &= temp;
            board->B_Queens &= temp;
        }
        else{
            board->W_Pawns &= temp;
            board->W_Knights &= temp;
            board->W_Bishops &= temp;
            board->W_Rooks &= temp;
            board->W_Queens &= temp;
        }
        return 0;
    }

    return -1;
}

int _CBoard_MoveAndCheck(CBoard* board, int turn, cuint64* piece_map, cuint64 square, cuint64 current, cuint64 capture_sqr, cuint64 op_map){
    cuint64 temp_map = *piece_map;

    NCH_MKMOVE(*piece_map, square, current)

    CBoard temp_board = *board;

    _CBoard_CaptureIfValid(&temp_board, turn, capture_sqr, op_map);

    int is_check = _CBoard_Check_IsCheck(&temp_board, turn);

    *piece_map = temp_map;

    return is_check;
}

void _CBoard_PawnPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 all_map, cuint64 op_map){
    cuint64 current;

    if (turn == NCH_WHITE){
        _NCH_POSSIBLEMOVES_PAWN(board, piece_map, turn, idx, square, king_effect_map, NCH_NXTSQR_UP, NCH_NXTSQR_UPRIGHT, NCH_NXTSQR_UPLEFT, NCH_ROW2, NCH_ROW5)
    }
    else{
        _NCH_POSSIBLEMOVES_PAWN(board, piece_map, turn, idx, square, king_effect_map, NCH_NXTSQR_DOWN, NCH_NXTSQR_DOWNRIGHT, NCH_NXTSQR_DOWNLEFT, NCH_ROW7, NCH_ROW4)
    }
}

void _CBoard_KnightPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    cuint64 current;
    int col = NCH_GETCOL(square);
    
    if (col < 6){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_UPLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_DOWNLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_LEFTUP, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_LEFTDOWN, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
    }
    else if (col < 7){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_UPLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_DOWNLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
    }
    
    if (col > 1){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_UPRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_DOWNRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_RIGHTUP, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_RIGHTDOWN, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
    }
    else if (col > 0){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_UPRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_K_DOWNRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map)
    }
}

void _CBoard_RookPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    cuint64 current, temp;

    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, NCH_NXTSQR_UP, square, current, current, ply_map, op_map, ply_map, king_effect_map, piece_map)
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, NCH_NXTSQR_DOWN, square, current, current, ply_map, op_map, ply_map, king_effect_map, piece_map)

    _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES(board, turn, idx, NCH_NXTSQR_RIGHT, square, current, current, NCH_COL1, ply_map, temp, op_map, king_effect_map, piece_map)
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES(board, turn, idx, NCH_NXTSQR_LEFT, square, current, current, NCH_COL8, ply_map, temp, op_map, king_effect_map, piece_map)
}

void _CBoard_BishopPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    cuint64 current, temp;

    _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES_BISHOP(board, turn, idx, NCH_NXTSQR_UPRIGHT, NCH_NXTSQR_DOWNRIGHT, square, current, current, NCH_COL1, ply_map, temp, op_map, king_effect_map, piece_map)
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN_SIDES_BISHOP(board, turn, idx, NCH_NXTSQR_UPLEFT, NCH_NXTSQR_DOWNLEFT, square, current, current, NCH_COL8, ply_map, temp, op_map, king_effect_map, piece_map)
}

void _CBoard_QueenPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    _CBoard_RookPossibleMoves(board, piece_map, turn, idx, square, king_effect_map, ply_map, op_map);
    _CBoard_BishopPossibleMoves(board, piece_map, turn, idx, square, king_effect_map, ply_map, op_map);
}

void _CBoard_KingPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    cuint64 current;

    _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_UP, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
    _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_DOWN, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);

    if (!NCH_CHKFLG(NCH_COL1, square)){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_UPRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_RIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_DOWNRIGHT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
    }

    if (!NCH_CHKFLG(NCH_COL8, square)){
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_UPLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_LEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
        _NCH_POSSIBLEMOVES_MOVE_THEN_ASSIGN(board, turn, idx, NCH_NXTSQR_DOWNLEFT, square, square, current, current, ply_map, op_map, king_effect_map, piece_map);
    }

}

void _CBoard_SetPossibleMoves(CBoard* board, int turn){
    cuint64 ply_map, op_map, king_effect_map;

    if (turn == NCH_WHITE){
        ply_map = board->White_Map;
        op_map = board->Black_Map;
        king_effect_map = CBoard_QueenVision(board->W_King, ply_map | op_map);

        _NCH_SET_POSSIBLEMOVE(_CBoard_PawnPossibleMoves, board, board->W_Pawns, NCH_WHITE, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_RookPossibleMoves, board, board->W_Rooks, NCH_WHITE, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KnightPossibleMoves, board, board->W_Knights, NCH_WHITE, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_BishopPossibleMoves, board, board->W_Bishops, NCH_WHITE, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_QueenPossibleMoves, board, board->W_Queens, NCH_WHITE, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KingPossibleMoves, board, board->W_King, NCH_WHITE, king_effect_map, ply_map, op_map)
    }
    else{
        ply_map = board->Black_Map;
        op_map = board->White_Map;
        king_effect_map = CBoard_QueenVision(board->B_King, ply_map | op_map);

        _NCH_SET_POSSIBLEMOVE(_CBoard_PawnPossibleMoves, board, board->B_Pawns, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_RookPossibleMoves, board, board->B_Rooks, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KnightPossibleMoves, board, board->B_Knights, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_BishopPossibleMoves, board, board->B_Bishops, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_QueenPossibleMoves, board, board->B_Queens, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KingPossibleMoves, board, board->B_King, NCH_BLACK, king_effect_map, ply_map, op_map)
    }
}

void _CBoard_Update(CBoard* board, int turn){
    _CBoard_CLEAN_POSSIBLEMOVES(board);

    board->White_Map = NCH_B_GET_WHITEMAP(board);
    board->Black_Map = NCH_B_GET_BLACKMAP(board);

    _CBoard_SetPossibleMoves(board, turn);
}

void _CBoard_CheckPawnMoves(CBoard* board, int turn, cuint64 from_, cuint64 to_, cuint64 op_map, int promotion_piece){
    if (turn == NCH_WHITE){
        if (NCH_CHKFLG(board->W_Pawns, from_)){
            NCH_SETFLG(board->flags, NCH_B_PAWNMOVED);

            int from_col = NCH_GETCOL(from_);
            int to_col = NCH_GETCOL(to_);
            if (from_col == to_col){
                if (to_col == 7){
                    NCH_RMVFLG(board->W_Pawns, to_);
                    if (promotion_piece == NCH_PROMOTION_TO_QUEEN){
                        NCH_SETFLG(board->W_Queens, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_ROOK){
                        NCH_SETFLG(board->W_Rooks, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_KNIGHT){
                        NCH_SETFLG(board->W_Knights, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_BISHOP){
                        NCH_SETFLG(board->W_Bishops, to_);
                    }else{
                        NCH_SETFLG(board->W_Queens, to_);
                    }
                }
                else if (NCH_NXTSQR_UP(from_) != to_){
                    NCH_SETFLG(board->flags, NCH_B_PAWNMOVED2SQR);
                    _NCH_B_SET_PAWNCOL(board, to_col);
                }
            }
            else{
                if (!NCH_CHKFLG(op_map, to_)){
                    NCH_SETFLG(board->flags, NCH_B_ENPASSANT);
                }
            }
        }
    }
    else{
        if (NCH_CHKFLG(board->B_Pawns, from_)){
            NCH_SETFLG(board->flags, NCH_B_PAWNMOVED);

            int from_col = NCH_GETCOL(from_);
            int to_col = NCH_GETCOL(to_);
            if (from_col == to_col){
                if (to_col == 0){
                    NCH_RMVFLG(board->B_Pawns, to_);
                    if (promotion_piece == NCH_PROMOTION_TO_QUEEN){
                        NCH_SETFLG(board->B_Queens, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_ROOK){
                        NCH_SETFLG(board->B_Rooks, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_KNIGHT){
                        NCH_SETFLG(board->B_Knights, to_);
                    }else if (promotion_piece == NCH_PROMOTION_TO_BISHOP){
                        NCH_SETFLG(board->B_Bishops, to_);
                    }else{
                        NCH_SETFLG(board->B_Queens, to_);
                    }
                }
                else if (NCH_NXTSQR_DOWN(from_) != to_){
                    NCH_SETFLG(board->flags, NCH_B_PAWNMOVED2SQR);
                    _NCH_B_SET_PAWNCOL(board, to_col);
                }
            }
            else{
                if (!NCH_CHKFLG(op_map, to_)){
                    NCH_SETFLG(board->flags, NCH_B_ENPASSANT);
                }
            }
        }
    }
}

int CBoard_Step(CBoard* board, cuint64* piece_map, cuint64 from_, cuint64 to_, int promotion_piece){
    int idx = NCH_SQRIDX(from_);
    int turn = NCH_B_TURN(board);

    if (!NCH_CHKFLG(board->possible_moves[idx], to_)){
        return -1;
    }

    cuint64 op_map = turn == NCH_WHITE ? board->Black_Map : board->White_Map;

    NCH_MKMOVE(*piece_map, from_, to_)
    int is_capture = _CBoard_CaptureIfValid(board, turn, to_, op_map);

    if (is_capture == 1){
        NCH_SETFLG(board->flags, NCH_B_CAPTURE);
    }

    _CBoard_CheckPawnMoves(board, turn, from_, to_, op_map, promotion_piece);

    if (turn == NCH_WHITE){
        NCH_B_SET_BLACKTURN(board);
        turn = NCH_BLACK;
    }
    else{
        NCH_B_SET_WHITETURN(board);
        turn = NCH_WHITE;    
    }

    _CBoard_Update(board, turn);

    return 0;
}

CBoard* CBoard_New(){
    CBoard* board = malloc(sizeof(CBoard));
    if (!board){
        return NULL;
    }

    board->W_Pawns = NCH_WHITE_PAWNS_START_POS;
    board->B_Pawns = NCH_BLACK_PAWNS_START_POS;

    board->W_Knights = NCH_WHITE_KNIGHTS_START_POS;
    board->B_Knights = NCH_BLACK_KNIGHTS_START_POS;
    
    board->W_Bishops = NCH_WHITE_BISHOPS_START_POS;
    board->B_Bishops = NCH_BLACK_BISHOPS_START_POS;

    board->W_Rooks = NCH_WHITE_ROOKS_START_POS;
    board->B_Rooks = NCH_BLACK_ROOKS_START_POS;
    
    board->W_Queens = NCH_WHITE_QUEEN_START_POS;
    board->B_Queens = NCH_BLACK_QUEEN_START_POS;
    
    board->W_King = NCH_WHITE_KING_START_POS;
    board->B_King = NCH_BLACK_KING_START_POS;

    board->flags = 0;
    board->count = 0;

    NCH_B_SET_WHITETURN(board);

    _CBoard_Update(board, NCH_WHITE);

    return board;
}

#endif