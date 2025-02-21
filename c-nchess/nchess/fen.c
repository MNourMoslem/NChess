/*
    fen.c

    This file containes the definition of fen.h functions.
*/

#include "fen.h"
#include "utils.h"
#include "board_utils.h"
#include <stdlib.h>
#include <stdio.h>

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
    #if NCH_H1 == 0
        return ('h' - s[0]) + (char2number(s[1]) * 8); 
    #else
        return (s[0] - 'a') + ((8 - char2number(s[1])) * 8);
    #endif
}

int
parse_fen(Board* board, char* fen){
    Square sqr = NCH_A8;
    Piece piece;
    Side side;

    while (*fen != ' ')
    {
        if (is_number(*fen)){
            sqr -= char2number(*fen);
        }
        else if (*fen != '/'){
            char2piece(*fen, &side, &piece);
            if (piece != NCH_NO_PIECE){
                board->bitboards[side][piece] |= NCH_SQR(sqr);
                board->piecetables[side][sqr] = piece;
                sqr--;
            }
        }
        fen++;
    }
    for (Side s = 0; s < NCH_SIDES_NB; s++){
        for (Piece p = 0; p < NCH_PIECE_NB; p++){
            board->occupancy[s] |= board->bitboards[s][p];
        }
        board->occupancy[NCH_SIDES_NB] |= board->occupancy[s];  
    }
    fen++;

    if (*fen == 'w'){
        Board_SIDE(board) = NCH_White;
    }
    else if (*fen == 'b'){
        Board_SIDE(board) = NCH_Black;
    }
    else{
        return -1;
    }

    fen += 2;

    while (*fen != ' ')
    {
        if (*fen == 'K'){
            NCH_SETFLG(board->info.castles, Board_CASTLE_WK);
        }
        else if (*fen == 'Q'){
            NCH_SETFLG(board->info.castles, Board_CASTLE_WQ);
        }
        else if (*fen == 'k'){
            NCH_SETFLG(board->info.castles, Board_CASTLE_BK);
        }
        else if (*fen == 'q'){
            NCH_SETFLG(board->info.castles, Board_CASTLE_BQ);
        }
        fen++;
    }
    fen++;

    if (*fen != '-'){
        Side trg_side = Board_IS_WHITETURN(board) ? NCH_Black : NCH_White;
        if (fen[1] == '6'){
            *(fen+1) = '5';
        }
        else if (fen[1] == '3'){
            *(fen+1) = '4'; 
        }

        Square en_passant_idx = str2square(fen);
        if (!is_valid_square(en_passant_idx)){
            return -1;
        }
        
        set_board_enp_settings(board, trg_side, en_passant_idx);

        fen += 3;
    }
    else{
        reset_enpassant_variable(board);
        fen += 2;
    }

    if (*fen == '\0'){
        board->info.fifty_counter = 0;
        board->nmoves = 0;
        return 0;
    }

    if (!is_number(*fen)){
        return -1;
    }
    board->info.fifty_counter = char2number(*fen);
    fen+=2;
    
    if (!is_number(*fen)){
        return -1;
    }
    board->nmoves = char2number(*fen);
    
    return 0;
}

Board*
Board_FromFen(char* fen){
    Board* board = Board_NewEmpty();
    if (!board){
        return NULL;
    }

    int out = parse_fen(board, fen);
    if (out != 0){
        Board_Free(board);
        return NULL;
    }

    set_board_occupancy(board);
    init_piecetables(board);

    update_check(board);

    return board;
}