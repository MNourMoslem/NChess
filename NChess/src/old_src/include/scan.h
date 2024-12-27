#ifndef NCHESS_SRC_INCLUDE_SCAN_H
#define NCHESS_SRC_INCLUDE_SCAN_H

#include "core.h"
#include "types.h"

#define NCH_SCAN_RIGHT(block_map, sqr, idx) (~(NCH_CUINT64_MAX >> NCH_CLZLL((block_map | NCH_COL1) << NCH_CLZLL(sqr))) >> NCH_CLZLL(sqr) >> 1)
#define NCH_SCAN_LEFT(block_map, sqr, idx) (~(NCH_CUINT64_MAX << NCH_CTZLL(((block_map | NCH_COL8) >> idx))) << idx << 1)
#define NCH_SCAN_DOWN(block_map, sqr, idx) ((~(NCH_CUINT64_MAX >> NCH_CLZLL(((block_map | NCH_ROW1) << NCH_CLZLL(sqr)) & NCH_COL8)) & NCH_COL8) >> NCH_CLZLL(sqr) >> 8)
#define NCH_SCAN_UP(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL(((block_map | NCH_ROW8) >> idx) & NCH_COL1)) & NCH_COL1) << idx << 8)

#define NCH_SCAN_UP_RIGHT(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL(((block_map | 0xff01010101010101) >> idx << 7) & 0x0102040810204080) << 1) & 0x0102040810204000) >> 7 << idx)
#define NCH_SCAN_UP_LEFT(block_map, sqr, idx) ((~(NCH_CUINT64_MAX << NCH_CTZLL(((block_map | 0xff80808080808080) >> idx) & 0x8040201008040201) << 1) & 0x8040201008040200) << idx)
#define NCH_SCAN_DOWN_RIGHT(block_map, sqr) ((~(NCH_CUINT64_MAX >> NCH_CLZLL(((block_map | 0x01010101010101ff) << NCH_CLZLL(sqr)) & 0x8040201008040201) >> 1) & 0x0040201008040201) >> NCH_CLZLL(sqr))
#define NCH_SCAN_DOWN_LEFT(block_map, sqr) ((~(NCH_CUINT64_MAX >> NCH_CLZLL(((block_map | 0x80808080808080ff) << NCH_CLZLL(sqr) >> 7) & 0x0102040810204080) >> 1) & 0x0002040810204080) << 7 >> NCH_CLZLL(sqr))

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
                                 & 0x7f7f7f7f7f7f7f7f)\
                                 |(NCH_NXTSQR_UPLEFT(sqr)\
                                 | NCH_NXTSQR_LEFT(sqr) \
                                 | NCH_NXTSQR_DOWNLEFT(sqr)\
                                 & 0xfefefefefefefefe)\
                                 | NCH_NXTSQR_UP(sqr)\
                                 | NCH_NXTSQR_DOWN(sqr))

#endif