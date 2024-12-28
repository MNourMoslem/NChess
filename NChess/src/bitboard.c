#include "bitboard.h"
#include "config.h"
#include "types.h"
#include <stdio.h>


uint64 PawnAttacks[2][NCH_SQUARE_NB];
uint64 KnightAttacks[NCH_SQUARE_NB];
uint64 KingAttacks[NCH_SQUARE_NB];

uint64 BetweenTable[NCH_SQUARE_NB][NCH_SQUARE_NB];  // 4,096

uint64 Magics[2][NCH_SQUARE_NB];                    // 128
uint64 RelativeSquares[2][NCH_SQUARE_NB];           // 128
uint64 SlidersAttackMask[2][NCH_SQUARE_NB];         // 128

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
                bet = NCH_COL1 << NCH_GETCOL(i);
                bet &= ~s1; 
            }
            else if (NCH_SAME_ROW(i, j))
            {
                bet = 0xFFFFFFFFFFFFFFFF;
            }
            else if (NCH_SAME_MAIN_DG(i, j))
            {
                bet = NCH_GETDIGMAIN(i);
            }
            else if (NCH_SAME_ANTI_DG(i, j))
            {
                bet = NCH_GETDIGANTI(i);
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
        col = NCH_COL1 << NCH_GETCOL(i);
        row = NCH_ROW1 << (NCH_GETROW(i) * 8);
        
        SlidersAttackMask[NCH_RS][i] = row | col;
        SlidersAttackMask[NCH_RS][i] &= 0x007e7e7e7e7e7e00;
    }
}

NCH_STATIC void
init_bishop_mask(){
    uint64 main, anti;
    
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        main = NCH_GETDIGMAIN(i);
        anti = NCH_GETDIGANTI(i);
        
        SlidersAttackMask[NCH_BS][i] = main | anti;
        SlidersAttackMask[NCH_BS][i] &= 0x007e7e7e7e7e7e00;
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
}