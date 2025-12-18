#ifndef NCHESS_TEST_MAIN_H
#define NCHESS_TEST_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nchess.h"

// Test function pointer type
typedef int (*TestFunc)(void);

// Test suite structure
typedef struct {
    const char* name;
    TestFunc* tests;
    int test_count;
} TestSuite;

// Test result tracking
typedef struct {
    int total;
    int passed;
    int failed;
} TestResults;

// Function declarations for test runners
void run_test_suite(const char* suite_name, TestFunc* tests, int test_count, TestResults* results);
void print_test_results(const char* suite_name, TestResults* results);
void print_final_results(TestResults* results);

// External test suite declarations
void test_bitboard_suite(TestResults* results);
void test_board_suite(TestResults* results);
void test_copy_suite(TestResults* results);
void test_fen_suite(TestResults* results);
void test_generate_suite(TestResults* results);
void test_hash_suite(TestResults* results);
void test_io_suite(TestResults* results);
void test_move_suite(TestResults* results);
void test_perft_suite(TestResults* results);

#endif // NCHESS_TEST_MAIN_H
