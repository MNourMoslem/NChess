#include "board.h"
#include "config.h"
#include "io.h"
#include "bitboard.h"
#include "generate_utils.h"
#include "generate.h"
#include <stdlib.h>
#include <string.h>
#include "move.h"
#include <stdio.h>


NCH_STATIC_INLINE void
flip_turn(Board* board){
    NCH_FLPFLG(board->flags, Board_TURN);
}

NCH_STATIC_INLINE void
reset_state_flags(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK);
}

NCH_STATIC_INLINE void
reset_castle_rigths(Board* board){
    if (Board_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->castles, Board_CASTLE_WK) &&
            !NCH_CHKFLG(Board_WHITE_OCC(board), (NCH_SQR(NCH_E1) | NCH_SQR(NCH_H1))))
        {
            NCH_RMVFLG(board->castles, Board_CASTLE_WK);
        }
        if (NCH_CHKFLG(board->castles, Board_CASTLE_WQ) && 
            !NCH_CHKFLG(Board_WHITE_OCC(board), (NCH_SQR(NCH_E1) | NCH_SQR(NCH_A1))))
        {
            NCH_RMVFLG(board->castles, Board_CASTLE_WQ);
        }
    }
    else{
        if (NCH_CHKFLG(board->castles, Board_CASTLE_BK) &&
            !NCH_CHKFLG(Board_BLACK_OCC(board), (NCH_SQR(NCH_E8) | NCH_SQR(NCH_H8))))
        {
            NCH_RMVFLG(board->castles, Board_CASTLE_BK);
        }
        if (NCH_CHKFLG(board->castles, Board_CASTLE_BQ) &&
            !NCH_CHKFLG(Board_BLACK_OCC(board), (NCH_SQR(NCH_E8) | NCH_SQR(NCH_A8))))
        {
            NCH_RMVFLG(board->castles, Board_CASTLE_BQ);
        }
    }
}

NCH_STATIC_INLINE void
init_piecetables(Board* board){
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_WHITE_PIECE(board, i) = NCH_NO_PIECE;
    }

    for (int i = 0; i < NCH_SQUARE_NB; i++){
        Board_BLACK_PIECE(board, i) = NCH_NO_PIECE;
    }

    int idx;
    LOOP_U64_T(Board_WHITE_PAWNS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_WHITE_KNIGHTS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_WHITE_BISHOPS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_WHITE_ROOKS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_WHITE_QUEENS(board)){
        Board_WHITE_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_WHITE_KING(board)){
        Board_WHITE_PIECE(board, idx) = NCH_King;
    }

    LOOP_U64_T(Board_BLACK_PAWNS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Pawn;
    }

    LOOP_U64_T(Board_BLACK_KNIGHTS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Knight;
    }

    LOOP_U64_T(Board_BLACK_BISHOPS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Bishop;
    }
    
    LOOP_U64_T(Board_BLACK_ROOKS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Rook;
    }

    LOOP_U64_T(Board_BLACK_QUEENS(board)){
        Board_BLACK_PIECE(board, idx) = NCH_Queen;
    }

    LOOP_U64_T(Board_BLACK_KING(board)){
        Board_BLACK_PIECE(board, idx) = NCH_King;
    }
}

NCH_STATIC_INLINE void
_set_board_occupancy(Board* board){
    board->occupancy[NCH_White] = board->bitboards[NCH_White][NCH_Pawn]
                                | board->bitboards[NCH_White][NCH_Knight]
                                | board->bitboards[NCH_White][NCH_Bishop]
                                | board->bitboards[NCH_White][NCH_Rook]
                                | board->bitboards[NCH_White][NCH_Queen]
                                | board->bitboards[NCH_White][NCH_King];

    board->occupancy[NCH_Black] = board->bitboards[NCH_Black][NCH_Pawn]
                                | board->bitboards[NCH_Black][NCH_Knight]
                                | board->bitboards[NCH_Black][NCH_Bishop]
                                | board->bitboards[NCH_Black][NCH_Rook]
                                | board->bitboards[NCH_Black][NCH_Queen]
                                | board->bitboards[NCH_Black][NCH_King];

    board->occupancy[NCH_SIDES_NB] = board->occupancy[NCH_Black] 
                                   | board->occupancy[NCH_White];
}

NCH_STATIC_INLINE void
_init_board(Board* board){
    board->castles = Board_CASTLE_WQ | Board_CASTLE_WK | Board_CASTLE_BQ | Board_CASTLE_BK;
    board->en_passant_idx = 0;
    board->en_passant_map = 0ULL;
    board->en_passant_trg = 0ULL;
    board->flags = 0ULL;
    _set_board_occupancy(board);
    init_piecetables(board);
    Board_Update(board);
}

