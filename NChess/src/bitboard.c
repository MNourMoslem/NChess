#include "bitboard.h"
#include "config.h"
#include "types.h"
#include <stdio.h>
#include "magics.h"
#include "magic_utils.h"

uint64 PawnAttacks[2][NCH_SQUARE_NB];
uint64 KnightAttacks[NCH_SQUARE_NB];
uint64 KingAttacks[NCH_SQUARE_NB];

uint64 BetweenTable[NCH_SQUARE_NB][NCH_SQUARE_NB];  // 4,096

uint64 Magics[2][NCH_SQUARE_NB];                    // 128
uint64 SlidersAttackMask[2][NCH_SQUARE_NB];         // 128
int ReleventSquares[2][NCH_SQUARE_NB];              // 128

uint64 RookTable[NCH_SQUARE_NB][4096];              // 262,144
uint64 BishopTable[NCH_SQUARE_NB][512];    

NCH_STATIC void
init_pawn_attacks(){
    uint64 sqr;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        sqr = NCH_SQR(i);
        PawnAttacks[NCH_White][i] = (NCH_NXTSQR_UPRIGHT(sqr) & 0x7F7F7F7F7F7F7F7F)
                                  | (NCH_NXTSQR_UPLEFT(sqr) & 0xFeFeFeFeFeFeFeFe);
    }

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        sqr = NCH_SQR(i);
        PawnAttacks[NCH_Black][i] = (NCH_NXTSQR_DOWNRIGHT(sqr) & 0x7F7F7F7F7F7F7F7F)
                                  | (NCH_NXTSQR_DOWNLEFT(sqr) & 0xFeFeFeFeFeFeFeFe);
    }
}

NCH_STATIC void
init_knight_attacks(){
    uint64 sqr;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        sqr = NCH_SQR(i);
        KnightAttacks[i] = (((NCH_NXTSQR_K_UPLEFT(sqr)
                            | NCH_NXTSQR_K_DOWNLEFT(sqr))
                            & 0xfefefefefefefefe)|

                            ((NCH_NXTSQR_K_LEFTUP(sqr)
                            | NCH_NXTSQR_K_LEFTDOWN(sqr))
                            & 0xfcfcfcfcfcfcfcfc)|

                            ((NCH_NXTSQR_K_UPRIGHT(sqr)
                            | NCH_NXTSQR_K_DOWNRIGHT(sqr))
                            & 0x7f7f7f7f7f7f7f7f)|

                            ((NCH_NXTSQR_K_RIGHTUP(sqr)
                            | NCH_NXTSQR_K_RIGHTDOWN(sqr))
                            & 0x3f3f3f3f3f3f3f3f));
    }
}

NCH_STATIC void
init_king_attacks(){
    uint64 sqr;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        sqr = NCH_SQR(i);
        KingAttacks[i] = ( (NCH_NXTSQR_UPRIGHT(sqr)
                        | NCH_NXTSQR_RIGHT(sqr)
                        | NCH_NXTSQR_DOWNRIGHT(sqr))
                        & 0x7f7f7f7f7f7f7f7f)
                        |( (NCH_NXTSQR_UPLEFT(sqr)
                        | NCH_NXTSQR_LEFT(sqr) 
                        | NCH_NXTSQR_DOWNLEFT(sqr))
                        & 0xfefefefefefefefe)
                        | NCH_NXTSQR_UP(sqr)
                        | NCH_NXTSQR_DOWN(sqr);
    }
}

NCH_STATIC void
init_between_table(){
    uint64 s1, s2, bet;

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        for (int j = 0; j < NCH_SQUARE_NB; j++){
            s1 = NCH_SQR(i);
            s2 = NCH_SQR(j);
            if (s1 == s2){
                bet = s1;
            }
            else if (NCH_SAME_COL(i, j)){
                bet = NCH_GET_COL(i);
                bet &= ~s1; 
            }
            else if (NCH_SAME_ROW(i, j))
            {
                bet = 0xFFFFFFFFFFFFFFFF;
            }
            else if (NCH_SAME_MAIN_DG(i, j))
            {
                bet = NCH_GET_DIGMAIN(i);
            }
            else if (NCH_SAME_ANTI_DG(i, j))
            {
                bet = NCH_GET_DIGANTI(i);
            }
            else{
                bet = 0ull;
            }

            if (s1 > s2){
                bet &= (-s2) & (s1-1);
            }
            else{
                bet &= (-s1) & (s2-1);
            }

            BetweenTable[i][j] = bet | s2;
        }
    }
}

