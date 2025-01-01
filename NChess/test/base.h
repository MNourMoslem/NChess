#ifndef NCHESS_TEST_BASE_H
#define NCHESS_TEST_BASE_H

#include <stdio.h>
#include <string.h>

typedef int (*testfunc) ();

void test_all(char* testing_header, testfunc* funcs, int num_funcs){

    int len = strlen(testing_header);

    printf("\n\n");
    printf("==============================\n");
    
    int prefix = 15 - len / 2;
    for (int i = 0; i < prefix; i++){
        printf(" ");
    }

    printf(testing_header);
    printf("\n==============================\n");

    int out;
    testfunc current;
    for (int i = 0; i < num_funcs; i++){
        current = funcs[i];
        out = current();
        printf("Test %i: %s\n", i + 1, out ? "Success" : "Fail");
    }

    printf("==============================\n");
}

void print_bb(uint64 bb){
    int i = 63;
    uint64 sqr;
    char out;

    for (int file = 0; file < 8; file++){
        for (int raw = 0; raw < 8; raw++){
            sqr = NCH_SQR(i);
            out = NCH_CHKFLG(bb, sqr) ? '1' : '.';
            printf("%c", out);            
            i--;
        }
        printf("\n");
    }
    printf("\n");
}

#endif 