void
Board_Init(Board* board){
    board->bitboards[NCH_White][NCH_Pawn] = NCH_BOARD_W_PAWNS_STARTPOS;
    board->bitboards[NCH_White][NCH_Knight] = NCH_BOARD_W_KNIGHTS_STARTPOS;
    board->bitboards[NCH_White][NCH_Bishop] = NCH_BOARD_W_BISHOPS_STARTPOS;
    board->bitboards[NCH_White][NCH_Rook] = NCH_BOARD_W_ROOKS_STARTPOS;
    board->bitboards[NCH_White][NCH_Queen] = NCH_BOARD_W_QUEEN_STARTPOS;
    board->bitboards[NCH_White][NCH_King] = NCH_BOARD_W_KING_STARTPOS;

    board->bitboards[NCH_Black][NCH_Pawn] = NCH_BOARD_B_PAWNS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Knight] = NCH_BOARD_B_KNIGHTS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Bishop] = NCH_BOARD_B_BISHOPS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Rook] = NCH_BOARD_B_ROOKS_STARTPOS;
    board->bitboards[NCH_Black][NCH_Queen] = NCH_BOARD_B_QUEEN_STARTPOS;
    board->bitboards[NCH_Black][NCH_King] = NCH_BOARD_B_KING_STARTPOS;

    _init_board(board);
}

void
Board_InitEmpty(Board* board){
    board->bitboards[NCH_White][NCH_Pawn] = 0ULL;
    board->bitboards[NCH_White][NCH_Knight] = 0ULL;
    board->bitboards[NCH_White][NCH_Bishop] = 0ULL;
    board->bitboards[NCH_White][NCH_Rook] = 0ULL;
    board->bitboards[NCH_White][NCH_Queen] = 0ULL;
    board->bitboards[NCH_White][NCH_King] = 0ULL;

    board->bitboards[NCH_Black][NCH_Pawn] = 0ULL;
    board->bitboards[NCH_Black][NCH_Knight] = 0ULL;
    board->bitboards[NCH_Black][NCH_Bishop] = 0ULL;
    board->bitboards[NCH_Black][NCH_Rook] = 0ULL;
    board->bitboards[NCH_Black][NCH_Queen] = 0ULL;
    board->bitboards[NCH_Black][NCH_King] = 0ULL;

    _init_board(board);
}

void
Board_SetSquare(Board* board, Side side, Piece ptype, int sqr_idx){
    board->bitboards[side][ptype] |= NCH_SQR(sqr_idx);
    _set_board_occupancy(board);
}

void
Board_SetBitboard(Board* board, Side side, Piece ptype, uint64 bb){
    board->bitboards[side][ptype] = bb;
    _set_board_occupancy(board);
}

int
Board_IsCheck(Board* board){
    return get_checkmap(
            board,
            Board_IS_WHITETURN(board) ? NCH_White : NCH_Black,
            NCH_SQRIDX( Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
            Board_ALL_OCC(board)
        ) != 0ULL;
}

void
Board_Update(Board* board){
    reset_state_flags(board);
    reset_castle_rigths(board);
    flip_turn(board);
    if (Board_IsCheck(board)){
        NCH_SETFLG(board->flags, Board_CHECK);
    }

    generate_moves(board);
}

NCH_STATIC_INLINE int
is_valid_column(char arg){
    return arg >= 'a' && arg <= 'h';
}

NCH_STATIC_INLINE int
is_valid_row(char arg){
    return arg >= '1' && arg <= '8';
}

int
parse_step_arg(Board* board, char* arg, Square* from_, Square* to_, Piece* promotion, uint8* castle){
    int len = strlen(arg);
    if (len > 5 || len < 4){
        return -1;
    }

    if (!is_valid_column(arg[0]) || !is_valid_row(arg[1])
         || !is_valid_column(arg[2]) || !is_valid_row(arg[3]))
    {
        return -1;
    }

    *from_ = ('h' - arg[0]) + 8 * (arg[1] - '1');
    *to_ = ('h' - arg[2]) + 8 * (arg[3] - '1');

    if (len == 5){
        if (arg[4] == 'q'){
            *promotion = NCH_Queen;
        }
        else if (arg[4] == 'r'){
            *promotion = NCH_Rook;
        }
        else if (arg[4] == 'b'){
            *promotion = NCH_Bishop;
        }
        else if (arg[4] == 'n'){
            *promotion = NCH_Knight;
        }
        else{
            return -1;
        }
    }
    else{
        *promotion = 0;
    }

    if (Board_IS_WHITETURN(board)){
        if (*from_ == NCH_E1 && Board_WHITE_KING(board) == NCH_SQR(NCH_E1)){
            if (*to_ == NCH_G1){
                *castle = Board_CASTLE_WK;
            }
            else if (*to_ == NCH_C1){
                *castle = Board_CASTLE_WQ;
            }
        }
        else{
            *castle = 0;
        }
    }
    else{
        if (*from_ == NCH_E8 && Board_BLACK_KING(board) == NCH_SQR(NCH_E8)){
            if (*to_ == NCH_G8){
                *castle = Board_CASTLE_BK;
            }
            else if (*to_ == NCH_C8){
                *castle = Board_CASTLE_BQ;
            }
        }
        else{
            *castle = 0;
        }
    }

    return 0;
}

void
Board_Step(Board* board, char* move){
    Square from_, to_;
    Piece promotion;
    uint8 castle;

    if ( parse_step_arg(board, move, &from_, &to_, &promotion, &castle) != 0){
        return;
    }

    if (!is_valid_move(board, from_, to_)){
        return;
    }

    make_move(board, from_, to_, promotion, castle);
    Board_Update(board);   
}