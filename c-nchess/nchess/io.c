/*
    io.c

    This file contains the function definitions for the io functions.
*/

#include "board.h" 
#include "stdio.h"
#include "loops.h"
#include "generate.h"
#include <string.h>


NCH_STATIC const char* pieces_char = ".PNBRQKpnbrqk";

const char* squares_char[] = {
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
Board_AsString(const Board* board, char* buffer){
    int i = NCH_A8, buffer_idx = 0;
    Piece p;
    for (int raw = 7; raw > -1; raw--){
        for (int file = 7; file > -1; file--){
            i = raw * 8 + file;

            p = Board_ON_SQUARE(board, i);
            buffer[buffer_idx++] = pieces_char[p];
        }
        buffer[buffer_idx++] = '\n';
    }
    buffer[buffer_idx] = '\0';
}


void
Board_Print(const Board* board){
    char buffer[90];
    Board_AsString(board, buffer);
    printf("%s ", buffer);
}
