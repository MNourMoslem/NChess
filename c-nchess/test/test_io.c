#include "main.h"
#include "helpers.h"

// Test board printing (basic smoke test)
static int test_io_board_print(void) {
    Board board;
    Board_Init(&board);
    
    printf("\n  Printing board:\n");
    Board_Print(&board);
    
    return 1;
}

// Test FEN output
static int test_io_fen_output(void) {
    Board board;
    Board_Init(&board);
    
    char fen[300];
    Board_AsFen(&board, fen);
    
    ASSERT(strlen(fen) > 0);
    ASSERT_STR_EQ(fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    return 1;
}

// Test move to string conversion
static int test_io_move_to_string(void) {
    Board board;
    Board_Init(&board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    ASSERT(nmoves > 0);
    
    // Test converting first move to string
    char move_str[10];
    int result = Move_AsString(moves[0], move_str);
    ASSERT(result == 0);
    
    ASSERT(strlen(move_str) >= 4);  // At minimum "e2e4" format
    
    return 1;
}

// Test suite runner
void test_io_suite(TestResults* results) {
    TestFunc tests[] = {
        test_io_board_print,
        test_io_fen_output,
        test_io_move_to_string
    };
    
    run_test_suite("I/O Tests", tests, 3, results);
}