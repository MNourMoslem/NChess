#include "main.h"
#include "helpers.h"

// Run a test suite and update results
void run_test_suite(const char* suite_name, TestFunc* tests, int test_count, TestResults* results) {
    printf("\n========================================\n");
    printf("  %s\n", suite_name);
    printf("========================================\n");
    
    TestResults suite_results = {0, 0, 0};
    
    for (int i = 0; i < test_count; i++) {
        suite_results.total++;
        printf("Test %d: ", i + 1);
        
        if (tests[i]()) {
            printf("PASSED\n");
            suite_results.passed++;
        } else {
            printf("FAILED\n");
            suite_results.failed++;
        }
    }
    
    // Update global results
    results->total += suite_results.total;
    results->passed += suite_results.passed;
    results->failed += suite_results.failed;
    
    // Print suite summary
    printf("----------------------------------------\n");
    printf("Suite Results: %d/%d passed\n", suite_results.passed, suite_results.total);
    printf("========================================\n");
}

// Print final test results
void print_final_results(TestResults* results) {
    printf("\n\n");
    printf("========================================\n");
    printf("       FINAL TEST RESULTS\n");
    printf("========================================\n");
    printf("Total Tests:  %d\n", results->total);
    printf("Passed:       %d\n", results->passed);
    printf("Failed:       %d\n", results->failed);
    printf("Success Rate: %.2f%%\n", 
           results->total > 0 ? (results->passed * 100.0 / results->total) : 0.0);
    printf("========================================\n");
    
    if (results->failed == 0) {
        printf("\n✓ All tests passed!\n\n");
    } else {
        printf("\n✗ Some tests failed.\n\n");
    }
}

int main(void) {
    // Initialize NChess library
    NCH_Init();
    
    TestResults results = {0, 0, 0};
    
    // Run all test suites
    printf("Starting comprehensive test suite...\n");
    
    test_board_suite(&results);
    test_bitboard_suite(&results);
    test_move_suite(&results);
    test_generate_suite(&results);
    test_perft_suite(&results);
    test_copy_suite(&results);
    test_fen_suite(&results);
    test_hash_suite(&results);
    test_io_suite(&results);
    
    // Print final results
    print_final_results(&results);
    
    return (results.failed == 0) ? 0 : 1;
}

// Implementation of helper functions
void print_bitboard(uint64 bb) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 7; file >= 0; file--) {
            int sq = rank * 8 + file;
            uint64 sqr = NCH_SQR(sq);
            char out = NCH_CHKFLG(bb, sqr) ? '1' : '.';
            printf("%c ", out);
        }
        printf("\n");
    }
    printf("\n");
}

int move_nodes_are_equal(const MoveNode* n1, const MoveNode* n2) {
    return !memcmp(&n1->pos_info, &n2->pos_info, sizeof(n1->pos_info))
        && (n1->move == n2->move);
}

int board_nodes_are_equal(const BoardNode* n1, const BoardNode* n2) {
    return !memcmp(n1->bitboards, n2->bitboards, sizeof(n1->bitboards))
        && (n1->count == n2->count)
        && (n1->empty == n2->empty);
}

static int check_dict_recursive(const BoardNode* n1, const BoardNode* n2) {
    if (!n1 && !n2)
        return 1;
    
    if (n1 && n2) {
        if (!board_nodes_are_equal(n1, n2))
            return 0;
        
        if (n1->next)
            return check_dict_recursive(n1->next, n2->next);
        
        return 1;
    }
    
    return 0;
}

int boards_are_equal(const Board* b1, const Board* b2) {
    // Check movelist
    if (b1->movelist.len != b2->movelist.len)
        return 0;
    
    int max_len = NCH_MOVELIST_SIZE > b1->movelist.len ? 
                  b1->movelist.len : NCH_MOVELIST_SIZE;
    
    const MoveNode *mn_1, *mn_2;
    for (int i = 0; i < max_len; i++) {
        mn_1 = &b1->movelist.nodes[i];
        mn_2 = &b2->movelist.nodes[i];
        
        if (!move_nodes_are_equal(mn_1, mn_2))
            return 0;
    }
    
    if (b1->movelist.extra) {
        if (!b2->movelist.extra)
            return 0;
        
        mn_1 = b1->movelist.extra;
        mn_2 = b2->movelist.extra;
        
        while (mn_1 && mn_2) {
            if (!move_nodes_are_equal(mn_1, mn_2))
                return 0;
            
            mn_1 = mn_1->next;
            mn_2 = mn_2->next;
        }
        
        if (mn_1 || mn_2)
            return 0;
    }
    
    // Check dict
    const BoardNode *bn_1, *bn_2;
    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++) {
        bn_1 = &b1->dict.nodes[i];
        bn_2 = &b2->dict.nodes[i];
        
        if (!board_nodes_are_equal(bn_1, bn_2))
            return 0;
        
        if (bn_1->next) {
            if (!check_dict_recursive(bn_1, bn_2))
                return 0;
        }
    }
    
    // Check board state
    int res = (!memcmp(b1->bitboards, b2->bitboards, sizeof(b1->bitboards)))
           && (!memcmp(b1->occupancy, b2->occupancy, sizeof(b1->occupancy)))
           && (!memcmp(b1->piecetables, b2->piecetables, sizeof(b1->piecetables)))
           && (!memcmp(&b1->info, &b2->info, sizeof(PositionInfo)))
           && (b1->nmoves == b2->nmoves);
    
    return res;
}