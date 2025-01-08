#include "board.h" 
#include "stdio.h"
#include "loops.h"
#include <string.h>


const char NCH_PIECES[13] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
const char NCH_COLUMNS[8] = {'h' ,'g', 'f', 'e', 'd', 'c', 'b', 'a'};

char* squares_char[] = {
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
Board_Print(Board* board){
    int i = 64;
    uint64 current;
    uint64 white_occ = Board_WHITE_OCC(board);
    uint64 black_occ = Board_BLACK_OCC(board);

    Side side;
    Piece p;

    for (int file = 0; file < 8; file++){
        for (int raw = 0; raw < 8; raw++){
            i--;
            current = NCH_SQR(i);
            
            if (NCH_CHKFLG(white_occ, current)){
                p = Board_WHITE_PIECE(board, i);
                side = NCH_White;
            }
            else if (NCH_CHKFLG(black_occ, current)){
                p = Board_BLACK_PIECE(board, i);
                side = NCH_Black;
            }
            else{
                printf(".");
                continue;
            }
            printf("%c", NCH_PIECES[side * NCH_PIECE_NB + p]);
        }
        printf("\n");
    }
}

void
Board_PrintMoves(Board* board){
    char* src;
    int idx;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        if (board->moves[i]){
           src = squares_char[i];
           LOOP_U64_T(board->moves[i]){
                printf("%s%s ", src, squares_char[idx]);
           }
        }
    }
    printf("\n");
}

int
Board_MovesAsString(Board* board, char buffer[][7]){
    char* src;
    int idx;
    int move_counter = 0;
    Side side = Board_GET_SIDE(board);

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        if (board->moves[i]){
           src = squares_char[i];
           LOOP_U64_T(board->moves[i]){
                buffer[move_counter][0] = src[0];
                buffer[move_counter][1] = src[1];
                buffer[move_counter][2] = squares_char[idx][0];
                buffer[move_counter][3] = squares_char[idx][1];

                if (board->piecetables[side][i] == NCH_Pawn
                    && (idx <= NCH_A1 || idx >= NCH_H8))
                {             
                    memcpy(buffer[move_counter + 1], buffer[move_counter], sizeof(char) * 4);
                    buffer[move_counter+1][4] = 'r';
                    buffer[move_counter+1][5] = '\0';

                    memcpy(buffer[move_counter + 2], buffer[move_counter], sizeof(char) * 4);
                    buffer[move_counter+2][4] = 'b';
                    buffer[move_counter+2][5] = '\0';

                    memcpy(buffer[move_counter + 3], buffer[move_counter], sizeof(char) * 4);
                    buffer[move_counter+3][4] = 'n';
                    buffer[move_counter+3][5] = '\0';

                    buffer[move_counter][4] = 'q';
                    buffer[move_counter][5] = '\0';

                    move_counter+=4;
                    continue;
                }
                buffer[move_counter][4] = '\0';
                move_counter++;
           }
        }
    }
    return move_counter;
}

void
Board_PrintInfo(Board *board){
    printf("=========\n  Board  \n=========\n");
    Board_Print(board);
    printf("=========\n  Moves  \n=========\n");
    Board_PrintMoves(board);
    printf("=========\n  Info  \n=========\n");
    printf("Turn: %s\n", Board_IS_WHITETURN(board) ? "White" : "Black");
    printf("caslte rights: %c%c%c%c\n"  , Board_IS_CASTLE_WK(board) ? 'K' : '-'
                                        , Board_IS_CASTLE_WQ(board) ? 'Q' : '-'
                                        , Board_IS_CASTLE_BK(board) ? 'k' : '-'
                                        , Board_IS_CASTLE_BQ(board) ? 'q' : '-');
    printf("number of moves: %i\n", Board_NMoves(board));
    printf("played turns: %i\n", board->nmoves);
    printf("fifty counter: %i\n", board->fifty_counter);
    printf("enpassant square: %s\n", board->en_passant_trg ? 
                                    squares_char[NCH_SQRIDX(board->en_passant_trg)] : "-");
}   