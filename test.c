#include "NChess/src/include/nchess.h"

void test_1(char FEN[] ,int depth){
    CBoard* board = CBoard_FromFEN(FEN);
    if (!board){
        return;
    }

    int n_pos = CBoard_Perft(board, depth);

    printf("Possible Number of Possitions: %i, for depth: %i\n", n_pos, depth);

    CBoard_Free(board);
}

void test_2(char FEN[], int print){
    CBoard* board = CBoard_FromFEN(FEN);
    if (!board){
        return;
    }

    if (print){
        CBoard_Print(board);
    }

    char moves[100][8];
    int n_pos = CBoard_PossibleMovesAsString(board, moves);
    for (int i = 0; i < n_pos; i++){
        printf("%s ", moves[i]);
    }
    printf("\n");

    printf("Possible Number of Moves: %i\n", n_pos);

    CBoard_Free(board);
}

void test_3(int depth){
    CBoard* board = CBoard_New();
    if (!board){
        return;
    }

    int n_pos = CBoard_Perft(board, depth);

    printf("Possible Number of Possitions: %i, for depth: %i\n", n_pos, depth);

    CBoard_Free(board);
}

void test_4(char FEN[], char* move, int print){
    CBoard* board = CBoard_FromFEN(FEN);
    if (!board){
        return;
    }
    CBoard_StepFromString(board, move);

    if (print){
        CBoard_Print(board);
    }

    char moves[100][8];
    int n_pos = CBoard_PossibleMovesAsString(board, moves);
    for (int i = 0; i < n_pos; i++){
        printf("%s ", moves[i]);
    }
    printf("\n");

    printf("Possible Number of Moves: %i\n", n_pos);

    CBoard_Free(board);
}

int main(int argc, char* argv[]){
    int depth = atoi(argv[1]);

    char FEN[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    asm("jmb");

    test_1(FEN, depth);
    // test_2(FEN, 1);
    // test_4(FEN, "g2g4", 1);
    // test_3(5);

    // CBoard* board = CBoard_FromFEN(FEN);
    // if (!board){
    //     return -1;
    // }

    // CBoard_Print(board);
    // CBoard_Free(board);
    // return 0;
}