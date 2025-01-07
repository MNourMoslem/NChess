#include "test_io.c"
#include "test_bitboards.c"
#include "test_perft.c"

#include "nchess.h"

int main(){
    InitBitboards();

    // test_io_main(0);
    // test_bitboard_main(0);
    test_perft_main(0, 1);

    return 0;
}