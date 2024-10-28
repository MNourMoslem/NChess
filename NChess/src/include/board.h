#ifndef NCHESS_BOARD_H
#define NCHESS_BOARD_H

#include "assign.h"
#include "types.h"
#include "core.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int NCH_GAMEDICT_SIZE = 100;

#define _CBoard_CLEAN_POSSIBLEMOVES(board) memset(board->possible_moves, 0, sizeof(board->possible_moves))

void CBoard_PrintGameState(const CBoard* board){
    printf("Game State: ");
    if (NCH_B_IS_GAMEEND(board)){
        printf("End, Game Result: ");
        if (NCH_B_IS_DRAW(board)){
            printf("Draw by ");
            if (NCH_B_IS_STALEMATE(board)){
                printf("Stalemate");
            }
            else if (NCH_B_IS_THREEFOLD(board)){
                printf("Threefold Repetion");
            }
            else if (NCH_B_IS_FIFTYMOVES(board)){
                printf("Fifty Moves Rule");
            }
        }
        else{
            if (NCH_B_IS_WHITEWIN(board)){
                printf("White ");
            }
            else{
                printf("Black ");
            }
            printf("Winning!");
        }
    }
    else{
        printf("Continuing, ");
        if (NCH_B_IS_WHITETURN(board)){
            printf("White ");
        }
        else{
            printf("Black ");
        }
        printf("Move");
    }
    printf("\n");
}

cuint64 _CBoard_ToKey(const CBoard *board) {
    cuint64 key = 0;

    key ^= board->W_Pawns;
    key ^= board->B_Pawns;
    key ^= board->W_Knights;
    key ^= board->B_Knights;
    key ^= board->W_Bishops;
    key ^= board->B_Bishops;
    key ^= board->W_Rooks;
    key ^= board->B_Rooks;
    key ^= board->W_Queens;
    key ^= board->B_Queens;
    key ^= board->W_King;
    key ^= board->B_King;

    return key;
}

