#include "main.h"
#include "helpers.h"

// Test legal move generation from starting position
static int test_generate_starting_position(void) {
    Board board;
    Board_Init(&board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    
    ASSERT_EQ(nmoves, 20);  // 20 legal moves in starting position
    
    return 1;
}

// Test move generation after e4
static int test_generate_after_e4(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    
    ASSERT_EQ(nmoves, 20);  // Black also has 20 legal moves
    
    return 1;
}

// Test move generation in checkmate position
static int test_generate_checkmate(void) {
    Board* board = Board_NewFen("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    ASSERT_EQ(nmoves, 0);  // Checkmate - no legal moves
    
    Board_Free(board);
    return 1;
}

// Test move generation in stalemate position
static int test_generate_stalemate(void) {
    Board* board = Board_NewFen("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    ASSERT_EQ(nmoves, 0);  // Stalemate - no legal moves
    
    Board_Free(board);
    return 1;
}

// Test pseudo move generation for knight
static int test_generate_knight_moves(void) {
    Board board;
    Board_Init(&board);
    
    Move moves[256];
    int nmoves = Board_GeneratePseudoMovesOf(&board, moves, NCH_G1);
    
    ASSERT(nmoves >= 2);  // Knight on g1 has at least 2 pseudo moves
    
    return 1;
}

// Test legal moves from specific square
static int test_get_moves_of_square(void) {
    Board board;
    Board_Init(&board);
    
    Move moves[256];
    int nmoves = Board_GetMovesOf(&board, NCH_E2, moves);
    
    ASSERT_EQ(nmoves, 2);  // Pawn on e2 can move to e3 or e4
    
    return 1;
}

// Test move generation with pins
static int test_generate_with_pins(void) {
    Board* board = Board_NewFen("rnbqk2r/pppp1ppp/5n2/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    ASSERT(nmoves > 0);
    
    Board_Free(board);
    return 1;
}

// Test castling move generation
static int test_generate_castling(void) {
    Board* board = Board_NewFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    // Check that castling moves are included
    int found_kingside = 0, found_queenside = 0;
    for (int i = 0; i < nmoves; i++) {
        if (Move_FROM(moves[i]) == NCH_E1 && Move_TO(moves[i]) == NCH_G1) {
            found_kingside = 1;
        }
        if (Move_FROM(moves[i]) == NCH_E1 && Move_TO(moves[i]) == NCH_C1) {
            found_queenside = 1;
        }
    }
    
    ASSERT(found_kingside);
    ASSERT(found_queenside);
    
    Board_Free(board);
    return 1;
}

// Test en passant move generation
static int test_generate_en_passant(void) {
    Board board;
    Board_Init(&board);
    
    // Play moves to create en passant scenario
    Board_Step(&board, "e2e4");
    Board_Step(&board, "a7a6");
    Board_Step(&board, "e4e5");
    Board_Step(&board, "d7d5");  // Creates en passant opportunity
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(&board, moves);
    
    // Check if en passant capture exists
    int found_enpassant = 0;
    for (int i = 0; i < nmoves; i++) {
        if (Move_TYPE(moves[i]) == MoveType_EnPassant) {
            found_enpassant = 1;
            break;
        }
    }
    
    ASSERT(found_enpassant);
    
    return 1;
}

// Test promotion move generation
static int test_generate_promotions(void) {
    Board* board = Board_NewFen("8/P7/8/8/8/8/8/K6k w - - 0 1");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    // Should have 4 promotion moves (Q, R, B, N) plus king moves
    int promotion_count = 0;
    for (int i = 0; i < nmoves; i++) {
        if (Move_TYPE(moves[i]) == MoveType_Promotion) {
            promotion_count++;
        }
    }
    
    ASSERT_EQ(promotion_count, 4);
    
    Board_Free(board);
    return 1;
}

// Test suite runner
void test_generate_suite(TestResults* results) {
    TestFunc tests[] = {
        test_generate_starting_position,
        test_generate_after_e4,
        test_generate_checkmate,
        test_generate_stalemate,
        test_generate_knight_moves,
        test_get_moves_of_square,
        test_generate_with_pins,
        test_generate_castling,
        test_generate_en_passant,
        test_generate_promotions
    };
    
    run_test_suite("Move Generation Tests", tests, 10, results);
}
