#include "./base.h"
#include "nchess.h"
#include <stdio.h>

typedef int (*perfttest) (int);

void test_all_perft(char* testing_header, perfttest* funcs, int num_funcs, int tiny){

    int len = strlen(testing_header);

    printf("\n\n");
    printf("==============================\n");
    
    int prefix = 15 - len / 2;
    for (int i = 0; i < prefix; i++){
        printf(" ");
    }

    printf(testing_header);
    printf("\n==============================\n");

    int out;
    perfttest current;
    for (int i = 0; i < num_funcs; i++){
        current = funcs[i];
        out = current(tiny);
        printf("Test %i: %s\n", i + 1, out ? "Success" : "Fail");
    }

    printf("==============================\n");
}

int
perft_test(char* fen, long long* expected, int len){
    Board* board = Board_FromFen(fen);
    if (!board){
        printf("Failed to create board\n");
        return 0;
    }

    int res = 1;
    long long out;

    for (int i = 0; i < len; i++){
        out = Board_PerftNoPrint(board, i+1);
        if (out != expected[i]){
            res = 0;
            break;
        }
    }

    Board_Free(board);
    return res;
}

int
test_perft_1(int tiny){
    char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    long long perft_results[] = {20, 400, 8902, 197281, 4865609, 119060324};
    int len = tiny ? 5 : 6;
    return perft_test(fen, perft_results, len);
}

int
test_perft_2(int tiny){
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    long long perft_results[] = {48, 2039, 97862, 4085603, 193690690, 8031647685};
    int len = tiny ? 5 : 6;
    return perft_test(fen, perft_results, len);
}

int
test_perft_3(int tiny){
    char fen[] = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
    long long perft_results[] = {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
    int len = tiny ? 6 : 8;
    return perft_test(fen, perft_results, len);
}

int
test_perft_4(int tiny){
    char fen[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    long long perft_results[] = {6, 264, 9467, 422333, 15833292, 706045033};
    int len = tiny ? 5 : 6;
    return perft_test(fen, perft_results, len);
}

int
test_perft_5(int tiny){
    char fen[] = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    long long perft_results[] = {44, 1486, 62379, 2103487, 89941194};
    int len = 5;
    return perft_test(fen, perft_results, len);
}

int
test_perft_6(int tiny){
    char fen[] = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    long long perft_results[] = {46, 2079, 89890, 3894594, 164075551, 6923051137};
    int len = tiny ? 5 : 6;
    return perft_test(fen, perft_results, len);
}

void test_perft_main(int init_bb, int tiny){
    if (init_bb)
        InitBitboards();

    perfttest funcs[] = {
        test_perft_1,
        test_perft_2,
        test_perft_3,
        test_perft_4,
        test_perft_5,
        test_perft_6,
    };

    test_all_perft("Perft", funcs, 6, tiny);
}