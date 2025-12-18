#include "main.h"
#include "helpers.h"

// Test move creation and properties
static int test_move_creation(void) {
    Move move = Move_New(NCH_E2, NCH_E4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    
    ASSERT_EQ(Move_FROM(move), NCH_E2);
    ASSERT_EQ(Move_TO(move), NCH_E4);
    ASSERT_EQ(Move_TYPE(move), MoveType_Normal);
    
    return 1;
}

// Test move from UCI string
static int test_move_from_string(void) {
    Move move;
    int result = Move_FromString("e2e4", &move);
    
    ASSERT(result == 1);
    ASSERT_EQ(Move_FROM(move), NCH_E2);
    ASSERT_EQ(Move_TO(move), NCH_E4);
    
    return 1;
}

// Test promotion move from string
static int test_move_promotion_string(void) {
    Move move;
    int result = Move_FromString("e7e8q", &move);
    
    ASSERT(result == 1);
    ASSERT_EQ(Move_FROM(move), NCH_E7);
    ASSERT_EQ(Move_TO(move), NCH_E8);
    ASSERT_EQ(Move_TYPE(move), MoveType_Promotion);
    ASSERT_EQ(Move_PRO_PIECE(move), NCH_Queen);
    
    return 1;
}

// Test move to string conversion
static int test_move_to_string(void) {
    Move move = Move_New(NCH_G1, NCH_F3, MoveType_Normal, NCH_NO_PIECE_TYPE);
    char move_str[10];
    
    int result = Move_AsString(move, move_str);
    ASSERT(result == 0);
    ASSERT_STR_EQ(move_str, "g1f3");
    
    return 1;
}

// Test promotion move to string
static int test_move_promotion_to_string(void) {
    Move move = Move_New(NCH_A7, NCH_A8, MoveType_Promotion, NCH_Queen);
    char move_str[10];
    
    int result = Move_AsString(move, move_str);
    ASSERT(result == 0);
    ASSERT_STR_EQ(move_str, "a7a8q");
    
    return 1;
}

// Test invalid move string
static int test_move_invalid_string(void) {
    Move move;
    int result = Move_FromString("invalid", &move);
    
    ASSERT(result == 0);
    
    return 1;
}

// Test move equality
static int test_move_equality(void) {
    Move move1 = Move_New(NCH_E2, NCH_E4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    Move move2 = Move_New(NCH_E2, NCH_E4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    Move move3 = Move_New(NCH_D2, NCH_D4, MoveType_Normal, NCH_NO_PIECE_TYPE);
    
    ASSERT(move1 == move2);
    ASSERT(move1 != move3);
    
    return 1;
}

// Test castle move
static int test_move_castle(void) {
    Move move = Move_New(NCH_E1, NCH_G1, MoveType_Castle, NCH_NO_PIECE_TYPE);
    
    ASSERT_EQ(Move_TYPE(move), MoveType_Castle);
    ASSERT(Move_IsCastle(move));
    
    return 1;
}

// Test en passant move
static int test_move_en_passant(void) {
    Move move = Move_New(NCH_E5, NCH_D6, MoveType_EnPassant, NCH_NO_PIECE_TYPE);
    
    ASSERT_EQ(Move_TYPE(move), MoveType_EnPassant);
    ASSERT(Move_IsEnPassant(move));
    
    return 1;
}

// Test move with all promotion pieces
static int test_move_all_promotions(void) {
    PieceType promotions[] = {NCH_Queen, NCH_Rook, NCH_Bishop, NCH_Knight};
    const char* promo_strs[] = {"q", "r", "b", "n"};
    
    for (int i = 0; i < 4; i++) {
        Move move = Move_New(NCH_E7, NCH_E8, MoveType_Promotion, promotions[i]);
        ASSERT_EQ(Move_PRO_PIECE(move), promotions[i]);
        
        char move_str[10];
        Move_AsString(move, move_str);
        ASSERT(move_str[4] == promo_strs[i][0]);
    }
    
    return 1;
}

// Test suite runner
void test_move_suite(TestResults* results) {
    TestFunc tests[] = {
        test_move_creation,
        test_move_from_string,
        test_move_promotion_string,
        test_move_to_string,
        test_move_promotion_to_string,
        test_move_invalid_string,
        test_move_equality,
        test_move_castle,
        test_move_en_passant,
        test_move_all_promotions
    };
    
    run_test_suite("Move Tests", tests, 10, results);
}