void _CBoard_PiecesMovesAsString(CBoard* board, cuint64* piece_map, int* move_idx, char moves_str[][8], char piece_char, cuint64 op_map){
    int N = NCH_POPCOUNTLL(*piece_map);
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
            moves_str[idx][current++] = 'h' - (indices[i] % 8);
            moves_str[idx][current++] = (indices[i] / 8) + '1';

            if (NCH_CHKFLG(op_map, move.square)){
                moves_str[idx][current++] = 'x';
            }else{
                moves_str[idx][current++] = '-';
            }

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
    int N = NCH_POPCOUNTLL(*piece_map);
    int* indices = malloc(N * sizeof(int));
    int* columns = malloc(N * sizeof(int));
    cuint64* moves = malloc(N * sizeof(cuint64));
    int j = 0;

    _NCH_MAP_LOOP(*piece_map){
        indices[j] = move.idx;
        columns[j] = move.idx % 8;
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

            if (NCH_GETCOL(move.square) != columns[i]){
                moves_str[idx][current++] = 'x';
            }else{
                moves_str[idx][current++] = '-';
            }

            moves_str[idx][current++] = 'h' - (move.idx % 8);
            moves_str[idx][current++] = (move.idx / 8) + '1';

            if (NCH_CHKFLG(last_row, move.square)){
                for (int i = 1, last_idx = idx, last_current = current; i < 5; i++, last_current = current){
                    memcpy(moves_str[idx] ,moves_str[last_idx], sizeof(char) * current);

                    moves_str[idx][last_current++] = '=';
                    moves_str[idx][last_current++] = NCH_PIECES[i];
                    moves_str[idx][last_current++] = '\0';
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

void _CBoard_CastleMovesAsString(CBoard* board, int *move_idx, char moves_str[][8]){
    int idx = *move_idx;    
    if (NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OO)){
        moves_str[idx][0] = 'O';
        moves_str[idx][1] = '-';
        moves_str[idx][2] = 'O';
        moves_str[idx++][3] = '\0';
    }
    if (NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OOO)){
        moves_str[idx][0] = 'O';
        moves_str[idx][1] = '-';
        moves_str[idx][2] = 'O';
        moves_str[idx][3] = '-';
        moves_str[idx][4] = 'O';
        moves_str[idx++][5] = '\0';
    }
    *move_idx = idx;
}

int CBoard_PossibleMovesAsString(CBoard* board, char moves_str[][8]){
    int move_idx = 0;
    int piece_char_idx = 0;

    if (NCH_B_IS_WHITETURN(board)){
        _CBoard_PiecesMovesAsString_Pawn(board, &board->W_Pawns, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map, NCH_ROW8);
        _CBoard_PiecesMovesAsString(board, &board->W_Knights, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
        _CBoard_PiecesMovesAsString(board, &board->W_Bishops, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
        _CBoard_PiecesMovesAsString(board, &board->W_Rooks, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
        _CBoard_PiecesMovesAsString(board, &board->W_Queens, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
        _CBoard_PiecesMovesAsString(board, &board->W_King, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->Black_Map);
    }
    else{
        _CBoard_PiecesMovesAsString_Pawn(board, &board->B_Pawns, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map, NCH_ROW1);
        _CBoard_PiecesMovesAsString(board, &board->B_Knights, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
        _CBoard_PiecesMovesAsString(board, &board->B_Bishops, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
        _CBoard_PiecesMovesAsString(board, &board->B_Rooks, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
        _CBoard_PiecesMovesAsString(board, &board->B_Queens, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
        _CBoard_PiecesMovesAsString(board, &board->B_King, &move_idx, moves_str, NCH_PIECES[piece_char_idx++], board->White_Map);
    }

    _CBoard_CastleMovesAsString(board, &move_idx, moves_str);

    return move_idx;
}

int CBoard_NumberPossibleCastling(CBoard* board, int turn){
    int count = 0;
    if (turn == NCH_WHITE){
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_WHITE_OO)){
            count++;
        }
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_WHITE_OOO)){
            count++;
        }
    }
    else{
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_BLACK_OO)){
            count++;
        }
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_BLACK_OOO)){
            count++;
        }
    }
    return count;
}

int CBoard_NumberPossibleMoves(CBoard* board){
    int count = 0;
    for (int i = 0; i < 64; i++){
        count += NCH_POPCOUNTLL(board->possible_moves[i]);
    }

    if (NCH_B_IS_WHITETURN(board)){
        _NCH_MAP_LOOP(board->W_Pawns){
            if (NCH_CHKUNI(NCH_ROW8 ,board->possible_moves[move.idx])){
                count += NCH_POPCOUNTLL(board->possible_moves[move.idx]) * 3;
            }
        }

        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OO)){
            count += 1;
        }
        
        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OOO)){
            count += 1;
        }
    }
    else{
        _NCH_MAP_LOOP(board->B_Pawns){
            if (NCH_CHKUNI(NCH_ROW1 ,board->possible_moves[move.idx])){
                count += NCH_POPCOUNTLL(board->possible_moves[move.idx]) * 4;
            }
        }

        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OO)){
            count += 1;
        }
        
        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OOO)){
            count += 1;
        }
    }

    return count;
}

