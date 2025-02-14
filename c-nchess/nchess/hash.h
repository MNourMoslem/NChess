/*
    hash.h

    This file contains the main struct that stores the board position in a hash
    table for threefold repetition.

    - The hash table is a basic fixed-size array with linked lists.
    - The current naming convention uses BoardDict for the table and BoardNode for the
      hash node, but this may change later.
*/

#ifndef NCHESS_SRC_HASH_H
#define NCHESS_SRC_HASH_H

#include "types.h"
#include "core.h"

#define NCH_BOARD_DICT_SIZE 100

typedef struct BoardNode
{
    int empty;
    uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB];
    int count;

    struct BoardNode* next;
}BoardNode;

typedef struct
{
    BoardNode nodes[NCH_BOARD_DICT_SIZE];
}BoardDict;

// Initializes the board dictionary.
void
BoardDict_Init(BoardDict* dict);

// Frees all linked lists created outside the array.
void
BoardDict_FreeExtra(BoardDict* dict);

// Returns the count of a given position.
int
BoardDict_GetCount(const BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

// Adds a position to the dictionary. If the position already exists, increments
// its counter by 1.
int
BoardDict_Add(BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

// Removes a position from the dictionary. If the position exists, decrements
// its counter by 1.
int
BoardDict_Remove(BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

// Deletes all linked lists outside the array and sets all nodes in the array to empty.
// This function is equivalent to calling FreeExtra followed by Init.
void
BoardDict_Reset(BoardDict* dict);

// Copies all linked lists outside the array to the destination dictionary.
// This function is not responsible for copying the array of the dictionary itself.
int
BoardDict_CopyExtra(const BoardDict* src, BoardDict* dst);

#endif
