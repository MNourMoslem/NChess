#ifndef NCHESS_BOARD_H
#define NCHESS_BOARD_H

#include "assign.h"
#include "types.h"
#include "core.h"
#include "scan.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int NCH_GAMEDICT_SIZE = 100;

#define _CBoard_CLEAN_POSSIBLEMOVES(board) memset(board->possible_moves, 0, sizeof(board->possible_moves))

#define _NCH_WHITE_KING_ATTACKERS(board, sqr, idx)  ((NCH_SCAN_ROOK_LIKE(board->All_Map, king, king_idx)\
                                                    & (board->B_Rooks | board->B_Queens))\
                                                    |\
                                                    (NCH_SCAN_BISHOP_LIKE(board->All_Map, king, king_idx)\
                                                    & (board->B_Bishops | board->B_Queens))\
                                                    |\
                                                    (NCH_SCAN_KNIGHT_LIKE(king)\
                                                    & board->B_Knights)\
                                                    |\
                                                    (NCH_SCAN_PAWNATTACK_LIKE_W(king)\
                                                    & board->B_Pawns))

#define _NCH_BLACK_KING_ATTACKERS(board, sqr, idx)  ((NCH_SCAN_ROOK_LIKE(board->All_Map, king, king_idx)\
                                                    & (board->W_Rooks | board->W_Queens))\
                                                    |\
                                                    (NCH_SCAN_BISHOP_LIKE(board->All_Map, king, king_idx)\
                                                    & (board->W_Bishops | board->W_Queens))\
                                                    |\
                                                    (NCH_SCAN_KNIGHT_LIKE(king)\
                                                    & board->W_Knights)\
                                                    |\
                                                    (NCH_SCAN_PAWNATTACK_LIKE_B(king)\
                                                    & board->W_Pawns))

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
            moves_str[idx][current++] = 'h' - (indices[i] % 8);
            moves_str[idx][current++] = (indices[i] / 8) + '1';
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
            moves_str[idx][current++] = 'h' - (move.idx % 8);
            moves_str[idx][current++] = (move.idx / 8) + '1';

            if (NCH_CHKFLG(last_row, move.square)){
                for (int i = 7, last_idx = idx, last_current = current; i < 11; i++, last_current = current){
                    memcpy(moves_str[idx] ,moves_str[last_idx], sizeof(char) * current);
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
            if (NCH_CHKUNI(NCH_ROW8, board->possible_moves[move.idx])){
                count += (NCH_POPCOUNTLL(board->possible_moves[move.idx]) * 3);
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
                count += (NCH_POPCOUNTLL(board->possible_moves[move.idx]) * 3);
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

int _CBoard_MakeMove(CBoard* board, int turn, cuint64* piece_map, cuint64 from_, cuint64 to_, cuint64 cap_sqr){
    NCH_CNGFLG(*piece_map, from_, to_);
    int is_cap = 0;

    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->Black_Map, to_)){
            board->B_Pawns &= ~to_;
            board->B_Knights &= ~to_;
            board->B_Bishops &= ~to_;
            board->B_Rooks &= ~to_;
            board->B_Queens &= ~to_;
            is_cap = 1;
        }
    }
    else{
        if (NCH_CHKFLG(board->White_Map, to_)){
            board->W_Pawns &= ~to_;
            board->W_Knights &= ~to_;
            board->W_Bishops &= ~to_;
            board->W_Rooks &= ~to_;
            board->W_Queens &= ~to_;
            is_cap = 1;
        }
    }

    _NCH_ASSAIGN_BOARD_MAPS(board)
    return is_cap;
}

