#include "test_copy.c"
#include "test_io.c"
#include "test_bitboards.c"
#include "test_perft.c"
#include "test_fen.c"

#include "nchess.h"

int main(){
    NCH_Init();
    int init = 0;

    // test_io_main(0);
    test_bitboard_main(init);
    test_perft_main(init, 1);
    test_copy_main(init);
    test_fen_main(init);

    return 0;
}