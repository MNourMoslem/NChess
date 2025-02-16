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

#define Board_BB(board, side, piece) (board)->bitboards[side][piece]

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

#define Board_FLAGS(board) (board)->info.flags
#define Board_CASTLES(board) (board)->info.castles
#define Board_FIFTY_COUNTER(board) (board)->info.fifty_counter
#define Board_ENP_IDX(board) (board)->info.en_passant_idx
#define Board_ENP_MAP(board) (board)->info.en_passant_map
#define Board_ENP_TRG(board) (board)->info.en_passant_trg
#define Board_CAP_PIECE(board) (board)->info.captured_piece

#define Board_NMOVES(board) (board)->nmoves

// returns the piece on the square idx
#define Board_ON_SQUARE(board, idx) Board_WHITE_PIECE(board, idx) != NCH_NO_PIECE ?\
                                    Board_WHITE_PIECE(board, idx) : Board_BLACK_PIECE(board, idx)

// returns the side that owns the piece on the square idx
#define Board_OWNED_BY(board, idx) Board_WHITE_PIECE(board, idx) != NCH_NO_PIECE ?\
                                   NCH_White : Board_BLACK_PIECE(board, idx) != NCH_NO_PIECE ?\
                                   NCH_Black : NCH_NO_SIDE;


/*
    Board flags.

    These flags are used to store the state of the board.
    flags realted to the state of the game would be removed like
    DRAW, WIN, GAMEEND, etc. and would be replaced with a single.
    the nchess.Board will not be responsible for the state of the game.
    however the Board_State function would be used to get the state of the game.
    the flags would be used to store the state of the board like CHECK, CAPTURE, etc.
*/
#define Board_PAWNMOVED 0x1
#define Board_ENPASSANT 0x2
#define Board_CAPTURE 0x4
#define Board_PROMOTION 0x8
#define Board_CHECK 0x10
#define Board_DOUBLECHECK 0x20
#define Board_CHECKMATE 0x40
#define Board_STALEMATE 0x80
#define Board_THREEFOLD 0x100
#define Board_FIFTYMOVES 0x200
#define Board_GAMEEND 0x400
#define Board_DRAW 0x800
#define Board_WIN 0x1000
#define Board_TURN 0x2000

#define Board_IS_PAWNMOVED(board) (board->info.flags & Board_PAWNMOVED)
#define Board_IS_DOUBLECHECK(board) (board->info.flags & Board_DOUBLECHECK)
#define Board_IS_ENPASSANT(board) (board->info.flags & Board_ENPASSANT)
#define Board_IS_CAPTURE(board) (board->info.flags & Board_CAPTURE)
#define Board_IS_PROMOTION(board) (board->info.flags & Board_PROMOTION)
#define Board_IS_CHECK(board) (board->info.flags & Board_CHECK)
#define Board_IS_CHECKMATE(board) (board->info.flags & Board_CHECKMATE)
#define Board_IS_STALEMATE(board) (board->info.flags & Board_STALEMATE)
#define Board_IS_THREEFOLD(board) (board->info.flags & Board_THREEFOLD)
#define Board_IS_FIFTYMOVES(board) (board->info.flags & Board_FIFTYMOVES)
#define Board_IS_GAMEEND(board) (board->info.flags & Board_GAMEEND)
#define Board_IS_DRAW(board) (board->info.flags & Board_DRAW)
#define Board_IS_WHITEWIN(board) (board->info.flags & Board_WIN)
#define Board_IS_BLACKWIN(board) !Board_IS_WHITEWIN(board)
#define Board_IS_WHITETURN(board) (board->info.flags & Board_TURN)
#define Board_IS_BLACKTURN(board) !Board_IS_WHITETURN(board)

#define Board_GAME_ON(board) !Board_IS_GAMEEND(board) // would be removed later
#define Board_GET_SIDE(board) (Board_IS_WHITETURN(board) ? NCH_White : NCH_Black)
#define Board_GET_OP_SIDE(board) (Board_IS_WHITETURN(board) ? NCH_Black : NCH_White)

/*
    Castle rights flags
*/
#define Board_CASTLE_WK (uint8)1
#define Board_CASTLE_WQ (uint8)2
#define Board_CASTLE_BK (uint8)4
#define Board_CASTLE_BQ (uint8)8

#define Board_IS_CASTLE_WK(board) (board->info.castles & Board_CASTLE_WK)
#define Board_IS_CASTLE_WQ(board) (board->info.castles & Board_CASTLE_WQ)
#define Board_IS_CASTLE_BK(board) (board->info.castles & Board_CASTLE_BK)
#define Board_IS_CASTLE_BQ(board) (board->info.castles & Board_CASTLE_BQ)

/*
    Board functions.
    As metions above all functions in this file are creation, initialization, copying
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
// this functions used if the board is already allocated and initialized
void
Board_Init(Board* board);


// initializes the board with the empty board
// this functions used if the board is already allocated and initialized
void
Board_InitEmpty(Board* board);

// checks if the king of the side that is to move is under attack
int
Board_IsCheck(const Board* board);

// resets the board to the initial state
void
Board_Reset(Board* board);

// checks if the board is in a state of insufficient material
int
Board_IsInsufficientMaterial(const Board* board);

// checks if the board is in a state of threefold repetition
int
Board_IsThreeFold(const Board* board);

// checks if the board is in a state of fifty moves rule
int
Board_IsFiftyMoves(const Board* board);

// copies the board and returns the new board
Board*
Board_Copy(const Board* src_board);

// returns the state of the game
GameState
Board_State(const Board* board, int can_move);


#endif