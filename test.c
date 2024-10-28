#include "NChess/src/include/nchess.h"

int main(){
    int N = 8;

    cuint64 from_[8] = {NCH_E2, NCH_E7, NCH_F1, NCH_B8, NCH_G1, NCH_F8, NCH_NONE};
    cuint64 to_[8] = {NCH_E4, NCH_E5, NCH_C4, NCH_C6, NCH_F3, NCH_B4, NCH_NONE};
    cuint64 sm_[8] = {NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_OO};

    CBoard* board = CBoard_New();

    if (!board){
        return 1;
    }

    for (int i = 0; i < 6; i++){
        CBoard_Step(board, from_[i], to_[i], sm_[i]);
    }

    char moves[100][8];
    int N_Moves = CBoard_PossibleMovesAsString(board, moves);
    for (int i = 0; i < N_Moves; i++){
        printf("%s ", moves[i]);
        printf("\n");
    }

    printf("CASTLE FLAGS: 0x%x\n", board->castle_flags);

    CBoard_Print(board);

    // CBoard board_list[500];
    // int n_boards = CBoard_StepAllPossibleMoves(board, board_list);

    // for (int i = 0; i < n_boards; i++){
    //     CBoard_Print(&board_list[i]);
    //     printf("\n");
    // }

    printf("Done!\n");

    CBoard_Free(board);
}