#include "./base.h"
#include "nchess.h"
#include <stdio.h>

typedef int (*perfttest) (int);

#define FEN_TEST(num, fen_str)\
int test_fen_##num(){\
    const char* fen = fen_str;\
\
    Board board;\
    Board_Init(&board);\
    Board_FromFen(fen, &board);\
\
    char our_fen[300];\
    Board_AsFen(&board, our_fen);\
\
    return !strcmp(fen, our_fen);\
}

FEN_TEST(1, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
FEN_TEST(2, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 32 16")
FEN_TEST(3, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 10 444")
FEN_TEST(4, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1")
FEN_TEST(5, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8")
FEN_TEST(6, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10")

void test_fen_main(int init_bb){
    if (init_bb)
        NCH_Init();

    testfunc funcs[] = {
        test_fen_1,
        test_fen_2,
        test_fen_3,
        test_fen_4,
        test_fen_5,
        test_fen_6,
    };

    test_all("Fen", funcs, 6);
}