int CBoard_HasNoPossibleMove(CBoard* board) {
    return (board->possible_moves[0] == 0ull) &&
           (memcmp(board->possible_moves, board->possible_moves + 1, 63 * sizeof(uint64_t)) == 0);
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

cuint64 CBoard_PawnAttackVision_White(cuint64 square){
    return (NCH_CHKFLG(NCH_COL1, square) ? 0ull : NCH_NXTSQR_UPRIGHT(square))
        | (NCH_CHKFLG(NCH_COL8, square) ? 0ull : NCH_NXTSQR_UPLEFT(square));
}

cuint64 CBoard_PawnAttackVision_Black(cuint64 square){
    return (NCH_CHKFLG(NCH_COL1, square) ? 0ull : NCH_NXTSQR_DOWNRIGHT(square))
        | (NCH_CHKFLG(NCH_COL8, square) ? 0ull : NCH_NXTSQR_DOWNLEFT(square));
}

int _CBoard_CheckSquare_IsCheck(CBoard* board, cuint64 square, int turn){
    cuint64 knight_like_map = CBoard_KnightVision(square, board->All_Map);
    cuint64 rook_like_map = CBoard_RookVision(square, board->All_Map);
    cuint64 bishop_like_map = CBoard_BishopVision(square, board->All_Map);
    cuint64 pawnattack_like_map;

    if (turn == NCH_WHITE){
        pawnattack_like_map = CBoard_PawnAttackVision_White(square);
        if (NCH_CHKUNI(rook_like_map, board->B_Rooks) || NCH_CHKUNI(rook_like_map, board->B_Queens) ||
            NCH_CHKUNI(bishop_like_map, board->B_Bishops) || NCH_CHKUNI(bishop_like_map, board->B_Queens) ||
            NCH_CHKUNI(knight_like_map, board->B_Knights) || NCH_CHKUNI(pawnattack_like_map, board->B_Pawns) ||
            NCH_CHKUNI(rook_like_map, board->B_King) || NCH_CHKUNI(bishop_like_map, board->B_King))
            {
                return 1;
            }
        return 0;
    }
    else{
        pawnattack_like_map = CBoard_PawnAttackVision_Black(square);
        if (NCH_CHKUNI(rook_like_map, board->W_Rooks) || NCH_CHKUNI(rook_like_map, board->W_Queens) ||
            NCH_CHKUNI(bishop_like_map, board->W_Bishops) || NCH_CHKUNI(bishop_like_map, board->W_Queens) ||
            NCH_CHKUNI(knight_like_map, board->W_Knights) || NCH_CHKUNI(pawnattack_like_map, board->W_Pawns) ||
            NCH_CHKUNI(rook_like_map, board->W_King) || NCH_CHKUNI(bishop_like_map, board->W_King))
            {
                return 1;
            }
        return 0;    
    }
}

int _CBoard_Check_IsCheck(CBoard* board, int turn){
    cuint64 king_square = turn == NCH_WHITE ? board->W_King : board->B_King;
    return _CBoard_CheckSquare_IsCheck(board, king_square, turn);
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

    temp_board.White_Map = NCH_B_GET_WHITEMAP(board);
    temp_board.Black_Map = NCH_B_GET_BLACKMAP(board);
    temp_board.All_Map = temp_board.White_Map | temp_board.Black_Map;

    _CBoard_CaptureIfValid(&temp_board, turn, capture_sqr, op_map);

    int is_check = _CBoard_Check_IsCheck(&temp_board, turn);

    *piece_map = temp_map;

    return is_check;
}

void _CBoard_PawnPossibleMoves(CBoard* board, cuint64* piece_map, int turn, int idx, cuint64 square, cuint64 king_effect_map, cuint64 ply_map, cuint64 op_map){
    cuint64 current;

    if (turn == NCH_WHITE){
        _NCH_POSSIBLEMOVES_PAWN(board, turn, idx, square, piece_map, ply_map, op_map, king_effect_map, NCH_NXTSQR_UP, NCH_NXTSQR_UPRIGHT, NCH_NXTSQR_UPLEFT, NCH_ROW2, NCH_ROW5)
    }
    else{
        _NCH_POSSIBLEMOVES_PAWN(board, turn, idx, square, piece_map, ply_map, op_map, king_effect_map, NCH_NXTSQR_DOWN, NCH_NXTSQR_DOWNRIGHT, NCH_NXTSQR_DOWNLEFT, NCH_ROW7, NCH_ROW4)
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

    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, NCH_NXTSQR_UP, square, current, current, ply_map, op_map, board->All_Map, king_effect_map, piece_map)
    _NCH_POSSIBLEMOVES_WHILE_ASSIGN(board, turn, idx, NCH_NXTSQR_DOWN, square, current, current, ply_map, op_map, board->All_Map, king_effect_map, piece_map)

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

void _CBoard_CheckCastle(CBoard* board, int turn){
    if (NCH_B_IS_CHECK(board)){
        return;
    }

    if (turn == NCH_WHITE){
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_WHITE_OO) && !NCH_CHKUNI(board->All_Map, 6ull)
            && _CBoard_CheckSquare_IsCheck(board, NCH_F1, turn) != 1 && _CBoard_CheckSquare_IsCheck(board, NCH_G1, turn) != 1){
            NCH_SETFLG(board->castle_flags, NCH_CF_COULD_WHITE_OO);
        }
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_WHITE_OOO) && !NCH_CHKUNI(board->All_Map, 112ull)
            && _CBoard_CheckSquare_IsCheck(board, NCH_D1, turn) != 1 && _CBoard_CheckSquare_IsCheck(board, NCH_C1, turn) != 1){            
            NCH_SETFLG(board->castle_flags, NCH_CF_COULD_WHITE_OOO);
        }
    }
    else{
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_BLACK_OO) && !NCH_CHKUNI(board->All_Map, 0x0600000000000000ull)
            && _CBoard_CheckSquare_IsCheck(board, NCH_F8, turn) != 1 && _CBoard_CheckSquare_IsCheck(board, NCH_G8, turn) != 1){                
            NCH_SETFLG(board->castle_flags, NCH_CF_COULD_BLACK_OO);
        }
        if (!NCH_CHKFLG(board->castle_flags, NCH_CF_BLACK_OOO) && !NCH_CHKUNI(board->All_Map, 0x7000000000000000ull)
            && _CBoard_CheckSquare_IsCheck(board, NCH_D8, turn) != 1 && _CBoard_CheckSquare_IsCheck(board, NCH_C8, turn) != 1){            
            NCH_SETFLG(board->castle_flags, NCH_CF_COULD_BLACK_OOO);
        }
    }
}

