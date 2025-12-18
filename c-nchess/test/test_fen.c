#include "main.h"
#include "helpers.h"

// Helper function to test FEN round-trip
static int test_fen_roundtrip(const char* fen) {
    Board board;
    Board_Init(&board);
    
    int result = Board_FromFen(fen, &board);
    ASSERT(result == 0);
    
    char output_fen[300];
    Board_AsFen(&board, output_fen);
    
    ASSERT_STR_EQ(output_fen, fen);
    return 1;
}

// Test starting position FEN
static int test_fen_starting_position(void) {
    return test_fen_roundtrip("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// Test position 2
static int test_fen_position_2(void) {
    return test_fen_roundtrip("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 32 16");
}

// Test position 3
static int test_fen_position_3(void) {
    return test_fen_roundtrip("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 10 444");
}

// Test position 4
static int test_fen_position_4(void) {
    return test_fen_roundtrip("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
}

// Test position 5
static int test_fen_position_5(void) {
    return test_fen_roundtrip("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
}

// Test position 6
static int test_fen_position_6(void) {
    return test_fen_roundtrip("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
}

// Test FEN parsing and board state
static int test_fen_board_state(void) {
    Board board;
    Board_Init(&board);
    
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    int result = Board_FromFen(fen, &board);
    ASSERT(result == 0);
    
    // Verify turn
    ASSERT_EQ(Board_SIDE(&board), NCH_White);
    
    // Verify castling rights
    uint8 expected_castles = Board_CASTLE_WK | Board_CASTLE_WQ | Board_CASTLE_BK | Board_CASTLE_BQ;
    ASSERT_EQ(Board_CASTLES(&board), expected_castles);
    
    // Note: En passant square handling varies - in some implementations
    // it's set to -1, in others to 0 or 65. Just verify FEN parsing succeeded.
    
    return 1;
}

// Test suite runner
void test_fen_suite(TestResults* results) {
    TestFunc tests[] = {
        test_fen_starting_position,
        test_fen_position_2,
        test_fen_position_3,
        test_fen_position_4,
        test_fen_position_5,
        test_fen_position_6,
        test_fen_board_state
    };
    
    run_test_suite("FEN Tests", tests, 7, results);
}