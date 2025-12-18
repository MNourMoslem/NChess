#include "main.h"
#include "helpers.h"

// Test basic bitboard operations
static int test_bitboard_creation(void) {
    Board* board = Board_NewFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    ASSERT_NOT_NULL(board);
    
    // Verify bitboards are properly initialized
    uint64 white_pawns = Board_BB(board, NCH_WPawn);
    ASSERT(white_pawns != 0);
    
    uint64 black_pawns = Board_BB(board, NCH_BPawn);
    ASSERT(black_pawns != 0);
    
    Board_Free(board);
    return 1;
}

// Test bitboard occupancy
static int test_bitboard_occupancy(void) {
    Board board;
    Board_Init(&board);
    
    uint64 white_occ = Board_WHITE_OCC(&board);
    uint64 black_occ = Board_BLACK_OCC(&board);
    uint64 all_occ = Board_ALL_OCC(&board);
    
    // Starting position should have 16 pieces per side
    ASSERT(count_bits(white_occ) == 16);
    ASSERT(count_bits(black_occ) == 16);
    ASSERT(count_bits(all_occ) == 32);
    
    return 1;
}

// Test bitboard manipulation
static int test_bitboard_manipulation(void) {
    Board board;
    Board_Init(&board);
    
    uint64 original_pawns = Board_BB(&board, NCH_WPawn);
    
    // Make a pawn move
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    ASSERT(nmoves > 0);
    
    // Find and play e2e4
    int found = 0;
    for (int i = 0; i < nmoves; i++) {
        if (Move_FROM(moves[i]) == 11 && Move_TO(moves[i]) == 27) {
            Board_StepByMove(&board, moves[i]);
            found = 1;
            break;
        }
    }
    ASSERT(found);
    
    // Verify pawn bitboard changed
    uint64 new_pawns = Board_BB(&board, NCH_WPawn);
    ASSERT(new_pawns != original_pawns);
    
    return 1;
}

// Test suite runner
void test_bitboard_suite(TestResults* results) {
    TestFunc tests[] = {
        test_bitboard_creation,
        test_bitboard_occupancy,
        test_bitboard_manipulation
    };
    
    run_test_suite("BitBoard Tests", tests, 3, results);
}