void _CBoard_SetPossibleMoves(CBoard* board, int turn, int is_check){
    cuint64 ply_map, op_map, king_effect_map;

    if (turn == NCH_WHITE){
        ply_map = board->White_Map;
        op_map = board->Black_Map;
        king_effect_map = is_check != 1 ? CBoard_QueenVision(board->W_King, board->All_Map) | board->W_King : NCH_CUINT64_MAX;

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
        king_effect_map = is_check != 1 ? CBoard_QueenVision(board->B_King, ply_map | op_map) | board->B_King: NCH_CUINT64_MAX;

        _NCH_SET_POSSIBLEMOVE(_CBoard_PawnPossibleMoves, board, board->B_Pawns, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_RookPossibleMoves, board, board->B_Rooks, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KnightPossibleMoves, board, board->B_Knights, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_BishopPossibleMoves, board, board->B_Bishops, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_QueenPossibleMoves, board, board->B_Queens, NCH_BLACK, king_effect_map, ply_map, op_map)
        _NCH_SET_POSSIBLEMOVE(_CBoard_KingPossibleMoves, board, board->B_King, NCH_BLACK, king_effect_map, ply_map, op_map)
    }

    _CBoard_CheckCastle(board, turn);
}

int _CBoard_Check_FiftyMoves(CBoard* board){
    if (NCH_CHKUNI(board->flags, NCH_B_MASK_GAMEACTIONS)){
        board->fifty_count = 0;
    }
    else{
        board->fifty_count += 1;
        if (board->fifty_count >= 50){
            NCH_SETFLG(board->flags, NCH_B_FIFTYMOVES);
            NCH_SETFLG(board->flags, NCH_B_DRAW);
            NCH_SETFLG(board->flags, NCH_B_GAMEEND);
            return 1;
        }
    }
    return 0;
}

int _CBoard_Check_CheckMateAndStaleMate(CBoard* board, int is_check){
    if (CBoard_HasNoPossibleMove(board)){
        if (is_check == 1){
            NCH_SETFLG(board->flags, NCH_B_CHECKMATE);
            if (!NCH_B_IS_WHITETURN(board)){
                NCH_SETFLG(board->flags, NCH_B_WHITEWIN);
            }
            else{
                // if the flag is off black is considered the winner.
            }
        }
        else{
            NCH_SETFLG(board->flags, NCH_B_STALEMATE);
            NCH_SETFLG(board->flags, NCH_B_DRAW);
        }
        NCH_SETFLG(board->flags, NCH_B_GAMEEND);
        return 1;
    }
    return 0;
}

