/*
    random.c

    This file containes function definitions of random.h
*/


#include "random.h"

unsigned int state = 1804289383;

unsigned int generate_random_number()
{
	unsigned int x = state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	state = x;
	return x;
}

uint64 random_uint64()
{
    uint64 u1, u2, u3, u4;
    
    u1 = (uint64)(generate_random_number()) & 0xFFFF;
    u2 = (uint64)(generate_random_number()) & 0xFFFF;
    u3 = (uint64)(generate_random_number()) & 0xFFFF;
    u4 = (uint64)(generate_random_number()) & 0xFFFF;
    
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64 random_fewbits() {
    return random_uint64() & random_uint64() & random_uint64();
}