#include "board.h" 
#include "stdio.h"


const char NCH_PIECES[13] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
const char NCH_COLUMNS[8] = {'h' ,'g', 'f', 'e', 'd', 'c', 'b', 'a'};

NCH_STATIC_INLINE int
match_square_with_map(Board* board, uint64 sqr, Side* side, Piece* ptype){
    if (NCH_CHKFLG(Board_WHITE_OCC(board), sqr)){
        *side = NCH_White;
        if (NCH_CHKFLG(Board_WHITE_PAWNS(board), sqr)){
            *ptype = NCH_Pawn;
        }
        else if (NCH_CHKFLG(Board_WHITE_KNIGHTS(board), sqr)){
            *ptype = NCH_Knight;
        }
        else if (NCH_CHKFLG(Board_WHITE_BISHOPS(board), sqr)){
            *ptype = NCH_Bishop;
        }
        else if (NCH_CHKFLG(Board_WHITE_ROOKS(board), sqr)){
            *ptype = NCH_Rook;
        }
        else if (NCH_CHKFLG(Board_WHITE_QUEENS(board), sqr)){
            *ptype = NCH_Queen;
        }
        else if (NCH_CHKFLG(Board_WHITE_KING(board), sqr)){
            *ptype = NCH_King;
        }
        else{
            return 0;
        }
    } else if (NCH_CHKFLG(Board_BLACK_OCC(board), sqr)){
        *side = NCH_Black;
        if (NCH_CHKFLG(Board_BLACK_PAWNS(board), sqr)){
            *ptype = NCH_Pawn;
        }
        else if (NCH_CHKFLG(Board_BLACK_KNIGHTS(board), sqr)){
            *ptype = NCH_Knight;
        }
        else if (NCH_CHKFLG(Board_BLACK_BISHOPS(board), sqr)){
            *ptype = NCH_Bishop;
        }
        else if (NCH_CHKFLG(Board_BLACK_ROOKS(board), sqr)){
            *ptype = NCH_Rook;
        }
        else if (NCH_CHKFLG(Board_BLACK_QUEENS(board), sqr)){
            *ptype = NCH_Queen;
        }
        else if (NCH_CHKFLG(Board_BLACK_KING(board), sqr)){
            *ptype = NCH_King;
        }
        else{
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

void
Board_Print(Board* board){
    int i = 63;
    uint64 current;
    uint64 occupancy = Board_ALL_OCC(board);
    
    Side side;
    Piece ptype;
    int out;
    char c;

    for (int file = 0; file < 8; file++){
        for (int raw = 0; raw < 8; raw++){
            current = NCH_SQR(i);
            out = 0;
            
            if (NCH_CHKFLG(occupancy, current)){
                out = match_square_with_map(board, current, &side, &ptype);
            }

            c = out ? NCH_PIECES[side * NCH_PIECE_NB + ptype] : '.';
            printf("%c", c);            
            i--;
        }
        printf("\n");
    }
}