NCH_STATIC void
init_rook_mask(){
    uint64 col, row;
    
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        col = NCH_GET_COL(i) & 0x00FFFFFFFFFFFF00;
        row = NCH_GET_ROW(i) & 0x7e7e7e7e7e7e7e7e;
        
        SlidersAttackMask[NCH_RS][i] = row | col;
        SlidersAttackMask[NCH_RS][i] &= ~NCH_SQR(i);
    }
}

NCH_STATIC void
init_bishop_mask(){
    uint64 main, anti;
    
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        main = NCH_GET_DIGMAIN(i);
        anti = NCH_GET_DIGANTI(i);
        
        SlidersAttackMask[NCH_BS][i] = main | anti;
        SlidersAttackMask[NCH_BS][i] &= 0x007e7e7e7e7e7e00;
        SlidersAttackMask[NCH_BS][i] &= ~NCH_SQR(i);
    }
}

NCH_STATIC void
init_relevant_bits(){
    for (int i = 0; i < 64; i++){
        ReleventSquares[NCH_RS][i] = count_bits(bb_rook_mask(i));
        ReleventSquares[NCH_BS][i] = count_bits(bb_bishop_mask(i));
    }
}

NCH_STATIC void
init_magics(){
    const uint64 rook_magics[64] = {
        0x8a80104000800020ULL,
        0x140002000100040ULL,
        0x2801880a0017001ULL,
        0x100081001000420ULL,
        0x200020010080420ULL,
        0x3001c0002010008ULL,
        0x8480008002000100ULL,
        0x2080088004402900ULL,
        0x800098204000ULL,
        0x2024401000200040ULL,
        0x100802000801000ULL,
        0x120800800801000ULL,
        0x208808088000400ULL,
        0x2802200800400ULL,
        0x2200800100020080ULL,
        0x801000060821100ULL,
        0x80044006422000ULL,
        0x100808020004000ULL,
        0x12108a0010204200ULL,
        0x140848010000802ULL,
        0x481828014002800ULL,
        0x8094004002004100ULL,
        0x4010040010010802ULL,
        0x20008806104ULL,
        0x100400080208000ULL,
        0x2040002120081000ULL,
        0x21200680100081ULL,
        0x20100080080080ULL,
        0x2000a00200410ULL,
        0x20080800400ULL,
        0x80088400100102ULL,
        0x80004600042881ULL,
        0x4040008040800020ULL,
        0x440003000200801ULL,
        0x4200011004500ULL,
        0x188020010100100ULL,
        0x14800401802800ULL,
        0x2080040080800200ULL,
        0x124080204001001ULL,
        0x200046502000484ULL,
        0x480400080088020ULL,
        0x1000422010034000ULL,
        0x30200100110040ULL,
        0x100021010009ULL,
        0x2002080100110004ULL,
        0x202008004008002ULL,
        0x20020004010100ULL,
        0x2048440040820001ULL,
        0x101002200408200ULL,
        0x40802000401080ULL,
        0x4008142004410100ULL,
        0x2060820c0120200ULL,
        0x1001004080100ULL,
        0x20c020080040080ULL,
        0x2935610830022400ULL,
        0x44440041009200ULL,
        0x280001040802101ULL,
        0x2100190040002085ULL,
        0x80c0084100102001ULL,
        0x4024081001000421ULL,
        0x20030a0244872ULL,
        0x12001008414402ULL,
        0x2006104900a0804ULL,
        0x1004081002402ULL,
    };

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Magics[NCH_RS][i] = rook_magics[i];
    }

    const uint64 bishop_magics[64] = {
        0x40040844404084ULL,
        0x2004208a004208ULL,
        0x10190041080202ULL,
        0x108060845042010ULL,
        0x581104180800210ULL,
        0x2112080446200010ULL,
        0x1080820820060210ULL,
        0x3c0808410220200ULL,
        0x4050404440404ULL,
        0x21001420088ULL,
        0x24d0080801082102ULL,
        0x1020a0a020400ULL,
        0x40308200402ULL,
        0x4011002100800ULL,
        0x401484104104005ULL,
        0x801010402020200ULL,
        0x400210c3880100ULL,
        0x404022024108200ULL,
        0x810018200204102ULL,
        0x4002801a02003ULL,
        0x85040820080400ULL,
        0x810102c808880400ULL,
        0xe900410884800ULL,
        0x8002020480840102ULL,
        0x220200865090201ULL,
        0x2010100a02021202ULL,
        0x152048408022401ULL,
        0x20080002081110ULL,
        0x4001001021004000ULL,
        0x800040400a011002ULL,
        0xe4004081011002ULL,
        0x1c004001012080ULL,
        0x8004200962a00220ULL,
        0x8422100208500202ULL,
        0x2000402200300c08ULL,
        0x8646020080080080ULL,
        0x80020a0200100808ULL,
        0x2010004880111000ULL,
        0x623000a080011400ULL,
        0x42008c0340209202ULL,
        0x209188240001000ULL,
        0x400408a884001800ULL,
        0x110400a6080400ULL,
        0x1840060a44020800ULL,
        0x90080104000041ULL,
        0x201011000808101ULL,
        0x1a2208080504f080ULL,
        0x8012020600211212ULL,
        0x500861011240000ULL,
        0x180806108200800ULL,
        0x4000020e01040044ULL,
        0x300000261044000aULL,
        0x802241102020002ULL,
        0x20906061210001ULL,
        0x5a84841004010310ULL,
        0x4010801011c04ULL,
        0xa010109502200ULL,
        0x4a02012000ULL,
        0x500201010098b028ULL,
        0x8040002811040900ULL,
        0x28000010020204ULL,
        0x6000020202d0240ULL,
        0x8918844842082200ULL,
        0x4010011029020020ULL,
    };

    for (int i = 0; i < 64; i++){
        Magics[NCH_BS][i] = bishop_magics[i];
    }
}

