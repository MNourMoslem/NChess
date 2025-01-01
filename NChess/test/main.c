#include "test_io.c"
#include "test_bitboards.c"

#include "nchess.h"

int main(){
    InitBitboards();

    // test_io_main(0);
    test_bitboard_main(0);
}