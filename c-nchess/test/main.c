#include "test_copy.c"
#include "test_io.c"
#include "test_bitboards.c"
#include "test_perft.c"

#include "nchess.h"

int main(){
    NCH_Init();

    // test_io_main(0);
    // test_bitboard_main(0);
    // test_perft_main(0, 1);
    test_copy_main(0);

    return 0;
}