/*
    move.c

    This file contains all function definitions of move.h
*/

#include "move.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

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

Move
Move_New(Square from_, Square to_, MoveType type, PieceType promotion_piece){
    if (promotion_piece <= NCH_Pawn || promotion_piece >= NCH_King)
        promotion_piece = 1;

    if (!MoveType_IsValid(type))
        type = MoveType_Normal;

    if (!is_valid_square(from_) || !is_valid_square(to_))
        return Move_NULL;

    return _Move_New(from_, to_, promotion_piece, type);
}

Move
Move_FromString(const char* move_str){
    if (strlen(move_str) > 5)
        return Move_NULL;

    Square from_ = str_to_square(move_str);
    Square to_ = str_to_square(move_str + 2);
    PieceType promotion_piece;
    MoveType type;

    const char pp = move_str[4];
    if (pp  != '\0'){
        if (pp == 'q'){
            promotion_piece = NCH_Queen;
        }
        else if (pp == 'r'){
            promotion_piece = NCH_Rook;
        }
        else if (pp == 'b'){
            promotion_piece = NCH_Bishop;
        }
        else if (pp == 'k'){
            promotion_piece = NCH_Knight;
        }
        else{
            promotion_piece = NCH_Queen;
        }
        type = MoveType_Promotion;
    }
    else{
        promotion_piece = NCH_NO_PIECE;
        type = MoveType_Normal;
    }

    return Move_New(from_, to_, promotion_piece, type);
}

Move
Move_FromStringAndType(const char* move_str, MoveType type){
    Move move = Move_FromString(move_str);
    if (move != Move_NULL){
        if (!MoveType_IsValid(type))
            return Move_NULL;

        move = Move_REASSAGIN_TYPE(move, type);
    }
    return move;
}

int
Move_AsString(Move move, char* dst){
    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);
    PieceType promotion = Move_TYPE(move) == MoveType_Promotion 
                    ? Move_PRO_PIECE(move)
                    : 0;

    if (!is_valid_square(from_) || !is_valid_square(to_)){
        return -1;
    }

    dst[0] = squares_char[from_][0];
    dst[1] = squares_char[from_][1];
    dst[2] = squares_char[to_][0];
    dst[3] = squares_char[to_][1];

    if (promotion){
        if (promotion == NCH_Queen)
            dst[4] = 'q';
        else if (promotion == NCH_Rook)
            dst[4] = 'r';
        else if (promotion == NCH_Knight)
            dst[4] = 'n';
        else if (promotion == NCH_Bishop)
            dst[4] = 'b';
        else{
            dst[4] = '\0';
            return 0;
        }

        dst[5] = '\0';
    }
    else{
        dst[4] = '\0';
    }

    return 0;
}

void
Move_Print(Move move){
    char buffer[7];
    Move_AsString(move, buffer);
    printf(buffer);
}

void
Move_PrintAll(Move* moves, int nmoves){
    char buffer[7];
    for (int i = 0; i < nmoves; i++){
        Move_AsString(moves[i], buffer);
        printf("%s ", buffer);
    }
    printf("\n");
}