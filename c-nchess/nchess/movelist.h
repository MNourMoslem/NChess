/*
    movelist.h

    This file defines the MoveList and MoveNode structures, which are used to 
    store and manage a list of moves in a chess game. It also provides 
    function declarations for initializing, modifying, and accessing moves 
    within the list.
*/

#ifndef NCHESS_SRC_MOVELIST_H
#define NCHESS_SRC_MOVELIST_H

#include "types.h"
#include "config.h"
#include "core.h"
#include "move.h"
#include <stdlib.h>

#define NCH_MOVELIST_SIZE 400

/*
    Important:

    The names MoveNode and MoveList have an issue, which is that they do
    not represent a Move only. MoveNode contains a move and position 
    information, which also gets stored in MoveNode as well.
*/

/*
    MoveNode

    Represents a single move in a move list, along with its associated position 
    information. Each MoveNode is part of a doubly linked list.
*/
typedef struct MoveNode {
    Move move;                // The move stored in this node.
    PositionInfo pos_info;    // Position information.

    struct MoveNode* prev;    // Pointer to the previous MoveNode in the list.
    struct MoveNode* next;    // Pointer to the next MoveNode in the list.
} MoveNode;

/*
    MoveList

    A data structure that holds a list of moves for a game. It maintains both 
    a fixed-size array of MoveNodes and an optional linked list for additional 
    moves when the array limit is exceeded.
*/
typedef struct {
    MoveNode nodes[NCH_MOVELIST_SIZE]; // An array of MoveNodes for fast access.
    int len;                           // The current number of moves stored.

    // Pointer to dynamically allocated extra MoveNodes if the array overflows.
    MoveNode* extra;                   
    MoveNode* last_extra;              // Pointer to the last allocated extra MoveNode.
} MoveList;

// Macros for accessing move properties from a MoveNode.
#define MoveNode_FROM(node) Move_FROM((node)->move)
#define MoveNode_TO(node) Move_TO((node)->move)
#define MoveNode_CASTLE(node) Move_CASTLE((node)->move)
#define MoveNode_PRO_PIECE(node) Move_PRO_PIECE((node)->move)
#define MoveNode_ENP_SQR(node) ((node)->pos_info.enp_sqr)
#define MoveNode_CAP_PIECE(node) ((node)->pos_info.captured_piece)
#define MoveNode_FIFTY_COUNT(node) ((node)->pos_info.fifty_count)
#define MoveNode_CASTLE_FLAGS(node) ((node)->pos_info.castle)
#define MoveNode_GAME_FLAGS(node) ((node)->pos_info.gameflags)

// Initializes a MoveList by resetting its length and clearing extra nodes.
void MoveList_Init(MoveList* movelist);

// Appends a new move to the MoveList.
// Returns 0 on success, -1 if the MoveList is full.
int MoveList_Append(MoveList* movelist, Move move, PositionInfo pos_info);

// Removes the last move from the MoveList.
// Does not return the last node.
void MoveList_Pop(MoveList* movelist);

// Retrieves a MoveNode from the MoveList by index.
// Returns a pointer to the MoveNode if found, NULL if the index is out of range.
MoveNode* MoveList_Get(MoveList* movelist, int idx);

// Frees any dynamically allocated memory used by the MoveList.
void MoveList_Free(MoveList* movelist);

// Resets the MoveList, clearing its contents.
void MoveList_Reset(MoveList* movelist);

// Copies extra MoveNodes from one MoveList to another.
// Returns 0 on success, -1 if copying fails.
int MoveList_CopyExtra(const MoveList* src, MoveList* dst);

// Returns a pointer to the last MoveNode if the list is not empty,
// returns NULL otherwise.
NCH_STATIC_INLINE MoveNode* MoveList_Last(MoveList* movelist) {
    if (movelist->len <= 0)
        return NULL;

    if (movelist->len <= NCH_MOVELIST_SIZE)
        return movelist->nodes + movelist->len - 1;

    return movelist->last_extra;
}

#endif // NCHESS_SRC_MOVELIST_H
