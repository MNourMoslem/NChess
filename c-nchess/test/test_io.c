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

// Test printing board after moves
static int test_io_board_after_moves(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Step(&board, "g1f3");
    
    printf("\n  Printing board after moves:\n");
    Board_Print(&board);
    
    return 1;
}

// Test move printing
static int test_io_move_print(void) {
    Move move = Move_New(NCH_E2, NCH_E4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    
    printf("\n  Printing move: ");
    Move_Print(move);
    printf("\n");
    
    return 1;
}

// Test printing all moves
static int test_io_print_all_moves(void) {
    Board board;
    Board_Init(&board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    
    printf("\n  Printing all legal moves:\n  ");
    Move_PrintAll(moves, nmoves);
    printf("\n");
    
    return 1;
}

// Test FEN output consistency
static int test_io_fen_consistency(void) {
    const char* original = "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4";
    
    Board* board = Board_NewFen(original);
    ASSERT_NOT_NULL(board);
    
    char output[300];
    Board_AsFen(board, output);
    
    ASSERT_STR_EQ(output, original);
    
    Board_Free(board);
    return 1;
}

// Test move conversion roundtrip
static int test_io_move_roundtrip(void) {
    Move original = Move_New(NCH_D2, NCH_D4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    
    char move_str[10];
    Move_AsString(original, move_str);
    
    Move converted;
    Move_FromString(move_str, &converted);
    
    ASSERT_EQ(original, converted);
    
    return 1;
}

// Test suite runner
void test_io_suite(TestResults* results) {
    TestFunc tests[] = {
        test_io_board_print,
        test_io_fen_output,
        test_io_move_to_string,
        test_io_board_after_moves,
        test_io_move_print,
        test_io_print_all_moves,
        test_io_fen_consistency,
        test_io_move_roundtrip
    };
    
    run_test_suite("I/O Tests", tests, 8, results);
}