void _CBoard_Check_CastleAfterUpdate(CBoard* board, int turn){
    if (turn == NCH_WHITE){
        if (!NCH_CHKFLG(board->Black_Map, 0x0900000000000000)){
            NCH_SETFLG(board->castle_flags, NCH_CF_BLACK_OO);
        }
        if (!NCH_CHKFLG(board->Black_Map, 0x8800000000000000)){
            NCH_SETFLG(board->castle_flags, NCH_CF_BLACK_OOO);
        }
    }
    else{
        if (!NCH_CHKFLG(board->Black_Map, 0x0000000000000009)){
            NCH_SETFLG(board->castle_flags, NCH_CF_WHITE_OO);
        }
        if (!NCH_CHKFLG(board->Black_Map, 0x8800000000000088)){
            NCH_SETFLG(board->castle_flags, NCH_CF_WHITE_OOO);
        }
    }
}

void _CBoard_Update(CBoard* board, int turn){
    _CBoard_CLEAN_POSSIBLEMOVES(board);
    NCH_CF_RESET_COULDCASLTE(board);

    board->White_Map = NCH_B_GET_WHITEMAP(board);
    board->Black_Map = NCH_B_GET_BLACKMAP(board);
    board->All_Map = board->Black_Map | board->White_Map;

    int three = _NCH_Ht_AddValueToItem(board->GameDict, _CBoard_ToKey(board), 1);
    if (three >= 3){
        NCH_SETFLG(board->flags, NCH_B_THREEFOLD);
        NCH_SETFLG(board->flags, NCH_B_DRAW);
        NCH_SETFLG(board->flags, NCH_B_GAMEEND);
        return;  
    }    

    int is_check = _CBoard_Check_IsCheck(board, turn);
    if (is_check == 1){
        NCH_SETFLG(board->flags, NCH_B_CHECK);
    }

    if (_CBoard_Check_FiftyMoves(board) != 0){
        return;
    }

    _CBoard_SetPossibleMoves(board, turn, is_check);

    _CBoard_Check_CheckMateAndStaleMate(board, is_check);
}

