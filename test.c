#include "NChess/src/include/nchess.h"

int main(){
    int N = 8;

    cuint64 from_[8] = {NCH_E2, NCH_E7, NCH_F1, NCH_B8, NCH_G1, NCH_F8, NCH_NONE};
    cuint64 to_[8] = {NCH_E4, NCH_E5, NCH_C4, NCH_C6, NCH_F3, NCH_B4, NCH_NONE};
    cuint64 sm_[8] = {NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_NoSM, NCH_OO};

    char FEN[] = "2bqkb2/1pppppp1/3rr3/p2N3p/P2nn2P/R2N3R/1PPPPPP1/2BQKB2 w Kk - 28 17";

    CBoard* board = CBoard_FromFEN(FEN);

    CBoard_Print(board);

    if (NCH_B_IS_WHITETURN(board)){
        printf("WHITE TURN");
    }
    else{
        printf("BLACK TURN");
    }

    printf("\n");
    printf("Castle Flags: 0x%x\n", board->castle_flags);
    printf("Fifty Count: %i\n", board->fifty_count);
    printf("Move Count: %i\n", board->move_count);

    CBoard_Free(board);
}