#include "main.h"
#include "helpers.h"

// Test basic board copy
static int test_copy_basic(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    Board* copy_board = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy_board);
    
    ASSERT(boards_are_equal(board, copy_board));
    
    Board_Free(board);
    Board_Free(copy_board);
    return 1;
}

// Test copy independence after move
static int test_copy_independence(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    Board* copy_board = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy_board);
    
    // Make a move on original board
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    ASSERT(nmoves > 0);
    
    Board_StepByMove(board, moves[0]);
    
    // Boards should now be different
    ASSERT(!boards_are_equal(board, copy_board));
    
    Board_Free(board);
    Board_Free(copy_board);
    return 1;
}

// Test copy with multiple moves
static int test_copy_after_moves(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    Board* copy_board = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy_board);
    
    // Play same moves on both boards
    Move moves[256];
    int nmoves;
    for (int i = 0; i < 50; i++) {
        nmoves = Board_GenerateLegalMoves(board, moves);
        if (nmoves == 0)
            break;
        
        Board_StepByMove(board, moves[0]);
        Board_StepByMove(copy_board, moves[0]);
    }
    
    // Boards should still be equal
    ASSERT(boards_are_equal(board, copy_board));
    
    Board_Free(board);
    Board_Free(copy_board);
    return 1;
}

// Test copy with bitboard modification
static int test_copy_bitboard_modification(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    Board* copy_board = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy_board);
    
    // Modify copy's bitboard
    Board_BB(copy_board, NCH_WPawn) = NCH_BOARD_B_QUEEN_STARTPOS;
    
    // Boards should be different
    ASSERT(!boards_are_equal(board, copy_board));
    
    Board_Free(board);
    Board_Free(copy_board);
    return 1;
}

// Test deep copy of movelist
static int test_copy_movelist(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    // Make several moves to populate movelist
    Move moves[256];
    for (int i = 0; i < 10; i++) {
        int nmoves = Board_GenerateLegalMoves(board, moves);
        if (nmoves == 0)
            break;
        Board_StepByMove(board, moves[0]);
    }
    
    Board* copy_board = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy_board);
    
    // Verify movelists are equal
    ASSERT(boards_are_equal(board, copy_board));
    
    Board_Free(board);
    Board_Free(copy_board);
    return 1;
}

// Test suite runner
void test_copy_suite(TestResults* results) {
    TestFunc tests[] = {
        test_copy_basic,
        test_copy_independence,
        test_copy_after_moves,
        test_copy_bitboard_modification,
        test_copy_movelist
    };
    
    run_test_suite("Board Copy Tests", tests, 5, results);
}