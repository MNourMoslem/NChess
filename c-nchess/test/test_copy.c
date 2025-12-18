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

// Test copy from FEN position
static int test_copy_from_fen(void) {
    Board* board = Board_NewFen("r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    ASSERT_NOT_NULL(board);
    
    Board* copy = Board_NewCopy(board);
    ASSERT_NOT_NULL(copy);
    
    ASSERT(boards_are_equal(board, copy));
    
    Board_Free(board);
    Board_Free(copy);
    return 1;
}

// Test copy preserves castle rights
static int test_copy_castle_rights(void) {
    Board board;
    Board_Init(&board);
    
    // Remove some castle rights
    Board_Step(&board, "h2h3");
    Board_Step(&board, "a7a6");
    Board_Step(&board, "h1h2");  // Rook move, loses kingside castle
    
    Board* copy = Board_NewCopy(&board);
    ASSERT_NOT_NULL(copy);
    
    ASSERT_EQ(Board_CASTLES(&board), Board_CASTLES(copy));
    ASSERT(!Board_IS_CASTLE_WK(copy));
    
    Board_Free(copy);
    return 1;
}

// Test copy verification after simple move
static int test_copy_simple_verification(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    
    Board* copy = Board_NewCopy(&board);
    ASSERT_NOT_NULL(copy);
    
    // Just check basic equality
    ASSERT_EQ(Board_SIDE(&board), Board_SIDE(copy));
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E4), Board_ON_SQUARE(copy, NCH_E4));
    
    Board_Free(copy);
    return 1;
}

// Test copy nmoves field
static int test_copy_nmoves(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    
    Board* copy = Board_NewCopy(&board);
    ASSERT_NOT_NULL(copy);
    
    ASSERT_EQ(board.nmoves, copy->nmoves);
    
    Board_Free(copy);
    return 1;
}

// Test copy then diverge
static int test_copy_diverge(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    
    Board* copy = Board_NewCopy(&board);
    ASSERT_NOT_NULL(copy);
    
    // Make different moves
    Board_Step(&board, "g1f3");
    Board_Step(copy, "f1c4");
    
    ASSERT(!boards_are_equal(&board, copy));
    
    Board_Free(copy);
    return 1;
}

// Test multiple copies
static int test_copy_multiple(void) {
    Board board;
    Board_Init(&board);
    
    Board* copy1 = Board_NewCopy(&board);
    Board* copy2 = Board_NewCopy(&board);
    Board* copy3 = Board_NewCopy(&board);
    
    ASSERT_NOT_NULL(copy1);
    ASSERT_NOT_NULL(copy2);
    ASSERT_NOT_NULL(copy3);
    
    // Just check basic properties
    ASSERT_EQ(Board_SIDE(&board), Board_SIDE(copy1));
    ASSERT_EQ(Board_SIDE(&board), Board_SIDE(copy2));
    ASSERT_EQ(Board_SIDE(&board), Board_SIDE(copy3));
    
    Board_Free(copy1);
    Board_Free(copy2);
    Board_Free(copy3);
    return 1;
}

// Test suite runner
void test_copy_suite(TestResults* results) {
    TestFunc tests[] = {
        test_copy_basic,
        test_copy_independence,
        test_copy_after_moves,
        test_copy_bitboard_modification,
        test_copy_movelist,
        test_copy_from_fen,
        test_copy_castle_rights,
        test_copy_simple_verification,
        test_copy_nmoves,
        test_copy_diverge,
        test_copy_multiple
    };
    
    run_test_suite("Board Copy Tests", tests, 11, results);
}