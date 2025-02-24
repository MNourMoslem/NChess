/*
    magic_utils.c

    This file contains the function definitions for the magic utilities.

    All functions written here by taken from the tutorial of the Chess Programming
    youtube channel.
    https://www.youtube.com/@chessprogramming591
*/

#include "magic_utils.h"

uint64 
set_occupancy(int index, int bits_in_mask, uint64 attack_mask)
{
    uint64 occupancy = 0ULL;
    
    for (int count = 0; count < bits_in_mask; count++)
    {
        uint64 sqr = get_last_bit(attack_mask);
        NCH_RMVFLG(attack_mask, sqr);
        
        if (index & (1 << count))
            occupancy |= sqr;
    }
    
    return occupancy;
}

uint64
get_rook_mask_on_fly(int idx, uint64 block){
    uint64 mask = 0ULL;
    uint64 sqr, temp;

    temp = block | NCH_COL1;
    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_RIGHT(sqr);
        mask |= sqr;
    }

    temp = block | NCH_COL8;
    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_LEFT(sqr);
        mask |= sqr;
    }

    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(block, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_UP(sqr);
        mask |= sqr;
    }

    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(block, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_DOWN(sqr);
        mask |= sqr;
    }

    return mask;
}

uint64
get_bishop_mask_on_fly(int idx, uint64 block){
    uint64 mask = 0ULL;
    uint64 sqr, temp;

    temp = block | NCH_COL1;
    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_UPRIGHT(sqr);
        mask |= sqr;
    }

    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_DOWNRIGHT(sqr);
        mask |= sqr;
    }

    temp = block | NCH_COL8;
    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_UPLEFT(sqr);
        mask |= sqr;
    }

    sqr = NCH_SQR(idx);
    while (1)
    {
        if (NCH_CHKFLG(temp, sqr)){
            break;
        }
        sqr = NCH_NXTSQR_DOWNLEFT(sqr);
        mask |= sqr;
    }

    return mask;
}
