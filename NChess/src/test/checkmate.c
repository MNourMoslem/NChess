#include "../include/nchess.h"

int test(){
    int N = 7;

    cuint64 from_[7] = {NCH_E2, NCH_E7, NCH_F1, NCH_B8, NCH_D1, NCH_A8, NCH_F3};
    cuint64 to_[7] = {NCH_E4, NCH_E5, NCH_C4, NCH_C6, NCH_F3, NCH_B8, NCH_F7};

    CBoard* board = CBoard_New();

    if (!board){
        return 1;
    }

    for (int i = 0; i < N; i++){
        CBoard_Step(board, from_, to_, NCH_PROMOTION_TO_QUEEN);
    }

    CBoard_Print(board);
}