void _CBoard_CheckPawnMoves(CBoard* board, int turn, cuint64 from_, cuint64 to_, cuint64 op_map, NCH_SMoves special_move){
    if (turn == NCH_WHITE){
        if (NCH_CHKFLG(board->W_Pawns, from_)){
            NCH_SETFLG(board->flags, NCH_B_PAWNMOVED);

            int from_col = NCH_GETCOL(from_);
            int to_col = NCH_GETCOL(to_);
            if (from_col == to_col){
                if (to_col == 7){
                    NCH_RMVFLG(board->W_Pawns, to_);
                    if (special_move == NCH_Promote2Queen){
                        NCH_SETFLG(board->W_Queens, to_);
                    }else if (special_move == NCH_Promote2Rook){
                        NCH_SETFLG(board->W_Rooks, to_);
                    }else if (special_move == NCH_Promote2Knight){
                        NCH_SETFLG(board->W_Knights, to_);
                    }else if (special_move == NCH_Promote2Knight){
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
                    if (special_move == NCH_Promote2Queen){
                        NCH_SETFLG(board->B_Queens, to_);
                    }else if (special_move == NCH_Promote2Rook){
                        NCH_SETFLG(board->B_Rooks, to_);
                    }else if (special_move == NCH_Promote2Knight){
                        NCH_SETFLG(board->B_Knights, to_);
                    }else if (special_move == NCH_Promote2Knight){
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

int _CBoard_StepCastle(CBoard* board, int turn, NCH_SMoves special_move){
    if (special_move != NCH_OO && special_move != NCH_OOO){
        return -1;
    }

    if (turn == NCH_WHITE){
        if (special_move == NCH_OO && NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OO)){
            NCH_MKMOVE(board->W_Rooks, NCH_H1, NCH_F1)
            NCH_MKMOVE(board->W_King, NCH_E1, NCH_G1)
        }
        else if (special_move == NCH_OOO && NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OOO)){
            NCH_MKMOVE(board->W_Rooks, NCH_A1, NCH_D1)
            NCH_MKMOVE(board->W_King, NCH_E1, NCH_C1)
        }
        else{
            return -1;
        }
    }else{
        if (special_move == NCH_OO && NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OO)){
            NCH_MKMOVE(board->B_Rooks, NCH_H8, NCH_F8)
            NCH_MKMOVE(board->B_King, NCH_E8, NCH_G8)
        }
        else if (special_move == NCH_OOO && NCH_CHKUNI(board->castle_flags, NCH_CF_COULD_OOO)){
            NCH_MKMOVE(board->B_Rooks, NCH_A8, NCH_D8)
            NCH_MKMOVE(board->B_King, NCH_E8, NCH_C8)
        }
        else{
            return -1;
        }
    }

    return 0;
}

int _CBoard_Step(CBoard* board, int turn, cuint64* piece_map, cuint64 from_, cuint64 to_, NCH_SMoves special_move){
    NCH_B_RESET_GAMEACTIONS(board);

    if (from_ == 0ull || to_ == 0ull){
        if (_CBoard_StepCastle(board, turn, special_move) == -1){
            return -1;
        }
    }
    else{
        if (!NCH_CHKFLG(board->possible_moves[NCH_SQRIDX(from_)], to_)){
            return -1;
        }

        cuint64 op_map = turn == NCH_WHITE ? board->Black_Map : board->White_Map;
        NCH_MKMOVE(*piece_map, from_, to_)

        if (_CBoard_CaptureIfValid(board, turn, to_, op_map) == 1){
            NCH_SETFLG(board->flags, NCH_B_CAPTURE);
        }
        _CBoard_CheckPawnMoves(board, turn, from_, to_, op_map, special_move);
    }

    if (turn == NCH_WHITE){
        NCH_B_SET_BLACKTURN(board);
        turn = NCH_BLACK;
    }
    else{
        NCH_B_SET_WHITETURN(board);
        turn = NCH_WHITE;    
    }

    board->move_count += 1;

    _CBoard_Update(board, turn);

    return 0;
}

int CBoard_Step(CBoard* board, cuint64 from_, cuint64 to_, NCH_SMoves special_move){
    int turn = NCH_B_TURN(board);
    cuint64* piece_map;

    if (from_ == 0ull || to_ == 0ull){
        return _CBoard_Step(board, turn, 0ull, 0ull, 0ull, special_move); 
    }

    if (turn == NCH_WHITE){
        if (NCH_CHKFLG(board->W_Pawns, from_)){
            piece_map = &board->W_Pawns;
        }else if (NCH_CHKFLG(board->W_Knights, from_)){
            piece_map = &board->W_Knights;
        }else if (NCH_CHKFLG(board->W_Bishops, from_)){
            piece_map = &board->W_Bishops;
        }else if (NCH_CHKFLG(board->W_Rooks, from_)){
            piece_map = &board->W_Rooks;
        }else if (NCH_CHKFLG(board->W_Queens, from_)){
            piece_map = &board->W_Queens;
        }else if (NCH_CHKFLG(board->W_King, from_)){
            piece_map = &board->W_King;
        }else{
            return -1;
        }
    }
    else{
        if (NCH_CHKFLG(board->B_Pawns, from_)){
            piece_map = &board->B_Pawns;
        }else if (NCH_CHKFLG(board->B_Knights, from_)){
            piece_map = &board->B_Knights;
        }else if (NCH_CHKFLG(board->B_Bishops, from_)){
            piece_map = &board->B_Bishops;
        }else if (NCH_CHKFLG(board->B_Rooks, from_)){
            piece_map = &board->B_Rooks;
        }else if (NCH_CHKFLG(board->B_Queens, from_)){
            piece_map = &board->B_Queens;
        }else if (NCH_CHKFLG(board->B_King, from_)){
            piece_map = &board->B_King;
        }else{
            return -1;
        }
    }

    return _CBoard_Step(board, turn, piece_map, from_, to_, special_move);
}

int CBoard_StepAllPossibleMoves(CBoard* board, CBoard result_boards[]){
    int turn = NCH_B_TURN(board);
    int idx;
    int current_idx = 0;
    NCH_SMoves promotions[4] = {NCH_Promote2Queen, NCH_Promote2Rook, NCH_Promote2Knight, NCH_Promote2Bishop};

    cuint64 last_row, from_;
    cuint64* possible_moves = board->possible_moves;
    CBoard copy_board;

    if (turn == NCH_WHITE) {
        _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(board, possible_moves, turn, copy_board, W_Pawns, current_idx, NCH_ROW8, promotions)
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, W_Knights, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, W_Bishops, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, W_Queens, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, W_Rooks, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, W_King, current_idx)    
    } else {
        _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(board, possible_moves, turn, copy_board, B_Pawns, current_idx, NCH_ROW1, promotions)
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, B_Knights, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, B_Bishops, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, B_Queens, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, B_Rooks, current_idx)    
        _NCH_STEP_ALL_POSSIBLE_MOVES(board, possible_moves, turn, copy_board, B_King, current_idx)    
    }

    return current_idx;
}

int CBoard_CountAllPossibleMoves(CBoard* board, int depth){
    if (depth <= 1){
        return CBoard_NumberPossibleMoves(board);
    }

    int count = 0;    
    CBoard boards[250];
    int n_moves = CBoard_StepAllPossibleMoves(board, boards);
    for (int i = 0; i < n_moves; i++){
        count += CBoard_CountAllPossibleMoves(boards + i, depth - 1);
    }
    return count;
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

    board->GameDict = _NCH_Ht_New(NCH_GAMEDICT_SIZE);
    if (!board->GameDict){
        free(board);
        return NULL;
    }

    board->flags = 0;
    board->castle_flags = 0;
    board->fifty_count = 0;
    board->move_count = 0;

    NCH_B_SET_WHITETURN(board);

    _CBoard_Update(board, NCH_WHITE);

    return board;
}

void CBoard_Free(CBoard* board){
    if (board){
        _NCH_Ht_Free(board->GameDict);
        free(board);
    }
}

void _CBoard_FEN_Object2Map(CBoard* board, char* row_str, int row_len, int row){
    char current;
    cuint64 sqr, *piece_map;
    int idx, col = 7;
    for (int i = 0; i < row_len; i++){
        current = row_str[i];

        if (current < '9'){
            col -= current - '0';
            continue;
        }

        idx = row * 8 + col;
        sqr = NCH_SQR(idx); 

        switch (current)
        {
        case 'P':
            piece_map = &board->W_Pawns;
            break;
        
        case 'N':
            piece_map = &board->W_Knights;
            break;

        case 'B':
            piece_map = &board->W_Bishops;
            break;

        case 'R':
            piece_map = &board->W_Rooks;
            break;

        case 'Q':
            piece_map = &board->W_Queens;
            break;

        case 'K':
            piece_map = &board->W_King;
            break;

        case 'p':
            piece_map = &board->B_Pawns;
            break;
        
        case 'n':
            piece_map = &board->B_Knights;
            break;

        case 'b':
            piece_map = &board->B_Bishops;
            break;

        case 'r':
            piece_map = &board->B_Rooks;
            break;

        case 'q':
            piece_map = &board->B_Queens;
            break;

        case 'k':
            piece_map = &board->B_King;
            break;

        default:
            piece_map = NULL;
            break;
        }

        NCH_SETFLG(*piece_map, sqr);
        col--;
    }
}

int _CBoard_FEN_Board(CBoard* board, char* board_str){
    int i = 0;
    char current = board_str[i];
    int row = 7;
    int row_len = 0;
    int start = 0;

    while (current != ' ' && row > -1)
    {
        if (current == '/'){
            _CBoard_FEN_Object2Map(board, board_str + start, row_len, row);
            row--;
            start += row_len + 1;
            row_len = 0;
        }
        else{
            row_len++;
        }
        i++;
        current = board_str[i];
    }
    _CBoard_FEN_Object2Map(board, board_str + start, row_len, row);

    return i;
}

int _CBoard_FEN_Turn(CBoard* board, char turn_str){
    if (turn_str == 'w'){
        NCH_B_SET_WHITETURN(board);
    }
    else{
        NCH_B_SET_BLACKTURN(board);
    }
    return 1;
}

int _CBoard_FEN_CastleFlags(CBoard* board, char* flags_str){
    char current;

    for (int i = 0; i < 4; i++)
    {
        current = flags_str[i];

        switch (current)
        {
        case 'K':
            NCH_RMVFLG(board->castle_flags, NCH_CF_WHITE_OO);
            break;

        case 'Q':
            NCH_RMVFLG(board->castle_flags, NCH_CF_WHITE_OOO);
            break;

        case 'k':
            NCH_RMVFLG(board->castle_flags, NCH_CF_BLACK_OO);
            break;

        case 'q':
            NCH_RMVFLG(board->castle_flags, NCH_CF_BLACK_OOO);
            break;
        
        case '-':
            return 1;
            break;

        default:
            return i;
            break;
        }
    }

    return 4;
}

int _CBoard_FEN_EnPassant(CBoard* board, char* enp_str){
    if (enp_str[0] == '-'){
        return 1;
    }

    int col = ('h' - enp_str[0]);

    if (col < 8){
        NCH_SETFLG(board->flags, NCH_B_PAWNMOVED);
        NCH_SETFLG(board->flags, NCH_B_PAWNMOVED2SQR);
        _NCH_B_SET_PAWNCOL(board, col);
        return 2;
    }else{
        return 1;
    }
}

int _CBoard_FEN_FiftyMoves(CBoard* board, char* fif_str){
    int idx = 0;
    char current = fif_str[idx];

    while (current != ' ')
    {
        idx++;
        current = fif_str[idx];
    }

    for (int i = idx - 1, j = 1; i > -1; i--, j *= 10){
        board->fifty_count += (j * (fif_str[i] - '0'));
    }
    board->fifty_count--;

    return idx;
}

int _CBoard_FEN_MovesNumber(CBoard* board, char* moves_str){
    int idx = 0;
    char current = moves_str[idx];

    while (current != '\0')
    {
        idx++;
        current = moves_str[idx];
    }

    for (int i = idx - 1, j = 1; i > -1; i--, j *= 10){
        board->move_count += (j * (moves_str[i] - '0'));
    }
    board->move_count;

    return idx;
}

CBoard* CBoard_FromFEN(char* FEN){
    CBoard* board = malloc(sizeof(CBoard));
    if (!board){
        return NULL;
    }

    board->GameDict = _NCH_Ht_New(NCH_GAMEDICT_SIZE);
    if (!board->GameDict){
        free(board);
    }

    board->W_Pawns = 0ull;
    board->W_Knights = 0ull;
    board->W_Bishops = 0ull;
    board->W_Rooks = 0ull;
    board->W_Queens = 0ull;
    board->W_King = 0ull;
    board->B_Pawns = 0ull;
    board->B_Knights = 0ull;
    board->B_Bishops = 0ull;
    board->B_Rooks = 0ull;
    board->B_Queens = 0ull;
    board->B_King = 0ull;

    board->flags = 0;
    board->fifty_count = 0;
    board->castle_flags = NCH_CUINT8_MAX;
    board->move_count = 0;

    int i = 0;
    i += _CBoard_FEN_Board(board, FEN) + 1;
    i += _CBoard_FEN_Turn(board, FEN[i]) + 1;
    i += _CBoard_FEN_CastleFlags(board, FEN + i) + 1;
    i += _CBoard_FEN_EnPassant(board, FEN + i);

    if (FEN[i++] != '\0'){
        i += _CBoard_FEN_FiftyMoves(board, FEN + i);
    }

    if (FEN[i++] != '\0'){
        _CBoard_FEN_MovesNumber(board, FEN + i);
    }

    _CBoard_Update(board, NCH_B_TURN(board));

    return board;
}

#endif