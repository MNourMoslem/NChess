/*
    board.h

    This file contains the Board struct and its functions.
    functions in this files all are creation, initialization, copying
    and checking state functions. Other functions realted to move generation,
    move making, move undoing and perft search exist in files with related names.
*/

#ifndef NCHESS_SRC_BOARD_H
#define NCHESS_SRC_BOARD_H

#include "core.h"
#include "types.h"
#include "config.h"
#include "movelist.h"
#include "hash.h"

/*
    Starting position bitboards of each piece
*/
#define NCH_BOARD_W_PAWNS_STARTPOS 0x000000000000FF00
#define NCH_BOARD_W_KNIGHTS_STARTPOS 0x0000000000000042
#define NCH_BOARD_W_BISHOPS_STARTPOS 0x0000000000000024
#define NCH_BOARD_W_ROOKS_STARTPOS 0x0000000000000081
#define NCH_BOARD_W_QUEEN_STARTPOS 0x0000000000000010
#define NCH_BOARD_W_KING_STARTPOS 0x0000000000000008

#define NCH_BOARD_B_PAWNS_STARTPOS 0x00FF000000000000
#define NCH_BOARD_B_KNIGHTS_STARTPOS 0x4200000000000000
#define NCH_BOARD_B_BISHOPS_STARTPOS 0x2400000000000000
#define NCH_BOARD_B_ROOKS_STARTPOS 0x8100000000000000
#define NCH_BOARD_B_QUEEN_STARTPOS 0x1000000000000000
#define NCH_BOARD_B_KING_STARTPOS 0x0800000000000000

/*
    Board struct
*/
typedef struct
{
    uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB];    // bitboards for each piece type
    uint64 occupancy[NCH_SIDES_NB + 1];              // occupancy bitboards for each
                                                     // side and both sides
    
    // piece table for each side.
    // The table is an array of size NCH_SQUARE_NB
    // each index in the array represents a square on the board
    // and the value at that index represents the piece on that square
    // This is useful to retrieve the piece on a square quickly instead of
    // searching through the bitboards.
    Piece piecetables[NCH_SIDES_NB][NCH_SQUARE_NB];  

    // stores all variables that gets copied when a step is taken 
    // like flags, castle rights, etc.
    PositionInfo info;

    // These variables are used to store the information related to the move that was made
    MoveList movelist; // move stack
    BoardDict dict;   // position dictionary

    int nmoves;        // number of half moves

    // A table that containes the source and the destination squares of the rooks
    // when castling. like for example if the king goes to g1. then in the table
    // g1 index would contain the rook source square h1 for example and h1 index
    // would contain the rook destination square f1.
    Square castle_squares[NCH_SQUARE_NB];
}Board;

/*
    Macros below used to access the fields of the board struct
    it is more efficient to use these macros instead of accessing the fields directly
*/

#define Board_OCC(board, side) (board)->occupancy[side]

#define Board_WHITE_OCC(board) Board_OCC(board, NCH_White)
#define Board_BLACK_OCC(board) Board_OCC(board, NCH_Black)
#define Board_ALL_OCC(board) Board_OCC(board, NCH_SIDES_NB)

#define Board_WHITE_TABLE(board) (board)->piecetables[NCH_White]
#define Board_BLACK_TABLE(board) (board)->piecetables[NCH_Black]

#define Board_BBS_PTR(board) (board)->bitboards
#define Board_BB(board, side, piece) Board_BBS_PTR(board)[side][piece]

#define Board_WHITE_PAWNS(board) Board_BB(board, NCH_White, NCH_Pawn)
#define Board_WHITE_KNIGHTS(board) Board_BB(board, NCH_White, NCH_Knight)
#define Board_WHITE_BISHOPS(board) Board_BB(board, NCH_White, NCH_Bishop)
#define Board_WHITE_ROOKS(board) Board_BB(board, NCH_White, NCH_Rook)
#define Board_WHITE_QUEENS(board) Board_BB(board, NCH_White, NCH_Queen)
#define Board_WHITE_KING(board) Board_BB(board, NCH_White, NCH_King)

#define Board_BLACK_PAWNS(board) Board_BB(board, NCH_Black, NCH_Pawn)
#define Board_BLACK_KNIGHTS(board) Board_BB(board, NCH_Black, NCH_Knight)
#define Board_BLACK_BISHOPS(board) Board_BB(board, NCH_Black, NCH_Bishop)
#define Board_BLACK_ROOKS(board) Board_BB(board, NCH_Black, NCH_Rook)
#define Board_BLACK_QUEENS(board) Board_BB(board, NCH_Black, NCH_Queen)
#define Board_BLACK_KING(board) Board_BB(board, NCH_Black, NCH_King)

#define Board_PIECE(board, side, idx) (board)->piecetables[side][idx]
#define Board_WHITE_PIECE(board, idx) Board_PIECE(board, NCH_White, idx)
#define Board_BLACK_PIECE(board, idx) Board_PIECE(board, NCH_Black, idx)

