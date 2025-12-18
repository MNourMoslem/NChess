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

// Test individual piece bitboards
static int test_bitboard_individual_pieces(void) {
    Board board;
    Board_Init(&board);
    
    // Test white pieces
    ASSERT(count_bits(Board_WHITE_PAWNS(&board)) == 8);
    ASSERT(count_bits(Board_WHITE_KNIGHTS(&board)) == 2);
    ASSERT(count_bits(Board_WHITE_BISHOPS(&board)) == 2);
    ASSERT(count_bits(Board_WHITE_ROOKS(&board)) == 2);
    ASSERT(count_bits(Board_WHITE_QUEENS(&board)) == 1);
    ASSERT(count_bits(Board_WHITE_KING(&board)) == 1);
    
    // Test black pieces
    ASSERT(count_bits(Board_BLACK_PAWNS(&board)) == 8);
    ASSERT(count_bits(Board_BLACK_KNIGHTS(&board)) == 2);
    ASSERT(count_bits(Board_BLACK_BISHOPS(&board)) == 2);
    ASSERT(count_bits(Board_BLACK_ROOKS(&board)) == 2);
    ASSERT(count_bits(Board_BLACK_QUEENS(&board)) == 1);
    ASSERT(count_bits(Board_BLACK_KING(&board)) == 1);
    
    return 1;
}

// Test bitboard operations
static int test_bitboard_bit_operations(void) {
    uint64 bb = NCH_SQR(NCH_E4) | NCH_SQR(NCH_D4) | NCH_SQR(NCH_E5);
    
    ASSERT(count_bits(bb) == 3);
    ASSERT(more_than_one(bb));
    ASSERT(!has_two_bits(bb));
    
    uint64 bb2 = NCH_SQR(NCH_A1) | NCH_SQR(NCH_H8);
    ASSERT(has_two_bits(bb2));
    
    return 1;
}

// Test bitboard clearing
static int test_bitboard_clearing(void) {
    Board board;
    Board_Init(&board);
    
    uint64 initial_white = count_bits(Board_WHITE_OCC(&board));
    
    // Clear pawns
    Board_BB(&board, NCH_WPawn) = 0;
    
    ASSERT(count_bits(Board_WHITE_PAWNS(&board)) == 0);
    // Note: Occupancy bitboard needs manual update
    
    return 1;
}

// Test bitboard complex position
static int test_bitboard_complex_position(void) {
    Board* board = Board_NewFen("r1bqk2r/pppp1ppp/2n2n2/2b1p3/2B1P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 4 5");
    ASSERT_NOT_NULL(board);
    
    uint64 white_occ = Board_WHITE_OCC(board);
    uint64 black_occ = Board_BLACK_OCC(board);
    
    // Both sides should have pieces on board
    ASSERT(count_bits(white_occ) > 10);
    ASSERT(count_bits(black_occ) > 10);
    
    Board_Free(board);
    return 1;
}

// Test bitboard after capture
static int test_bitboard_after_capture(void) {
    Board board;
    Board_Init(&board);
    
    uint64 initial_black = count_bits(Board_BLACK_OCC(&board));
    
    // Scholar's mate setup
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Step(&board, "d1h5");
    Board_Step(&board, "b8c6");
    Board_Step(&board, "f1c4");
    Board_Step(&board, "g8f6");
    Board_Step(&board, "h5f7");  // Checkmate with capture
    
    uint64 final_black = count_bits(Board_BLACK_OCC(&board));
    ASSERT(final_black < initial_black);
    
    return 1;
}

// Test bitboard symmetry
static int test_bitboard_symmetry(void) {
    Board board;
    Board_Init(&board);
    
    // White and black should have same number of pieces initially
    ASSERT_EQ(count_bits(Board_WHITE_OCC(&board)), count_bits(Board_BLACK_OCC(&board)));
    
    return 1;
}

// Test suite runner
void test_bitboard_suite(TestResults* results) {
    TestFunc tests[] = {
        test_bitboard_creation,
        test_bitboard_occupancy,
        test_bitboard_manipulation,
        test_bitboard_individual_pieces,
        test_bitboard_bit_operations,
        test_bitboard_clearing,
        test_bitboard_complex_position,
        test_bitboard_after_capture,
        test_bitboard_symmetry
    };
    
    run_test_suite("BitBoard Tests", tests, 9, results);
}