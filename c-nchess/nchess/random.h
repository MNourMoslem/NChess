/*
    random.h

    This file containes functions for generating random numbers.
    Used by magic files.
*/


#ifndef NCHESS_SRC_RANDOM_H
#define NCHESS_SRC_RANDOM_H

#include "types.h"

unsigned int generate_random_number();
uint64 random_uint64();
uint64 random_fewbits();

#endif