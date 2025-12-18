#include "main.h"
#include "helpers.h"

// Use fast tests by default (depth 5), set to 0 for full depth tests
#define PERFT_FAST_MODE 1

// Helper function to test perft results
static int perft_test_helper(const char* fen, long long* expected, int depth) {
    Board* board = Board_NewFen(fen);
    ASSERT_NOT_NULL(board);
    
    for (int i = 0; i < depth; i++) {
        long long result = Board_PerftNoPrint(board, i + 1);
        if (result != expected[i]) {
            printf("  Depth %d: Expected %lld, got %lld\n", i + 1, expected[i], result);
            Board_Free(board);
            return 0;
        }
    }
    
    Board_Free(board);
    return 1;
}

// Test perft on starting position
static int test_perft_starting_position(void) {
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    long long expected[] = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860};
    int depth = PERFT_FAST_MODE ? 5 : 7;
    return perft_test_helper(fen, expected, depth);
}

// Test perft on position 2 (Kiwipete)
static int test_perft_kiwipete(void) {
    const char* fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    long long expected[] = {48, 2039, 97862, 4085603, 193690690, 8031647685};
    int depth = PERFT_FAST_MODE ? 5 : 6;
    return perft_test_helper(fen, expected, depth);
}

// Test perft on position 3
static int test_perft_position_3(void) {
    const char* fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
    long long expected[] = {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
    int depth = PERFT_FAST_MODE ? 6 : 8;
    return perft_test_helper(fen, expected, depth);
}

// Test perft on position 4
static int test_perft_position_4(void) {
    const char* fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    long long expected[] = {6, 264, 9467, 422333, 15833292, 706045033};
    int depth = PERFT_FAST_MODE ? 5 : 6;
    return perft_test_helper(fen, expected, depth);
}

// Test perft on position 5
static int test_perft_position_5(void) {
    const char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    long long expected[] = {44, 1486, 62379, 2103487, 89941194};
    int depth = 5;
    return perft_test_helper(fen, expected, depth);
}

// Test perft on position 6
static int test_perft_position_6(void) {
    const char* fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    long long expected[] = {46, 2079, 89890, 3894594, 164075551, 6923051137};
    int depth = PERFT_FAST_MODE ? 5 : 6;
    return perft_test_helper(fen, expected, depth);
}

// Test basic move generation count
static int test_perft_basic(void) {
    Board board;
    Board_Init(&board);
    
    long long result = Board_PerftNoPrint(&board, 1);
    ASSERT_EQ(result, 20);
    
    return 1;
}

// Test perft depth 1 various positions
static int test_perft_depth1_positions(void) {
    const char* fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
    };
    
    long long expected[] = {20, 48, 14, 44};
    
    for (int i = 0; i < 4; i++) {
        Board* board = Board_NewFen(fens[i]);
        ASSERT_NOT_NULL(board);
        
        long long result = Board_PerftNoPrint(board, 1);
        ASSERT_EQ(result, expected[i]);
        
        Board_Free(board);
    }
    
    return 1;
}

// Test perft with printing
static int test_perft_with_print(void) {
    Board board;
    Board_Init(&board);
    
    printf("\n  Running perft depth 2 with output:\n");
    long long result = Board_Perft(&board, 2);
    ASSERT_EQ(result, 400);
    
    return 1;
}

// Test perft consistency
static int test_perft_consistency(void) {
    Board board;
    Board_Init(&board);
    
    // Run same perft twice, should get same result
    long long result1 = Board_PerftNoPrint(&board, 3);
    Board_Reset(&board);
    long long result2 = Board_PerftNoPrint(&board, 3);
    
    ASSERT_EQ(result1, result2);
    ASSERT_EQ(result1, 8902);
    
    return 1;
}

// Test perft after undo
static int test_perft_after_undo(void) {
    Board board;
    Board_Init(&board);
    
    long long initial = Board_PerftNoPrint(&board, 3);
    
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Undo(&board);
    Board_Undo(&board);
    
    long long after_undo = Board_PerftNoPrint(&board, 3);
    
    ASSERT_EQ(initial, after_undo);
    
    return 1;
}

// Test suite runner
void test_perft_suite(TestResults* results) {
    TestFunc tests[] = {
        test_perft_basic,
        test_perft_starting_position,
        test_perft_kiwipete,
        test_perft_position_3,
        test_perft_position_4,
        test_perft_position_5,
        test_perft_position_6,
        test_perft_depth1_positions,
        test_perft_with_print,
        test_perft_consistency,
        test_perft_after_undo
    };
    
    run_test_suite("Perft Tests", tests, 11, results);
}