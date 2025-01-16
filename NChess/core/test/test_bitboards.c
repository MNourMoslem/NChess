#include "./base.h"
#include "nchess.h"

NCH_STATIC_INLINE int
test_bitboard_1(){
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    Board* board = Board_FromFen(fen);
    if (!board){
        return 0;
    }

    Board_Perft(board, 5);

    Board_Free(board);
    return 1;
}

void test_bitboard_main(int init_bb){
    if (init_bb)
        NCH_Init();

    testfunc funcs[] = {
        test_bitboard_1
    };

    test_all("BitBorad", funcs, 1);
}