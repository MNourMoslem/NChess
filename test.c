#include "NChess/src/include/nchess.h"


int main(){
    CBoard* board = CBoard_New();
    if (!board){
        return -1;
    }
    CBoard_Step(board, &board->W_Pawns, NCH_SQR(11), NCH_SQR(27), NCH_PROMOTION_TO_QUEEN);
    CBoard_Step(board, &board->B_Pawns, NCH_SQR(50), NCH_SQR(34), NCH_PROMOTION_TO_QUEEN);

    CBoard_Print(board);

    int n = CBoard_NumberPossibleMoves(board);
    printf("\nN: %i\n", n);

    char moves_str[100][8];

    CBoard_PossibleMovesAsString(board, moves_str);

    for (int i = 0; i < n; i++){
        printf("%s ", moves_str[i]);
    }

    free(board);

    return 0;
}