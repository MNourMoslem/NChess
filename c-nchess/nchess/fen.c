/*
    fen.c

    This file containes the definition of fen.h functions.
*/

#include "fen.h"
#include "utils.h"
#include "board_utils.h"
#include <stdlib.h>
#include <stdio.h>

#define PARSE(func)\
while (*fen == ' ') {fen++;}\
fen = func(board, fen);\
if (!fen) return -1;


NCH_STATIC const char* PIECE_CHAR = "\0PNBRQKpnbrqk";

NCH_STATIC_INLINE char
num_to_char(int num){
    return (char)('0' + num);
}

NCH_STATIC_INLINE int
end_of_str(char s){
    return s == '\0' || s == ' ';
}

NCH_STATIC_INLINE void
char2piece(char c, Side* side, PieceType* piece){
    switch (c)
    {
    case 'P':
        *piece = NCH_Pawn;
        *side = NCH_White;
        break;

    case 'N':
        *piece = NCH_Knight;
        *side = NCH_White;
        break;
    case 'B':
        *piece = NCH_Bishop;
        *side = NCH_White;
        break;

    case 'R':
        *piece = NCH_Rook;
        *side = NCH_White;
        break;

    case 'Q':
        *piece = NCH_Queen;
        *side = NCH_White;
        break;

    case 'K':
        *piece = NCH_King;
        *side = NCH_White;
        break;

    case 'p':
        *piece = NCH_Pawn;
        *side = NCH_Black;
        break;

    case 'n':
        *piece = NCH_Knight;
        *side = NCH_Black;
        break;

    case 'b':
        *piece = NCH_Bishop;
        *side = NCH_Black;
        break;

    case 'r':
        *piece = NCH_Rook;
        *side = NCH_Black;
        break;

    case 'q':
        *piece = NCH_Queen;
        *side = NCH_Black;
        break;

    case 'k':
        *piece = NCH_King;
        *side = NCH_Black;
        break;

    default:
        *piece = NCH_NO_PIECE_TYPE;
        *side = NCH_SIDES_NB;
        break;
    }
}

NCH_STATIC_INLINE int
is_number(char c){
    return c <= '9' && c >= '0';
}

NCH_STATIC_INLINE int
char2number(char c){
    return c - '0';
}

NCH_STATIC_INLINE Square
str2square(const char* s){
    return ('h' - s[0]) + (char2number(s[1]) * 8); 
}

const char*
parse_bb(Board* board, const char* fen){
    Square sqr = NCH_A8;
    PieceType piece;
    Side side;

    for (Piece p = 0; p < NCH_PIECE_NB; p++){
        Board_BB(board, p) = 0ull;
    }

    while (!end_of_str(*fen))
    {
        if (is_number(*fen)){
            sqr -= char2number(*fen);
        }
        else if (*fen != '/'){
            char2piece(*fen, &side, &piece);
            if (piece != NCH_NO_PIECE_TYPE){
                Board_BB_BYTYPE(board, side, piece) |= NCH_SQR(sqr);
                sqr--;
            }
        }
        fen++;
    }

    return fen;
}

const char*
parse_side(Board* board, const char* fen){
    if (*fen == 'w'){
        Board_SIDE(board) = NCH_White;
    }
    else if (*fen == 'b'){
        Board_SIDE(board) = NCH_Black;
    }
    else{
        return NULL;
    }
    fen++;
    return fen;
}

const char*
parse_castles(Board* board, const char* fen){
    Board_CASTLES(board) = 0;
    while (!end_of_str(*fen))
    {
        if (*fen == 'K'){
            NCH_SETFLG(Board_CASTLES(board), Board_CASTLE_WK);
        }
        else if (*fen == 'Q'){
            NCH_SETFLG(Board_CASTLES(board), Board_CASTLE_WQ);
        }
        else if (*fen == 'k'){
            NCH_SETFLG(Board_CASTLES(board), Board_CASTLE_BK);
        }
        else if (*fen == 'q'){
            NCH_SETFLG(Board_CASTLES(board), Board_CASTLE_BQ);
        }
        fen++;
    }

    return fen;
}

const char*
parse_enpassant(Board* board, const char* fen){
    if (*fen != '-'){
        if (end_of_str(*fen))
            return fen;

        Side op_side = Board_OP_SIDE(board);
        Square enp_sqr = str2square(fen);
        if (!is_valid_square(enp_sqr))
            return NULL;

        if (NCH_GET_ROWIDX(enp_sqr) == 6)
            enp_sqr -= 8;
        else if (NCH_GET_ROWIDX(enp_sqr) == 3)
            enp_sqr += 8;
        else
            return NULL;
        
        set_board_enp_settings(board, op_side, enp_sqr);
        fen += 2;
    }
    else{
        fen++;
    }
    return fen;
}

