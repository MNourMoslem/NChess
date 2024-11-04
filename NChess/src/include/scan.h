#ifndef NCHESS_SCAN_H
#define NCHESS_SCAN_H

#include "core.h"
#include "types.h"

#define NCH_SCAN_RIGHT(block_map, sqr, idx) (~(NCH_CUINT64_MAX >> NCH_CLZLL(block_map << NCH_CLZLL(sqr))) >> NCH_CLZLL(sqr) >> 1)
#define NCH_SCAN_LEFT(block_map, sqr, idx) (~(NCH_CUINT64_MAX << NCH_CTZLL(block_map >> idx)) << idx << 1)
#define NCH_SCAN_DOWN(block_map, sqr, idx) ((~(NCH_CUINT64_MAX >> NCH_CLZLL(block_map << NCH_CLZLL(sqr) & NCH_COL8)) & NCH_COL8) >> NCH_CLZLL(sqr) >> 8)
#define NCH_SCAN_UP(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL((block_map >> idx) & NCH_COL1)) & NCH_COL1) << idx << 8)

#define NCH_SCAN_UP_RIGHT(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL((block_map >> idx) & 0x0102040810204080) << 1) & 0x0102040810204080) << idx)
#define NCH_SCAN_UP_LEFT(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL((block_map >> idx) & 0x8040201008040200) << 1) & 0x8040201008040200) << idx)
#define NCH_SCAN_DOWN_RIGHT(block_map, sqr) ((~(NCH_CUINT64_MAX >> NCH_CLZLL((block_map << NCH_CLZLL(sqr)) & 0x0040201008040201) >> 1) & 0x0040201008040201) >> NCH_CLZLL(sqr))
#define NCH_SCAN_DOWN_LEFT(block_map, sqr) ((~(NCH_CUINT64_MAX >> NCH_CLZLL((block_map << NCH_CLZLL(sqr)) & 0x0102040810204080) >> 1) & 0x0102040810204080) >> NCH_CLZLL(sqr))

#define NCH_SCAN_PAWNATTACK_LIKE_W(sqr) ((NCH_NXTSQR_UPRIGHT(sqr) & 0x7f7f7f7f7f7f7f7f)\
                                        |(NCH_NXTSQR_UPLEFT(sqr) & 0xfefefefefefefefe))

#define NCH_SCAN_PAWNATTACK_LIKE_B(sqr) ((NCH_NXTSQR_DOWNRIGHT(sqr) & 0x7f7f7f7f7f7f7f7f)\
                                        |(NCH_NXTSQR_DOWNLEFT(sqr) & 0xfefefefefefefefe))

#define NCH_SCAN_KNIGHT_LIKE(sqr)  (((NCH_NXTSQR_K_UPLEFT(sqr)\
                                    | NCH_NXTSQR_K_DOWNLEFT(sqr))\
                                    & 0xfefefefefefefefe)|\
\
                                    ((NCH_NXTSQR_K_LEFTUP(sqr)\
                                    | NCH_NXTSQR_K_LEFTDOWN(sqr))\
                                    & 0xfcfcfcfcfcfcfcfc)|\
\
                                    ((NCH_NXTSQR_K_UPRIGHT(sqr)\
                                    | NCH_NXTSQR_K_DOWNRIGHT(sqr))\
                                    & 0x7f7f7f7f7f7f7f7f)|\
\
                                    ((NCH_NXTSQR_K_RIGHTUP(sqr)\
                                    | NCH_NXTSQR_K_RIGHTDOWN(sqr))\
                                    & 0x3f3f3f3f3f3f3f3f))

#define NCH_SCAN_ROOK_LIKE(block_map, sqr, idx) ( NCH_SCAN_UP(block_map, sqr, idx)\
                                                | NCH_SCAN_LEFT(block_map, sqr, idx)\
                                                | NCH_SCAN_DOWN(block_map, sqr, idx)\
                                                | NCH_SCAN_RIGHT(block_map, sqr, idx))


#define NCH_SCAN_BISHOP_LIKE(block_map, sqr, idx) ( NCH_SCAN_UP_RIGHT(block_map, sqr, idx)\
                                                  | NCH_SCAN_UP_LEFT(block_map, sqr, idx)\
                                                  | NCH_SCAN_DOWN_RIGHT(block_map, sqr)\
                                                  | NCH_SCAN_DOWN_LEFT(block_map, sqr))


#define NCH_SCAN_QUEEN_LIKE(block_map, sqr, idx) (NCH_SCAN_ROOK_LIKE(block_map, sqr, idx) | NCH_SCAN_BISHOP_LIKE(block_map, sqr, idx))

#define NCH_SCAN_KING_LIKE(sqr) (( NCH_NXTSQR_UPRIGHT(sqr)\
                                 | NCH_NXTSQR_RIGHT(sqr)\
                                 | NCH_NXTSQR_DOWNRIGHT(sqr)\
                                 & 0x7878787878787878)\
                                 |(NCH_NXTSQR_UPLEFT(sqr)\
                                 | NCH_NXTSQR_LEFT(sqr) \
                                 | NCH_NXTSQR_DOWNLEFT(sqr)\
                                 & 0x8e8e8e8e8e8e8e8e)\
                                 | NCH_NXTSQR_UP(sqr)\
                                 | NCH_NXTSQR_DOWN(sqr))

#endif