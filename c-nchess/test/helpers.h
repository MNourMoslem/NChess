#ifndef NCHESS_TEST_HELPERS_H
#define NCHESS_TEST_HELPERS_H

#include <stdio.h>
#include <stdint.h>
#include "nchess.h"

// Helper macros for assertions
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("  ASSERTION FAILED: %s\n", #condition); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

#define ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("  ASSERTION FAILED: Expected %lld, got %lld\n", \
                   (long long)(expected), (long long)(actual)); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

#define ASSERT_STR_EQ(actual, expected) \
    do { \
        if (strcmp((actual), (expected)) != 0) { \
            printf("  ASSERTION FAILED: Expected '%s', got '%s'\n", \
                   (expected), (actual)); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("  ASSERTION FAILED: Expected non-NULL pointer\n"); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

#define ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("  ASSERTION FAILED: Expected NULL pointer\n"); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

// Helper function to print bitboard (for debugging)
void print_bitboard(uint64 bb);

// Helper function to compare boards
int boards_are_equal(const Board* b1, const Board* b2);

// Helper function to compare move nodes
int move_nodes_are_equal(const MoveNode* n1, const MoveNode* n2);

// Helper function to compare board dict nodes
int board_nodes_are_equal(const BoardNode* n1, const BoardNode* n2);

#endif // NCHESS_TEST_HELPERS_H
