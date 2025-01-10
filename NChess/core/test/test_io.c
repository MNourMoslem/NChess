#include "nchess.h"
#include "./base.h"

NCH_STATIC_INLINE int
test_io_1(){
    Board board;
    Board_Init(&board);
    Board_Print(&board);
    return 1;
}

void test_io_main(int init_bb){
    if (init_bb)
        InitBitboards();

    testfunc funcs[] = {
        test_io_1
    };

    test_all("IO", funcs, 1);
}