/*
    core.c

    This file contains all the table and functions definitions of the core.h

    All tables valuse written manually except for diraction table.
    This would be changed in the future to be generated by a script to make 
    it more dynamic and easier to change.
*/

#include "core.h"

const int NCH_ROW_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,
};

const int NCH_COL_TABLE[64] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
};

// main diagonal starts from h1 to g1-h2 to f1-h3 and so on
const uint64 NCH_DIAGONAL_MAIN[15] = {
    0x0000000000000001ull,
    0x0000000000000101ull,
    0x0000000000010204ull,
    0x0000000001020408ull,
    0x0000000102040810ull,
    0x0000010204081020ull,
    0x0001020408102040ull,
    0x0102040810204080ull,
    0x0204081020408000ull,
    0x0408102040800000ull,
    0x0810204080000000ull,
    0x1020408000000000ull,
    0x2040800000000000ull,
    0x4080000000000000ull,
    0x8000000000000000ull,
};

const int NCH_DIAGONAL_MAIN_IDX[64] = {
    0, 1, 2,  3,  4,  5,  6,  7,
    1, 2, 3,  4,  5,  6,  7,  8,
    2, 3, 4,  5,  6,  7,  8,  9,
    3, 4, 5,  6,  7,  8,  9, 10,
    4, 5, 6,  7,  8,  9, 10, 11,
    5, 6, 7,  8,  9, 10, 11, 12,
    6, 7, 8,  9, 10, 11, 12, 13,
    7, 8, 9, 10, 11, 12, 13, 14,
};

// anti diagonal starts from a1 to b1-a2 to c1-a3 and so on
const uint64 NCH_DIAGONAL_ANTI[15] = {
    0x0000000000000080ull,
    0x0000000000008040ull,
    0x0000000000804020ull,
    0x0000000080402010ull,
    0x0000008040201008ull,
    0x0000804020100804ull,
    0x0080402010080402ull,
    0x8040201008040201ull,
    0x4020100804020100ull,
    0x2010080402010000ull,
    0x1008040201000000ull,
    0x0804020100000000ull,
    0x0402010000000000ull,
    0x0201000000000000ull,
    0x0100000000000000ull,
};

const int NCH_DIAGONAL_ANTI_IDX[64] = {
     7,  6,  5,  4,  3, 2, 1, 0,
     8,  7,  6,  5,  4, 3, 2, 1,
     9,  8,  7,  6,  5, 4, 3, 2,
    10,  9,  8,  7,  6, 5, 4, 3,
    11, 10,  9,  8,  7, 6, 5, 4,
    12, 11, 10,  9,  8, 7, 6, 5,
    13, 12, 11, 10,  9, 8, 7, 6,
    14, 13, 12, 11, 10, 9, 8, 7,
};

Diractions NCH_DIRACTION_TABLE[NCH_SQUARE_NB][NCH_SQUARE_NB];

NCH_STATIC void
init_diractions(){
    int cur;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        for (int j = 0; j < NCH_SQUARE_NB; j++)
            NCH_DIRACTION_TABLE[i][j] = NCH_NO_DIR;

        cur = i;
        while (cur < 56)
        {
            cur += 8;
            NCH_DIRACTION_TABLE[i][cur] = NCH_Up;
        }
        
        cur = i;
        while (cur > 7)
        {
            cur -= 8;
            NCH_DIRACTION_TABLE[i][cur] = NCH_Down;
        }
        
        cur = i;
        while (NCH_GET_COLIDX(cur) < 7)
        {
            cur += 1;
            NCH_DIRACTION_TABLE[i][cur] = NCH_Left;
        }
        
        cur = i;
        while (NCH_GET_COLIDX(cur) > 0)
        {
            cur -= 1;
            NCH_DIRACTION_TABLE[i][cur] = NCH_Right;
        }

        cur = i;
        while (NCH_GET_COLIDX(cur) > 0 && NCH_GET_ROWIDX(cur) < 7)
        {
            cur += 7;
            NCH_DIRACTION_TABLE[i][cur] = NCH_UpRight;
        }

        cur = i;
        while (NCH_GET_COLIDX(cur) < 7 && NCH_GET_ROWIDX(cur) < 7)
        {
            cur += 9;
            NCH_DIRACTION_TABLE[i][cur] = NCH_UpLeft;
        }

        cur = i;
        while (NCH_GET_COLIDX(cur) > 0 && NCH_GET_ROWIDX(cur) > 0)
        {
            cur -= 9;
            NCH_DIRACTION_TABLE[i][cur] = NCH_DownRight;
        }

        cur = i;
        while (NCH_GET_COLIDX(cur) < 7 && NCH_GET_ROWIDX(cur) > 0)
        {
            cur -= 7;
            NCH_DIRACTION_TABLE[i][cur] = NCH_DownLeft;
        }
    }
}

void
NCH_InitTables(){
    init_diractions();
}
