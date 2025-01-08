#include "move.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

NCH_STATIC_INLINE int
is_valid_column(char arg){
    return arg >= 'a' && arg <= 'h';
}

NCH_STATIC_INLINE int
is_valid_row(char arg){
    return arg >= '1' && arg <= '8';
}

void
Move_Parse(Move move, Square* from_, Square* to_, uint8* castle, Piece* promotion){
    *from_ = Move_FROM(move);
    *to_ = Move_TO(move);
    *castle = Move_CASTLE(move);
    *promotion = Move_PRO_PIECE(move);
}

Move
Move_New(Square from_, Square to_, uint8 castle, Piece promotion){
    Move move;
    move = Move_ASSIGN_FROM(from_) | Move_ASSIGN_TO(to_) 
         | Move_ASSIGN_CASTLE(castle) | Move_ASSIGN_PRO_PIECE(promotion) ;
    return move;
}

int
Move_ParseFromString(char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle){
    int len = strlen(arg);
    if (len > 5){
        return -1;
    }

    if (arg[0] == 'O'){
        if (len == 3){
            *castle = Board_CASTLE_WK | Board_CASTLE_BK;
        }
        else{
            *castle = Board_CASTLE_WQ | Board_CASTLE_BQ;
        }
        *from_ = 0;
        *to_ = 0;
        *promotion = 0;
        return 0;
    }

    if (!is_valid_column(arg[0]) || !is_valid_row(arg[1])
         || !is_valid_column(arg[2]) || !is_valid_row(arg[3]))
    {
        return -1;
    }

    *castle = 0;
    *from_ = ('h' - arg[0]) + 8 * (arg[1] - '1');
    *to_ = ('h' - arg[2]) + 8 * (arg[3] - '1');

    if (len == 5){
        if (arg[4] == 'q'){
            *promotion = NCH_Queen;
        }
        else if (arg[4] == 'r'){
            *promotion = NCH_Rook;
        }
        else if (arg[4] == 'b'){
            *promotion = NCH_Bishop;
        }
        else if (arg[4] == 'n'){
            *promotion = NCH_Knight;
        }
        else{
            return -1;
        }
    }
    else{
        *promotion = 0;
    }
    return 0;
}

Move
Move_FromString(char* move){
    Square from_, to_;
    Piece promotion;
    uint8 castle;
    Move_ParseFromString(move, &from_, &to_, &promotion, &castle);
    return Move_New(from_, to_, castle, promotion);
}

void
Move_Print(Move move){
    Square from_, to_;
    Piece promotion;
    uint8 castle;

    Move_Parse(move, &from_, &to_, &castle, &promotion);

    printf("from: %i\n", from_);
    printf("to: %i\n", to_);
    printf("castle: %s\n", NCH_CHKFLG(castle, Board_CASTLE_WK | Board_CASTLE_BK) ? "king"
                        : NCH_CHKFLG(castle, Board_CASTLE_WQ | Board_CASTLE_WQ) ? "queen" : "none");
    printf("promotion piece: %i\n", promotion);
}

static char* squares_char[] = {
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", 
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2", 
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", 
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4", 
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5", 
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6", 
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7", 
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};

void
Move_AsString(Move move, char* dst){
    char* temp;
    if (Move_CASTLE(move)){
        if (NCH_CHKUNI(Move_CASTLE(move), Board_CASTLE_WK | Board_CASTLE_BK)){
            strcpy(dst, "O-O");
        }
        else{
            strcpy(dst, "O-O-O");
        }
        return;
    }

    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);
    Piece promotion = Move_PRO_PIECE(move);

    temp = squares_char[from_];
    dst[0] = temp[0];
    dst[1] = temp[1];
    temp = squares_char[to_];
    dst[2] = temp[0];
    dst[3] = temp[1];

    if (promotion){
        if (promotion == NCH_Queen)
            dst[4] = 'q';
        else if (promotion == NCH_Rook)
            dst[4] = 'r';
        else if (promotion == NCH_Knight)
            dst[4] = 'n';
        else if (promotion == NCH_Bishop)
            dst[4] = 'b';
        else
            dst[4] = 'q';

        dst[5] = '\0';
    }
    else{
        dst[4] = '\0';
    }
}