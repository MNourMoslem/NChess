#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "NChess/src/include/types.h"
#include "NChess/src/include/core.h"
#include "NChess/src/include/scan.h"

int main() {
    uint64_t map = 0x0000001000001010ull | NCH_BOARDER;
    int idx = 41;
    uint64_t sqr = NCH_SQR(idx);
    uint64_t main_dg = NCH_GETDIGMAIN(idx);
    uint64_t anti_dg = NCH_GETDIGANTI(idx);

    char board[NCH_B_STRING_SIZE];

    int k = 63;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            board[9 * i + j] = NCH_CHKUNI(map, NCH_SQR(k)) ? '1' : '.';
            k--;
        }
        board[9 * (i+1) - 1] = '\n';
    }
    board[(63 - idx) + (63 - idx) / 9] = '0';
    board[72] = '\0';

    printf("%s\n", board);

    uint64_t up = NCH_SCAN_UP(map, sqr, idx);
    printf("Up: 0x%16llx\n", up);
    uint64_t down = NCH_SCAN_DOWN(map, sqr, idx);
    printf("Down: 0x%16llx\n", down);
    uint64_t right = NCH_SCAN_RIGHT(map, sqr, idx);
    printf("Right: 0x%16llx\n", right);
    uint64_t left = NCH_SCAN_LEFT(map, sqr, idx);
    printf("Left: 0x%16llx\n", left);

    printf("\n");

    uint64_t up_left = NCH_SCAN_UP_LEFT(map, sqr, idx);
    printf("Up Left: 0x%16llx\n", up_left);

    uint64_t up_right = NCH_SCAN_UP_RIGHT(map, sqr, idx);
    printf("Up Right: 0x%16llx\n", up_right);

    uint64_t down_left = NCH_SCAN_DOWN_LEFT(map, sqr);
    printf("Down Left: 0x%16llx\n", down_left);

    uint64_t down_right = NCH_SCAN_DOWN_RIGHT(map, sqr);
    printf("Down Right: 0x%16llx\n", down_right);

    printf("something: 0x%16llx\n", (map >> idx) & 0x0102040810204080);

    return 0;
}
