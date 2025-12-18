#include "main.h"
#include "helpers.h"

// Test basic board initialization
static int test_board_init(void) {
    Board board;
    Board_Init(&board);
    
    // Check piece counts
    ASSERT(count_bits(Board_WHITE_PAWNS(&board)) == 8);
    ASSERT(count_bits(Board_BLACK_PAWNS(&board)) == 8);
    ASSERT(count_bits(Board_WHITE_KNIGHTS(&board)) == 2);
    ASSERT(count_bits(Board_BLACK_KNIGHTS(&board)) == 2);
    
    // Check initial side
    ASSERT_EQ(Board_SIDE(&board), NCH_White);
    
    // Check castle rights
    uint8 all_castles = Board_CASTLE_WK | Board_CASTLE_WQ | Board_CASTLE_BK | Board_CASTLE_BQ;
    ASSERT_EQ(Board_CASTLES(&board), all_castles);
    
    return 1;
}

// Test empty board initialization
static int test_board_init_empty(void) {
    Board board;
    Board_InitEmpty(&board);
    
    // Check all bitboards are empty
    ASSERT(Board_WHITE_PAWNS(&board) == 0);
    ASSERT(Board_BLACK_PAWNS(&board) == 0);
    ASSERT(Board_ALL_OCC(&board) == 0);
    
    return 1;
}

// Test board dynamic allocation
static int test_board_new(void) {
    Board* board = Board_New();
    ASSERT_NOT_NULL(board);
    
    ASSERT(count_bits(Board_WHITE_PAWNS(board)) == 8);
    ASSERT_EQ(Board_SIDE(board), NCH_White);
    
    Board_Free(board);
    return 1;
}

// Test board after moves
static int test_board_after_moves(void) {
    Board board;
    Board_Init(&board);
    
    // Make some moves
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Step(&board, "g1f3");
    
    ASSERT(board.nmoves > 0);
    ASSERT_EQ(Board_SIDE(&board), NCH_Black);  // Black's turn after 3 moves
    
    return 1;
}

// Test board state check
static int test_board_is_check(void) {
    Board* board = Board_NewFen("rnb1kbnr/pppp1ppp/8/4p3/5PPq/8/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    ASSERT_NOT_NULL(board);
    
    ASSERT(Board_IsCheck(board));
    
    Board_Free(board);
    return 1;
}

// Test board state - not in check
static int test_board_not_check(void) {
    Board board;
    Board_Init(&board);
    
    ASSERT(!Board_IsCheck(&board));
    
    return 1;
}

// Test piece on square
static int test_board_piece_on_square(void) {
    Board board;
    Board_Init(&board);
    
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E2), NCH_WPawn);
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_G1), NCH_WKnight);
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E8), NCH_BKing);
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E4), NCH_NO_PIECE);
    
    return 1;
}

// Test board state after moves
static int test_board_state_after_moves(void) {
    Board board;
    Board_Init(&board);
    
    Board_Step(&board, "e2e4");
    ASSERT_EQ(Board_SIDE(&board), NCH_Black);
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E4), NCH_WPawn);
    ASSERT_EQ(Board_ON_SQUARE(&board, NCH_E2), NCH_NO_PIECE);
    
    Board_Step(&board, "c7c5");
    ASSERT_EQ(Board_SIDE(&board), NCH_White);
    
    return 1;
}

// Test board castle rights loss
static int test_board_castle_rights(void) {
    Board board;
    Board_Init(&board);
    
    // Move king
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Step(&board, "e1e2");
    
    // White should lose both castle rights
    ASSERT(!Board_IS_CASTLE_WK(&board));
    ASSERT(!Board_IS_CASTLE_WQ(&board));
    
    return 1;
}

// Test board fifty move counter
static int test_board_fifty_counter(void) {
    Board board;
    Board_Init(&board);
    
    ASSERT_EQ(Board_FIFTY_COUNTER(&board), 0);
    
    // Pawn move resets counter
    Board_Step(&board, "e2e4");
    ASSERT_EQ(Board_FIFTY_COUNTER(&board), 0);
    
    // Knight moves increment counter
    Board_Step(&board, "g8f6");
    Board_Step(&board, "g1f3");
    ASSERT(Board_FIFTY_COUNTER(&board) > 0);
    
    return 1;
}

// Test board state game over detection
static int test_board_state_checkmate(void) {
    Board* board = Board_NewFen("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int can_move = Board_GenerateLegalMoves(board, moves);
    GameState state = Board_State(board, can_move);
    ASSERT_EQ(state, NCH_GS_BlackWin);
    
    Board_Free(board);
    return 1;
}

// Test board state stalemate
static int test_board_state_stalemate(void) {
    Board* board = Board_NewFen("7k/5Q2/6K1/8/8/8/8/8 b - - 0 1");
    ASSERT_NOT_NULL(board);
    
    Move moves[256];
    int can_move = Board_GenerateLegalMoves(board, moves);
    GameState state = Board_State(board, can_move);
    ASSERT_EQ(state, NCH_GS_Draw_Stalemate);
    
    Board_Free(board);
    return 1;
}

// Test suite runner
void test_board_suite(TestResults* results) {
    TestFunc tests[] = {
        test_board_init,
        test_board_init_empty,
        test_board_new,
        test_board_after_moves,
        test_board_is_check,
        test_board_not_check,
        test_board_piece_on_square,
        test_board_state_after_moves,
        test_board_castle_rights,
        test_board_fifty_counter,
        test_board_state_checkmate,
        test_board_state_stalemate
    };
    
    run_test_suite("Board State Tests", tests, 12, results);
}
