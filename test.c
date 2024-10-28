#include "NChess/src/include/nchess.h"

int main(){
    int N = 8;

    cuint64 from_[8] = {NCH_E2, NCH_E7, NCH_F1, NCH_B8, NCH_G1, NCH_F8, NCH_NONE};
    cuint64 to_[8] = {NCH_E4, NCH_E5, NCH_C4, NCH_C6, NCH_F3, NCH_B4, NCH_NONE};
    cuint64 sm_[8] = {NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_OO};

    char FEN[] = "1r2kb1r/p1p2ppp/2n2n2/1ppPp1q1/4P1b1/2KB1N2/PPQN1PPP/R1B4R b k - 5 11";

    CBoard* board = CBoard_FromFEN(FEN);

    CBoard_Print(board);

    char moves[100][8];

    int n_moves = CBoard_PossibleMovesAsString(board, moves);

    for (int i = 0; i < n_moves; i++){
        printf("%s ", moves[i]);
    }
    printf("\nNumber of possible moves: %i\n", n_moves);

    CBoard_PrintGameState(board);

    CBoard_Free(board);
}