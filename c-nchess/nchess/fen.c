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


NCH_STATIC_INLINE int
end_of_str(char s){
    return s == '\0' || s == ' ';
}

NCH_STATIC_INLINE void
char2piece(char c, Side* side, Piece* piece){
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
        *piece = NCH_NO_PIECE;
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
str2square(char* s){
    return ('h' - s[0]) + (char2number(s[1]) * 8); 
}

char*
parse_bb(Board* board, char* fen){
    Square sqr = NCH_A8;
    Piece piece;
    Side side;

    while (!end_of_str(*fen))
    {
        if (is_number(*fen)){
            sqr -= char2number(*fen);
        }
        else if (*fen != '/'){
            char2piece(*fen, &side, &piece);
            if (piece != NCH_NO_PIECE){
                Board_BB(board, side, piece) |= NCH_SQR(sqr);
                sqr--;
            }
        }
        fen++;
    }

    return fen;
}

char*
parse_side(Board* board, char* fen){
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

char*
parse_castles(Board* board, char* fen){
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

char*
parse_enpassant(Board* board, char* fen){
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

char*
parse_fifty_counter(Board* board, char* fen){
    int count = 0;
    while (!end_of_str(*fen))
    {
        if (!is_number(*fen)){
            return NULL;
        }
        count += char2number(*fen);
        fen++;
    }
    Board_FIFTY_COUNTER(board) = count;
    return fen;
}

char*
parse_nmoves(Board* board, char* fen){
    int count = 0;
    while (!end_of_str(*fen))
    {
        if (!is_number(*fen)){
            return NULL;
        }
        count += char2number(*fen);
        fen++;
    }
    Board_NMOVES(board) = count;
    return fen;
}

int parse_fen(Board* board, char* fen){    
    PARSE(parse_bb)
    PARSE(parse_side)
    PARSE(parse_castles)
    PARSE(parse_enpassant) // fen could end here and it will work
    PARSE(parse_fifty_counter)
    PARSE(parse_nmoves)

    return 0;
}

int
Board_FromFen(char* fen, Board* dst_board){
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
Board_NewFen(char* fen){
    Board* board = Board_NewEmpty();
    if (!board){
        return NULL;
    }

    int res = Board_FromFen(fen, board);
    if (res < 0)
        return NULL;
        
    return board;
}