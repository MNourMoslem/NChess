/*
    magic_utils.h

    This file contains the function definitions for the magic utilities.
*/

#ifndef NCHESS_SRC_MAGICS_UTILS_H
#define NCHESS_SRC_MAGICS_UTILS_H

#include "core.h"
#include "types.h"
#include "config.h"

uint64 get_rook_mask_on_fly(int idx, uint64 block);
uint64 get_bishop_mask_on_fly(int idx, uint64 block);
uint64 set_occupancy(int index, int bits_in_mask, uint64 attack_mask);

#endif