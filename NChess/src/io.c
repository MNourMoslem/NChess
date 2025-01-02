#include "board.h" 
#include "stdio.h"
#include "loops.h"


const char NCH_PIECES[13] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k', '.'};
const char NCH_COLUMNS[8] = {'h' ,'g', 'f', 'e', 'd', 'c', 'b', 'a'};

char* squares_char[] = {
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", 
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2", 
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", 
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4", 
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5", 
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6", 
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7", 
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};

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

void
Board_PrintMoves(Board* board){
    char* src;
    int idx;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        if (board->moves[i]){
           src = squares_char[i];
           LOOP_U64_T(board->moves[i]){
                printf("%s%s ", src, squares_char[idx]);
           }
        }
    }
    printf("\n");
}

void
Board_PrintInfo(Board *board){
    printf("=========\n  Board  \n=========\n");
    Board_Print(board);
    printf("=========\n  Moves  \n=========\n");
    Board_PrintMoves(board);
    printf("\n");
    printf("=========\n  Info  \n=========\n");
    printf("Turn: %s\n", Board_IS_WHITETURN(board) ? "White" : "Black");
    printf("caslte rights: %c%c%c%c\n"  , Board_IS_CASTLE_WK(board) ? 'K' : '-'
                                        , Board_IS_CASTLE_WQ(board) ? 'Q' : '-'
                                        , Board_IS_CASTLE_BK(board) ? 'k' : '-'
                                        , Board_IS_CASTLE_BQ(board) ? 'q' : '-');
    printf("moves number: %i\n", board->nmoves);
    printf("fifty counter: %i\n", board->fifty_counter);
}   