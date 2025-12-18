/*
    perft.c

    This file contains the functions for running Perft (performance test) calculations.
*/

#include "perft.h"
#include "makemove.h"
#include "generate.h"
#include "io.h"
#include "move.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Format number with commas (e.g., 1000000 -> "1,000,000")
NCH_STATIC_INLINE void 
format_number_with_commas(long long num, char* output) {
    char buffer[30];
    sprintf(buffer, "%lld", num);
    int len = (int)strlen(buffer);
    int comma_count = (len - 1) / 3;
    int new_len = len + comma_count;
    output[new_len] = '\0';
    int j = new_len - 1;
    int comma_counter = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (comma_counter == 3) {
            output[j--] = ',';
            comma_counter = 0;
        }
        output[j--] = buffer[i];
        comma_counter++;
    }
}

// Format count based on pretty flag
NCH_STATIC_INLINE void
format_count(long long count, char* output, int pretty) {
    if (pretty) {
        format_number_with_commas(count, output);
    } else {
        sprintf(output, "%lld", count);
    }
}

// Recursive perft calculation
long long
preft_recursive(Board* board, int depth){
    if (depth < 1) return 1;

    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    if (depth == 1) return nmoves;
    
    long long count = 0;
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        count += preft_recursive(board, depth - 1);
        Board_Undo(board);
    }

    return count;
}

// Core perft implementation
NCH_STATIC_INLINE long long
perft_core(Board* board, int depth, char* buffer, size_t buffer_size, int pretty, int print_output) {
    if (depth < 1) {
        if (buffer) snprintf(buffer, buffer_size, "Depth must be at least 1\n");
        return 0;
    }

    clock_t start_time = clock();
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    long long total = 0;
    char move_str[6];
    char formatted_count[30];
    char line_buffer[100];
    size_t current_pos = 0;

    if (buffer) buffer[0] = '\0';

    // Process each move
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        long long count = preft_recursive(board, depth - 1);
        Board_Undo(board);
        
        total += count;

        if (print_output || buffer) {
            Move_AsString(moves[i], move_str);
            format_count(count, formatted_count, pretty);
            snprintf(line_buffer, sizeof(line_buffer), "%s: %s\n", move_str, formatted_count);
            
            if (print_output) {
                printf("%s", line_buffer);
            }
            if (buffer) {
                size_t line_len = strlen(line_buffer);
                if (current_pos + line_len < buffer_size - 1) {
                    strcat(buffer, line_buffer);
                    current_pos += line_len;
                }
            }
        }
    }

    // Format and output summary
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (print_output || buffer) {
        format_count(total, formatted_count, pretty);
        snprintf(line_buffer, sizeof(line_buffer), "Total: %s | Time spent: %f seconds\n", 
                 formatted_count, time_spent);
        
        if (print_output) {
            printf("%s", line_buffer);
        }
        if (buffer) {
            size_t line_len = strlen(line_buffer);
            if (current_pos + line_len < buffer_size - 1) {
                strcat(buffer, line_buffer);
            }
        }
    }

    return total;
}

long long
Board_Perft(Board* board, int depth) {
    return perft_core(board, depth, NULL, 0, 0, 1);
}

long long
Board_PerftPretty(Board* board, int depth) {
    return perft_core(board, depth, NULL, 0, 1, 1);
}

long long
Board_PerftNoPrint(Board* board, int depth) {
    if (depth < 1) return 0;
    
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    long long total = 0;
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        total += preft_recursive(board, depth - 1);
        Board_Undo(board);
    }
    
    return total;
}

long long
Board_PerftAsString(Board* board, int depth, char* buffer, size_t buffer_size, int pretty) {
    return perft_core(board, depth, buffer, buffer_size, pretty, 0);
}