const char*
parse_fifty_counter(Board* board, const char* fen){
    int count = 0;
    while (!end_of_str(*fen))
    {
        if (!is_number(*fen)){
            return NULL;
        }
        count *= 10;
        count += char2number(*fen);
        fen++;
    }
    Board_FIFTY_COUNTER(board) = count;
    return fen;
}

const char*
parse_nmoves(Board* board, const char* fen){
    int count = 0;
    while (!end_of_str(*fen))
    {
        if (!is_number(*fen)){
            return NULL;
        }
        count *= 10;
        count += char2number(*fen);
        fen++;
    }
    if (!count)
        return fen;
        
        
    count--;
    count *= 2;
        
    if (Board_IS_BLACKTURN(board))
        count++;

    Board_NMOVES(board) = count;
    return fen;
}

int parse_fen(Board* board, const char* fen){    
    PARSE(parse_bb)
    PARSE(parse_side)
    PARSE(parse_castles)
    PARSE(parse_enpassant) // fen could end here and it will work
    PARSE(parse_fifty_counter)
    PARSE(parse_nmoves)

    return 0;
}

int
Board_FromFen(const char* fen, Board* dst_board){
    int out = parse_fen(dst_board, fen);
    if (out != 0){
        return -1;
    }
    set_board_occupancy(dst_board);
    init_piecetables(dst_board);
    update_check(dst_board);
    return 0;
}

Board*
Board_NewFen(const char* fen){
    Board* board = Board_NewEmpty();
    if (!board){
        return NULL;
    }

    int res = Board_FromFen(fen, board);
    if (res < 0)
        return NULL;
        
    return board;
}

char*
bb_to_fen(const Board* board, char* fen){
    Piece p;
    char c;
    int count, idx;
    for (int row = 7; row > -1; row--){
        count = 0;
        for (int col = 7; col > -1; col--){
            idx = col + row * 8;
            p = Board_ON_SQUARE(board, idx);
            c = PIECE_CHAR[p];

            if (!c){
                count++;
            }
            else{
                if (count){
                    *fen++ = num_to_char(count);
                }
                *fen++ = c;
                count = 0;
            }
        }
        if (count){
            *fen++ = num_to_char(count);
        }
        *fen++ = '/';
    }

    return --fen;
}

char*
side_to_fen(const Board* board, char* fen){
    *fen++ = Board_IS_WHITETURN(board) ? 'w' : 'b';
    return fen;
}

char*
castle_to_fen(const Board* board, char* fen){
    if (!Board_CASTLES(board)){
        *fen++ = '-';
    }
    else{
        if (Board_IS_CASTLE_WK(board)) *fen++ = 'K';
        if (Board_IS_CASTLE_WQ(board)) *fen++ = 'Q';
        if (Board_IS_CASTLE_BK(board)) *fen++ = 'k';
        if (Board_IS_CASTLE_BQ(board)) *fen++ = 'q';
    }

    return fen;
}

char*
enpassant_to_fen(const Board* board, char* fen){
    if (!Board_ENP_IDX(board)){
        *fen++ = '-';
    }
    else{
        Square sqr = Board_ENP_IDX(board);
        int col = NCH_GET_COLIDX(sqr);
        *fen++ = num_to_char(col);
    }
    return fen;
}

char* _number_to_fen(int num, char* str) {
    char* tail;
    int temp = num;
    int len = 0;

    if (num < 0) {
        *str++ = '-';
        num = -num;
    }

    do {
        temp /= 10;
        len++;
    } while (temp);

    tail = str + len;

    for (int i = len - 1; i >= 0; i--) {
        str[i] = (num % 10) + '0';
        num /= 10;
    }

    return tail;
}

char*
fifty_to_fen(const Board* board, char* fen){
    int fifty = Board_FIFTY_COUNTER(board);
    return _number_to_fen(fifty, fen);
}

char*
nmoves_to_fen(const Board* board, char* fen){
    int nmoves = (Board_NMOVES(board) + 1) / 2;
    if (Board_IS_WHITETURN(board))
        nmoves++;
    return _number_to_fen(nmoves, fen);
}

#define TO_FEN(func, lsv)\
des_fen = func(board, des_fen);\
*des_fen++ = lsv;\

void
Board_AsFen(const Board* board, char* des_fen){
    TO_FEN(bb_to_fen, ' ')
    TO_FEN(side_to_fen, ' ')
    TO_FEN(castle_to_fen, ' ')
    TO_FEN(enpassant_to_fen, ' ')
    TO_FEN(fifty_to_fen, ' ')
    TO_FEN(nmoves_to_fen, '\0')
}