#define Board_INFO(board) (board)->info

#define Board_FLAGS(board) Board_INFO(board).flags
#define Board_CASTLES(board) Board_INFO(board).castles
#define Board_FIFTY_COUNTER(board) Board_INFO(board).fifty_counter
#define Board_ENP_IDX(board) Board_INFO(board).en_passant_idx
#define Board_ENP_MAP(board) Board_INFO(board).en_passant_map
#define Board_ENP_TRG(board) Board_INFO(board).en_passant_trg
#define Board_CAP_PIECE(board) Board_INFO(board).captured_piece
#define Board_SIDE(board) Board_INFO(board).side
#define Board_OP_SIDE(board) NCH_OP_SIDE(Board_SIDE(board))

#define Board_DICT(board) (board)->dict
#define Board_MOVELIST(board) (board)->movelist

#define Board_NMOVES(board) (board)->nmoves

#define Board_CASTLE_SQUARES(board, sqr) (board)->castle_squares[sqr]

// returns the piece on the square idx
#define Board_ON_SQUARE(board, idx) Board_WHITE_PIECE(board, idx) != NCH_NO_PIECE ?\
                                    Board_WHITE_PIECE(board, idx) : Board_BLACK_PIECE(board, idx)

// returns the side that owns the piece on the square idx
#define Board_OWNED_BY(board, idx) Board_WHITE_PIECE(board, idx) != NCH_NO_PIECE ?\
                                   NCH_White : Board_BLACK_PIECE(board, idx) != NCH_NO_PIECE ?\
                                   NCH_Black : NCH_NO_SIDE;


/*
    Board flags.

    These flags are used to store actions happens every step like if a pawn moved
    or a capture happened. These flags are not used to store the state of the game.
    The nchess.Board will not be responsible for the state of the game.
    however the Board_State function would be used to get the state of the game.
*/
#define Board_PAWNMOVED 1
#define Board_CAPTURE 2
#define Board_CHECK 4
#define Board_DOUBLECHECK 8

#define Board_IS_PAWNMOVED(board) (Board_FLAGS(board) & Board_PAWNMOVED)
#define Board_IS_CAPTURE(board) (Board_FLAGS(board) & Board_CAPTURE)
#define Board_IS_CHECK(board) (Board_FLAGS(board) & Board_CHECK)
#define Board_IS_DOUBLECHECK(board) (Board_FLAGS(board) & Board_DOUBLECHECK)
#define Board_IS_WHITETURN(board) !Board_SIDE(board)
#define Board_IS_BLACKTURN(board) Board_SIDE(board)

/*
    Castle rights flags
*/
#define Board_CASTLE_WK (uint8)1
#define Board_CASTLE_WQ (uint8)2
#define Board_CASTLE_BK (uint8)4
#define Board_CASTLE_BQ (uint8)8

#define Board_IS_CASTLE_WK(board) (Board_CASTLES(board) & Board_CASTLE_WK)
#define Board_IS_CASTLE_WQ(board) (Board_CASTLES(board) & Board_CASTLE_WQ)
#define Board_IS_CASTLE_BK(board) (Board_CASTLES(board) & Board_CASTLE_BK)
#define Board_IS_CASTLE_BQ(board) (Board_CASTLES(board) & Board_CASTLE_BQ)

/*
    Board functions.
    As mentioned above all functions in this file are creation, initialization, copying
    and checking state functions.
*/

// creates a new board and initializes it with the standard starting position
// no need to call Board_Init after calling this function.
Board*
Board_New();


// creates a new board and initializes it with no pieces on the board
// no need to call Board_Init after calling this function.
Board*
Board_NewEmpty();

// frees the memory allocated for the board
void
Board_Free(Board* board);

// initializes the board with the standard starting position
// this functions used if the board is already allocated
void
Board_Init(Board* board);

// initializes the board with the empty board
// this functions used if the board is already allocated
void
Board_InitEmpty(Board* board);

// checks if the king of the side that is to move is under attack
// returns 1 if the king is under attack and 0 otherwise
int
Board_IsCheck(const Board* board);

// resets the board to the initial state
void
Board_Reset(Board* board);

// checks if the board is in a state of insufficient material
// returns 1 if the board is in a state of insufficient material and 0 otherwise
int
Board_IsInsufficientMaterial(const Board* board);

// checks if the board is in a state of threefold repetition
// returns 1 if the board is in a state of threefold repetition and 0 otherwise
int
Board_IsThreeFold(const Board* board);

// checks if the board is in a state of fifty moves rule
// returns 1 if the board is in a state of fifty moves rule and 0 otherwise
int
Board_IsFiftyMoves(const Board* board);

// copies the board to the destination board
// returns 0 on success and -1 on failure
int
Board_Copy(const Board* src_board, Board* dst_board);

// copies the board and returns the new board
Board*
Board_NewCopy(const Board* src_board);

// returns the state of the game
GameState
Board_State(const Board* board, int can_move);

#endif