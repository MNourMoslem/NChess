#include "fen.h"
#include <stdlib.h>

NCH_STATIC_INLINE void
char2piece(char c, Side* side, Piece* piece){
    switch (c)
    {
    case 'P':
        *piece = NCH_Pawn;
        *side = NCH_White;
        break;

    case 'N':
        *piece = NCH_Knight;
        *side = NCH_White;
        break;
    case 'B':
        *piece = NCH_Bishop;
        *side = NCH_White;
        break;

    case 'R':
        *piece = NCH_Rook;
        *side = NCH_White;
        break;

    case 'Q':
        *piece = NCH_Queen;
        *side = NCH_White;
        break;

    case 'K':
        *piece = NCH_King;
        *side = NCH_White;
        break;

    case 'p':
        *piece = NCH_Pawn;
        *side = NCH_Black;
        break;

    case 'n':
        *piece = NCH_Knight;
        *side = NCH_Black;
        break;

    case 'b':
        *piece = NCH_Bishop;
        *side = NCH_Black;
        break;

    case 'r':
        *piece = NCH_Rook;
        *side = NCH_Black;
        break;

    case 'q':
        *piece = NCH_Queen;
        *side = NCH_Black;
        break;

    case 'k':
        *piece = NCH_King;
        *side = NCH_Black;
        break;

    default:
        *piece = NCH_NO_PIECE;
        *side = NCH_SIDES_NB;
        break;
    }
}

NCH_STATIC_INLINE int
is_number(char c){
    return c <= '9' && c >= '0';
}

NCH_STATIC_INLINE int
char2number(char c){
    return c - '0';
}

NCH_STATIC_INLINE Square
str2square(char* s){
    return ('h' - s[0]) + (char2number(s[1]) * 8); 
}

NCH_STATIC_INLINE Square
is_valid_square(Square s){
    return s >= NCH_H1 && s < NCH_A8;
}

int
parse_fen(Board* board, char* fen){
    Square sqr = NCH_A8;
    Piece piece;
    Side side;

    while (*fen != ' ')
    {
        if (is_number(*fen)){
            sqr -= char2number(*fen);
        }
        else if (*fen != '/'){
            char2piece(*fen, &side, &piece);
            if (piece != NCH_NO_PIECE){
                board->bitboards[side][piece] |= NCH_SQR(sqr);
                board->piecetables[side][sqr] = piece;
                sqr--;
            }
        }
        fen++;
    }
    for (Side s = 0; s < NCH_SIDES_NB; s++){
        for (Piece p = 0; p < NCH_PIECE_NB; p++){
            board->occupancy[s] |= board->bitboards[s][p];
        }
        board->occupancy[NCH_SIDES_NB] |= board->occupancy[s];  
    }
    fen++;

    if (*fen == 'w'){
        NCH_SETFLG(board->flags, Board_TURN);
    }
    else if (*fen == 'b'){
        NCH_RMVFLG(board->flags, Board_TURN);
    }
    else{
        return -1;
    }

    fen += 2;

    while (*fen != ' ')
    {
        if (*fen == 'K'){
            NCH_SETFLG(board->castles, Board_CASTLE_WK);
        }
        else if (*fen == 'Q'){
            NCH_SETFLG(board->castles, Board_CASTLE_WQ);
        }
        else if (*fen == 'k'){
            NCH_SETFLG(board->castles, Board_CASTLE_BK);
        }
        else if (*fen == 'q'){
            NCH_SETFLG(board->castles, Board_CASTLE_BQ);
        }
        else{
            return -1;
        }

        fen++;
    }
    fen++;

    if (*fen != '-'){
        Side trg_side = Board_IS_WHITETURN(board) ? NCH_Black : NCH_White;
        Square en_passant_idx = str2square(fen);
        if (!is_valid_square(en_passant_idx)){
            return -1;
        }
        board->en_passant_idx = en_passant_idx;
        board->en_passant_map = NCH_SQR(en_passant_idx) | (((NCH_NXTSQR_RIGHT(NCH_SQR(en_passant_idx)) & 0x7f7f7f7f7f7f7f7f)
                                |(NCH_NXTSQR_LEFT(NCH_SQR(en_passant_idx)) & 0xfefefefefefefefe))
                                & board->bitboards[trg_side][NCH_Pawn]);
        board->en_passant_trg = trg_side == NCH_Black ? NCH_NXTSQR_DOWN(NCH_SQR(en_passant_idx))
                                                      : NCH_NXTSQR_UP(NCH_SQR(en_passant_idx));
        fen += 3;
    }
    else{
        fen += 2;
    }

    if (!is_number(*fen)){
        return -1;
    }
    board->fifty_counter = char2number(*fen);
    fen+=2;
    
    if (!is_number(*fen)){
        return -1;
    }
    board->nmoves = char2number(*fen);
    
    return 0;
}

Board*
Board_FromFen(char* fen){
    Board* board = Board_New();
    if (!board){
        return NULL;
    }

    Board_InitEmpty(board);

    int out = parse_fen(board, fen);
    if (out != 0){
        Board_Free(board);
        return NULL;
    }

    Board_Update(board);

    return board;
}