int _CBoard_SetAvaiableMoves(CBoard* board){
    cuint64* pmoves = board->possible_moves;
    cuint64 ply_map, op_map, pmap;
    cuint64 all_map = board->All_Map;
    cuint64 ball_map = all_map | NCH_BOARDER;
    cuint64 temp, king, queens, rooks, bishops, knights;

    int turn = NCH_B_IS_WHITETURN(board) ? NCH_WHITE : NCH_BLACK;

    if (turn == NCH_WHITE){
        ply_map = board->White_Map;
        op_map = board->Black_Map;

        king = board->W_King;
        queens = board->W_Queens;
        rooks = board->W_Rooks;
        bishops = board->W_Bishops;
        knights = board->W_Knights;
    }
    else{
        ply_map = board->Black_Map;
        op_map = board->White_Map;
        
        king = board->B_King;
        queens = board->B_Queens;
        rooks = board->B_Rooks;
        bishops = board->B_Bishops;
        knights = board->B_Knights;
    }

    int king_idx = NCH_SQRIDX(king);
    cuint64 king_vision = NCH_SCAN_QUEEN_LIKE(temp, king, king_idx); 

    cuint64 psqrs;
    if (NCH_B_IS_CHECK(board)){
        psqrs = 0ull;
        if (!NCH_B_IS_MANYCHECKS(board)){
            _NCH_MAP_LOOP(board->king_attackers){
                psqrs |= king_vision & pmoves[move.idx];
            }
        }
    }
    else{
        psqrs = NCH_CUINT64_MAX;
    }

    _CBoard_CLEAN_POSSIBLEMOVES(board);
    
    if (psqrs != 0ull){
        pmap = ~ply_map & psqrs;
        if (turn == NCH_WHITE){
            _NCH_SET_POSSIBLE_MOVES_FOR_PAWN(board->W_Pawns, pmoves, all_map, op_map, NCH_NXTSQR_UP, NCH_NXTSQR_UP2, NCH_NXTSQR_UPRIGHT, NCH_NXTSQR_UPLEFT, NCH_ROW2, NCH_ROW5)
        }
        else{
            _NCH_SET_POSSIBLE_MOVES_FOR_PAWN(board->B_Pawns, pmoves, all_map, op_map, NCH_NXTSQR_DOWN, NCH_NXTSQR_DOWN2, NCH_NXTSQR_DOWNRIGHT, NCH_NXTSQR_DOWNLEFT, NCH_ROW7, NCH_ROW4)
        }

        _NCH_MAP_LOOP_NOIDX(knights){
            pmoves[NCH_SQRIDX(move.square)] =  NCH_SCAN_KNIGHT_LIKE(move.square)
                                                & pmap;
        }

        _NCH_MAP_LOOP(rooks){
            temp = all_map &~ move.square | NCH_BOARDER;
            pmoves[move.idx] =  NCH_SCAN_ROOK_LIKE(temp, move.square, move.idx) 
                                & pmap;
        }

        _NCH_MAP_LOOP(bishops){
            temp = all_map &~ move.square | NCH_BOARDER;
            pmoves[move.idx] =  NCH_SCAN_BISHOP_LIKE(temp, move.square, move.idx)
                                & pmap;
        }

        _NCH_MAP_LOOP(queens){
            temp = all_map &~ move.square | NCH_BOARDER;
            pmoves[move.idx] =  NCH_SCAN_QUEEN_LIKE(temp, move.square, move.idx)
                                & pmap;
        }
    }
    pmap = ~ply_map & ~psqrs;
    pmoves[king_idx] = NCH_SCAN_KING_LIKE(king)
                       & pmap;

    temp = (ply_map &~ king_vision | op_map);

}

int _CBoard_Update(CBoard* board){

    cuint64 king = NCH_B_IS_WHITETURN(board) ? board->W_King : board->B_King;
    int king_idx = NCH_SQRIDX(king);
    if (NCH_B_IS_WHITETURN(board)){
        board->king_attackers = _NCH_WHITE_KING_ATTACKERS(board, king, king_idx);
    }    
    else{
        board->king_attackers = _NCH_BLACK_KING_ATTACKERS(board, king, king_idx);
    }
    if (board->king_attackers != 0ull){
        NCH_SETFLG(board->flags, NCH_B_CHECK);
        if (NCH_POPCOUNTLL(board->king_attackers) > 1){
            NCH_SETFLG(board->flags, NCH_B_MANYCHECKS);
        }
    }

    _CBoard_SetAvaiableMoves(board);
    return 0;
}

int _CBoard_Step(CBoard* board, cuint64* piece_map, cuint64 from_, cuint64 to_){
    if (!NCH_CHKFLG(board->possible_moves[NCH_SQRIDX(from_)], to_)){
        return -1;
    }

    NCH_CNGFLG(*piece_map, from_, to_)

    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->Black_Map, to_)){
            board->B_Pawns &= ~to_;
            board->B_Knights &= ~to_;
            board->B_Bishops &= ~to_;
            board->B_Rooks &= ~to_;
            board->B_Queens &= ~to_;
        }
    }
    else{
        if (NCH_CHKFLG(board->White_Map, to_)){
            board->W_Pawns &= ~to_;
            board->W_Knights &= ~to_;
            board->W_Bishops &= ~to_;
            board->W_Rooks &= ~to_;
            board->W_Queens &= ~to_;
        }
    }

    _NCH_ASSAIGN_BOARD_MAPS(board)
    NCH_FLPFLG(board->flags, NCH_B_TURN);

    _CBoard_Update(board);

    return 0;
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
    board->enpassant_sqr = 0ull;

    board->White_Map = NCH_B_GET_WHITEMAP(board);
    board->Black_Map = NCH_B_GET_BLACKMAP(board);
    board->All_Map = board->Black_Map | board->White_Map;

    NCH_B_SET_WHITETURN(board);

    _CBoard_Update(board);

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
    
    board->White_Map = NCH_B_GET_WHITEMAP(board);
    board->Black_Map = NCH_B_GET_BLACKMAP(board);
    board->All_Map = board->Black_Map | board->White_Map;

    _CBoard_Update(board);

    return board;
}

#endif