#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "NChess/src/include/nchess.h"

int main(){

    CBoard* board = CBoard_New();
    if (!board){
        return -1;
    }

    _CBoard_Step(board, &board->W_Pawns, NCH_E2, NCH_E4);
    _CBoard_Step(board, &board->B_Pawns, NCH_D7, NCH_D5);

    CBoard_Print(board);

    char moves[100][8];
    int N = CBoard_PossibleMovesAsString(board, moves);

    printf("Number of Possible Moves: %i\n", N);
    for(int i = 0; i < N; i++){
        printf("%s ", moves[i]);
    }


    CBoard_Free(board);

    return 0;
}