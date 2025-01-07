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
#include "utils.h"
#include "hash.h"

NCH_STATIC_INLINE void
end_game_by_draw(Board* board, int state){
    NCH_SETFLG(board->flags, Board_GAMEEND | Board_DRAW | state);
}

NCH_STATIC_INLINE void
end_game_by_wl(Board* board){
    NCH_SETFLG(board->flags, Board_GAMEEND | (Board_IS_WHITETURN(board) ? 0 : Board_WIN));
}

NCH_STATIC_INLINE int
at_least_one_move(Board* board){
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        if (board->moves[i]){
            return 1;
        }
    }
    return 0;
}

NCH_STATIC_INLINE void
flip_turn(Board* board){
    NCH_FLPFLG(board->flags, Board_TURN);
}

NCH_STATIC_INLINE void
reset_state_flags(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK);
}

NCH_STATIC_INLINE void
reset_every_turn_states(Board* board){
    NCH_RMVFLG(board->flags, Board_CHECK | Board_DOUBLECHECK 
                           | Board_CAPTURE | Board_PAWNMOVED 
                           | Board_ENPASSANT | Board_PROMOTION);
}


NCH_STATIC_INLINE void
reset_castle_rigths(Board* board){
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

NCH_STATIC_INLINE void
_init_board_flags_and_states(Board* board){
    // board->castles = 0;
    // board->castles |= NCH_CHKFLG(Board_WHITE_KING(board), NCH_SQR(NCH_E1))
    //                 && NCH_CHKFLG(Board_WHITE_ROOKS(board), NCH_SQR(NCH_H1)) ? Board_CASTLE_WK : 0;
    // board->castles |= NCH_CHKFLG(Board_WHITE_KING(board), NCH_SQR(NCH_E1))
    //                 && NCH_CHKFLG(Board_WHITE_ROOKS(board), NCH_SQR(NCH_A1)) ? Board_CASTLE_WQ : 0;
    // board->castles |= NCH_CHKFLG(Board_BLACK_KING(board), NCH_SQR(NCH_E8))
    //                 && NCH_CHKFLG(Board_BLACK_ROOKS(board), NCH_SQR(NCH_H8)) ? Board_CASTLE_BK : 0;
    // board->castles |= NCH_CHKFLG(Board_BLACK_KING(board), NCH_SQR(NCH_E8))
    //                 && NCH_CHKFLG(Board_BLACK_ROOKS(board), NCH_SQR(NCH_A8)) ? Board_CASTLE_BQ : 0;

    board->castles = Board_CASTLE_WK | Board_CASTLE_WQ | Board_CASTLE_BK | Board_CASTLE_WQ;

    board->en_passant_idx = 0;
    board->en_passant_map = 0ULL;
    board->en_passant_trg = 0ULL;
    board->flags = Board_TURN;
    board->nmoves = 0;
    board->fifty_counter = 0;
}

NCH_STATIC_INLINE void
_init_board(Board* board){
    set_board_occupancy(board);
    init_piecetables(board);
    _init_board_flags_and_states(board);
    Board_Update(board);
}

NCH_STATIC_INLINE Board*
new_board(){
    Board* board = malloc(sizeof(Board));
    if (!board){
        return NULL;
    }

    board->movelist = MoveList_New();
    if (!board->movelist){
        free(board);
        return NULL;
    }

    board->dict = BoardDict_New();
    if (!board->dict){
        MoveList_Free(board->movelist);
        free(board);
        return NULL;
    }

    return board;
}

Board*
Board_New(){
    Board* board = new_board();
    if (!board){
        return NULL;
    }
    Board_Init(board);
    return board;
}


Board*
Board_NewEmpty(){
    Board* board = new_board();
    if (!board){
        return NULL;
    }
    Board_InitEmpty(board);
    return board;
}

void
Board_Free(Board* board){
    if (board){
        MoveList_Free(board->movelist);
        BoardDict_Free(board->dict);
        free(board);
    }
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
    board->castles = 0;
}

void
Board_SetSquare(Board* board, Side side, Piece ptype, int sqr_idx){
    board->bitboards[side][ptype] |= NCH_SQR(sqr_idx);
    set_board_occupancy(board);
}

void
Board_SetBitboard(Board* board, Side side, Piece ptype, uint64 bb){
    board->bitboards[side][ptype] = bb;
    set_board_occupancy(board);
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

NCH_STATIC_INLINE void
update_check(Board* board){
    if (Board_IsCheck(board)){
        uint64 check_map = get_checkmap(
            board,
            Board_IS_WHITETURN(board) ? NCH_White : NCH_Black,
            NCH_SQRIDX( Board_IS_WHITETURN(board) ? Board_WHITE_KING(board) : Board_BLACK_KING(board)),
            Board_ALL_OCC(board)
        );

        if (check_map){
            if (count_bits(check_map) > 1){
                NCH_SETFLG(board->flags, Board_CHECK | Board_DOUBLECHECK);
            }
            else{
                NCH_SETFLG(board->flags, Board_CHECK);
            }
        }
    }
}

void
Board_Update(Board* board){
    if (!Board_GAME_ON(board)){
        return;
    }
    update_check(board);

    generate_moves(board);

    if (BoardDict_GetCount(board->dict, board->bitboards) > 2){
        end_game_by_draw(board, Board_THREEFOLD);
        return;
    }

    if (board->fifty_counter > 49){
        end_game_by_draw(board, Board_FIFTYMOVES);
        return;
    }

    if (!at_least_one_move(board)){
        if (Board_IS_CHECK(board)){
            end_game_by_wl(board);
        }
        else{
            end_game_by_draw(board, Board_STALEMATE);
        }
    }
}

NCH_STATIC_INLINE void
increase_counter(Board* board){
    if (Board_IS_WHITETURN(board)){
        board->nmoves += 1;
    }

    if (NCH_CHKUNI(board->flags, Board_PAWNMOVED | Board_CAPTURE | Board_CHECK | Board_DOUBLECHECK)){
        board->fifty_counter = 0;
    }
    else{
        board->fifty_counter += 1;
    }
}

NCH_STATIC_INLINE void
_Board_MakeMove(Board* board, Square from_, Square to_, Piece promotion, uint8 castle){
    if (!Move_IsValid(board, from_, to_)){
        return;
    }
    Square last_en_passant_idx = board->en_passant_idx;
    uint8 last_castles = board->castles;
    int last_flags = board->flags;
    int last_fifty_counter = board->fifty_counter;

    reset_every_turn_states(board);

    Piece captured_piece = make_move(board, from_, to_, promotion, castle);
    Move move = Move_New(from_, to_, castle, promotion);
    MoveList_Append(board->movelist, move,
                     Board_IS_ENPASSANT(board), Board_IS_PROMOTION(board),
                     last_en_passant_idx, captured_piece,
                     last_fifty_counter, last_castles, last_flags);

    BoardDict_Add(board->dict, board->bitboards);

    reset_castle_rigths(board);
    flip_turn(board);
    increase_counter(board);
    Board_Update(board);
}

void
Board_StepByMove(Board* board, Move move){
    if (!Board_GAME_ON(board)){
        return;
    }

    Square from_, to_;
    Piece promotion;
    uint8 castle;
    
    Move_Parse(move, &from_, &to_, &castle, &promotion);
    _Board_MakeMove(board, from_, to_, promotion, castle);
}

void
Board_Step(Board* board, char* move){
    if (!Board_GAME_ON(board)){
        return;
    }

    Square from_, to_;
    Piece promotion;
    uint8 castle;

    if (Move_ParseFromString(board, move, &from_, &to_, &promotion, &castle) != 0){
        return;
    }

    _Board_MakeMove(board, from_, to_, promotion, castle);
}

void
Board_Undo(Board* board){
    MoveNode* node = MoveList_Last(board->movelist);
    if (!node){
        return;
    }
    BoardDict_Remove(board->dict, board->bitboards);

    undo_move(board, Board_GET_OP_SIDE(board),
             node->move, MoveNode_ENPASSANT(node),
             MoveNode_PROMOTION(node), MoveNode_CAP_PIECE(node));

    if (MoveNode_ENP_SQR(node)){
        set_board_enp_settings(board, Board_GET_SIDE(board), MoveNode_ENP_SQR(node));
    }
    else{
        reset_enpassant_variable(board);
    }
    board->fifty_counter = MoveNode_FIFTY_COUNT(node);
    board->castles = MoveNode_CASTLE_FLAGS(node);
    board->flags = MoveNode_GAME_FLAGS(node);

    if (Board_IS_BLACKTURN(board)){
        board->nmoves -= 1;
    }

    MoveList_Pop(board->movelist);
    Board_Update(board);
}

int
Board_NMoves(Board* board){
    int count = 0;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        if (board->moves[i]){
            count += count_bits(board->moves[i]);
        }
    }

    uint64* pawns_map = Board_IS_WHITETURN(board) ? &Board_WHITE_PAWNS(board)
                                                  : &Board_BLACK_PAWNS(board);

    int idx;
    LOOP_U64_T(*pawns_map){
        if (NCH_CHKUNI(board->moves[idx], NCH_ROW1 | NCH_ROW8)){
            count += 3 * count_bits(board->moves[idx]);
        }
    }

    return count;
}

int
Board_GetLegalMoves(Board* board, Move* moves){
    uint64 passer_pawns = Board_IS_WHITETURN(board) ?
                        Board_WHITE_PAWNS(board) & NCH_ROW7 :
                        Board_BLACK_PAWNS(board) & NCH_ROW2 ;

    uint64 king_sqr = Board_IS_WHITETURN(board) ? Board_WHITE_KING(board)
                                                : Board_BLACK_KING(board);
    int temp = NCH_SQRIDX(king_sqr);
    uint64 castle_moves = board->moves[temp] 
                        & (NCH_SQR(NCH_G1) | NCH_SQR(NCH_C1)
                        | NCH_SQR(NCH_G8) | NCH_SQR(NCH_C8));
    board->moves[temp] &= ~castle_moves;

    uint64 occ_map = (Board_IS_WHITETURN(board) ? Board_WHITE_OCC(board)
                                               : Board_BLACK_OCC(board))
                    & ~passer_pawns;


    int idx, counter = 0;
    LOOP_U64_NAMED(occ, temp, occ_map){
        if (board->moves[temp]){
            LOOP_U64_NAMED(mv, idx, board->moves[temp]){
                moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(idx);
            }
        }
    }
    
    LOOP_U64_NAMED(occ, temp, passer_pawns){
        if (board->moves[temp]){
            LOOP_U64_NAMED(mv, idx, board->moves[temp]){
                moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(idx) 
                                | Move_ASSIGN_PRO_PIECE(NCH_Queen);

                moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(idx) 
                                | Move_ASSIGN_PRO_PIECE(NCH_Rook);

                moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(idx) 
                                | Move_ASSIGN_PRO_PIECE(NCH_Bishop);

                moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(idx) 
                                | Move_ASSIGN_PRO_PIECE(NCH_Knight);
            }
        }
    }

    temp = NCH_SQRIDX(king_sqr);
    board->moves[temp] |= castle_moves;

    if (NCH_CHKFLG(NCH_SQR(NCH_E1) | NCH_SQR(NCH_E8), king_sqr) && castle_moves){
        if (NCH_CHKUNI(castle_moves, (NCH_SQR(NCH_G1) | NCH_SQR(NCH_G8)))){
            moves[counter++] = Move_ASSIGN_FROM(temp) 
                            | Move_ASSIGN_TO(NCH_SQRIDX(castle_moves & (NCH_SQR(NCH_G1) | NCH_SQR(NCH_G8)))) 
                            | Move_ASSIGN_CASTLE(
                                castle_moves & (NCH_SQR(NCH_G1) | NCH_SQR(NCH_G8)) ?
                                (Board_CASTLE_WK | Board_CASTLE_BK) : 0
                            );
        }

        if (NCH_CHKUNI(castle_moves, (NCH_SQR(NCH_C1) | NCH_SQR(NCH_C8)))){
            moves[counter++] = Move_ASSIGN_FROM(temp) 
                            | Move_ASSIGN_TO(NCH_SQRIDX(castle_moves & (NCH_SQR(NCH_C1) | NCH_SQR(NCH_C8)))) 
                            | Move_ASSIGN_CASTLE(
                                castle_moves & (NCH_SQR(NCH_C1) | NCH_SQR(NCH_C8)) ?
                                (Board_CASTLE_WQ | Board_CASTLE_BQ) : 0
                            );
        }
    }
    else{
        if (castle_moves){
            moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(NCH_SQRIDX(castle_moves));
            castle_moves &= castle_moves-1;
        }
        if (castle_moves){
            moves[counter++] = Move_ASSIGN_FROM(temp) | Move_ASSIGN_TO(NCH_SQRIDX(castle_moves));
        }
    }

    return counter;
}