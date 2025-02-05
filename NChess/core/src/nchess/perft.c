#include "perft.h"
#include "makemove.h"
#include "generate.h"
#include "io.h"
#include "move.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to format number with commas
NCH_STATIC_INLINE void 
format_number_with_commas(long long num, char* output) {
    char buffer[30];
    sprintf(buffer, "%lld", num);
    int len = strlen(buffer);
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

long long
preft_recursive(Board* board, int depth){
    if (depth <= 1){
        if (depth == 1){
            Move moves[256];
            return Board_GenerateLegalMoves(board, moves);
        }
        return 1;
    }
    
    long long count = 0;
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        count += preft_recursive(board, depth - 1);
        Board_Undo(board);
    }

    return count;
}

long long
Board_Perft(Board* board, int depth){
    if (depth < 1){
        return 0;
    }

    clock_t start_time = clock();

    long long total = 0, count;
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);

    char move_str[6];
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        count = preft_recursive(board, depth - 1);
        Board_Undo(board);
        Move_AsString(moves[i], move_str);
        printf("%s: %lld\n", move_str, count);
        total += count;
    }

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Total: %lld | Time spent: %f seconds\n", total, time_spent);

    return total;
}

long long
Board_PerftPretty(Board* board, int depth){
    if (depth < 1){
        return 0;
    }

    clock_t start_time = clock(); // Start the timer

    long long total = 0, count;
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);

    char move_str[6];
    char formatted_total[30];
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        
        count = preft_recursive(board, depth - 1);

        Board_Undo(board);
        format_number_with_commas(count, formatted_total);

        Move_AsString(moves[i], move_str);
        printf("%s: %s\n", move_str, formatted_total);
        total += count;
    }

    clock_t end_time = clock(); // End the timer
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC; // Calculate the time spent

    format_number_with_commas(total, formatted_total);
    printf("Total: %s | Time spent: %f seconds\n", formatted_total, time_spent); // Print the time spent

    return total;
}

long long
Board_PerftNoPrint(Board* board, int depth){
    if (depth < 1){
        return 0;
    }

    long long total = 0;
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board, moves);
    for (int i = 0; i < nmoves; i++){
        _Board_MakeMove(board, moves[i]);
        total += preft_recursive(board, depth - 1);
        Board_Undo(board);
    }

    return total;
}