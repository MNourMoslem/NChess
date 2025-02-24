/*
    magic.c

    This file containes defentions of magic.h functions
*/

#include "magics.h"
#include "loops.h"
#include "core.h"
#include "./random.h"
#include "bitboard.h"
#include "magic_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NCH_STATIC uint64
find_magic(int idx, int relevant_bits, int bishop){
    uint64 occupancies[4096];
    uint64 attacks[4096];
    uint64 used_attacks[4096];

    int occupancy_variations = 1 << relevant_bits;

    uint64 mask = bishop ? bb_bishop_mask(idx) :
                           bb_rook_mask(idx);

    for (int i = 0; i < occupancy_variations; i++){
        occupancies[i] = set_occupancy(i, relevant_bits, mask);
        attacks[i] = bishop ? get_bishop_mask_on_fly(idx, occupancies[i]) :
                              get_rook_mask_on_fly(idx, occupancies[i]);
    }

    uint64 magic;
    for (int random_count = 0; random_count < 100000000; random_count++){
        magic = random_fewbits();

        if(count_bits((mask * magic) & 0xFF00000000000000ULL) < 6){
            continue;
        }

        memset(used_attacks, 0ULL, sizeof(used_attacks));

        int test_count, fail, magic_index;

        for (test_count = 0, fail = 0; !fail && test_count < occupancy_variations; test_count++){
            magic_index = (int)((occupancies[test_count] * magic) >> (64 - relevant_bits));
          
            if(used_attacks[magic_index] == 0ULL){
                used_attacks[magic_index] = attacks[test_count];
            }
            else if(used_attacks[magic_index] != attacks[test_count]){
                fail = 1;  
            }
        }

        if (!fail){
            return magic;
        }
    }

    return 0ULL;
}

void find_all_magic_numbers()
{
  printf("const uint64 rook_magics[64] = {\n");
  
  for(int i = 0; i < 64; i++)
      printf("    0x%llxULL,\n", find_magic(i, bb_rook_relevant(i), 0));
  
  printf("};\n\nconst uint64 bishop_magics[64] = {\n");
  
  for(int i = 0; i < 64; i++)
      printf("    0x%llxULL,\n", find_magic(i, bb_bishop_relevant(i), 1));
  
  printf("};\n\n");
}


