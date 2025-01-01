#include "./base.h"
#include "nchess.h"
#include "magic_utils.h"
#include "stdio.h"
#include "magics.h"
#include "loops.h"

NCH_STATIC_INLINE int
test_bitboard_1(){
    Board board;
    Board_Init(&board);
    // Board_PrintMoves(&board);
    Board_Step(&board, "e2e4");
    Board_Step(&board, "e7e5");
    Board_Step(&board, "f1b5");
    Board_Step(&board, "f7f5");
    Board_Step(&board, "d1e2");
    Board_Step(&board, "f8c5");
    Board_Step(&board, "c2c3");
    Board_Step(&board, "g8h6");
    Board_Step(&board, "e4f5");
    Board_Step(&board, "g7g5");
    Board_Step(&board, "f5g6");
    Board_Step(&board, "e8g8");
    Board_Step(&board, "g6g7");
    Board_Step(&board, "a7a5");
    Board_Step(&board, "g7f8q");
    Board_PrintInfo(&board);

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        printf("%d ", Board_WHITE_PIECE(&board, i));
    }
    printf("\n");

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        printf("%d ", Board_BLACK_PIECE(&board, i));
    }
    printf("\n");

    // printf("0x%llx\n",  board.moves[NCH_E2]);
    return 1;
}

void test_bitboard_main(int init_bb){
    if (init_bb)
        InitBitboards();

    testfunc funcs[] = {
        test_bitboard_1
    };

    test_all("BitBorad", funcs, 1);
}