NCH_STATIC void
init_sliders_table(){
    uint64 mask, occupancy;
    int rel_bits, occupancy_indicies, magic_index;

    for (int sqr_idx = 0; sqr_idx < NCH_SQUARE_NB; sqr_idx++){
        mask = bb_rook_mask(sqr_idx);
        rel_bits = count_bits(mask);
        occupancy_indicies = 1 << rel_bits;

        for (int index = 0; index < occupancy_indicies; index++){
            occupancy = set_occupancy(index, rel_bits, mask);
            magic_index = (occupancy * bb_rook_magic(sqr_idx)) >> (64 - bb_rook_relevant(sqr_idx));
            RookTable[sqr_idx][magic_index] = get_rook_mask_on_fly(sqr_idx, occupancy);
        }
    }

    for (int sqr_idx = 0; sqr_idx < NCH_SQUARE_NB; sqr_idx++){
        mask = bb_bishop_mask(sqr_idx);
        rel_bits = count_bits(mask);
        occupancy_indicies = 1 << rel_bits;

        for (int index = 0; index < occupancy_indicies; index++){
            occupancy = set_occupancy(index, rel_bits, mask);
            magic_index = (int)((occupancy * bb_bishop_magic(sqr_idx)) >> (64 - rel_bits));
            BishopTable[sqr_idx][magic_index] = get_bishop_mask_on_fly(sqr_idx, occupancy);
        }
    }
}


void
InitBitboards(){
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_between_table();
    init_rook_mask();
    init_bishop_mask();
    init_relevant_bits();
    init_magics();
    init_sliders_table();
}