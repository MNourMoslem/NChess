#ifndef NCHESS_BOARD_H
#define NCHESS_BOARD_H

#include "assign.h"
#include "types.h"
#include "core.h"
#include "scan.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int NCH_GAMEDICT_SIZE = 100;

#define _CBoard_CLEAN_POSSIBLEMOVES(board) memset(board->possible_moves, 0, sizeof(board->possible_moves))

#define _NCH_WHITE_KING_ATTACKERS(board, map, sqr, idx)     ((NCH_SCAN_ROOK_LIKE(map, sqr, idx)\
                                                            & (board->B_Rooks | board->B_Queens))\
                                                            |\
                                                            (NCH_SCAN_BISHOP_LIKE(map, sqr, idx)\
                                                            & (board->B_Bishops | board->B_Queens))\
                                                            |\
                                                            (NCH_SCAN_KNIGHT_LIKE(sqr)\
                                                            & board->B_Knights)\
                                                            |\
                                                            (NCH_SCAN_PAWNATTACK_LIKE_W(sqr)\
                                                            & board->B_Pawns))

#define _NCH_BLACK_KING_ATTACKERS(board, map, sqr, idx)     ((NCH_SCAN_ROOK_LIKE(map, sqr, idx)\
                                                            & (board->W_Rooks | board->W_Queens))\
                                                            |\
                                                            (NCH_SCAN_BISHOP_LIKE(map, sqr, idx)\
                                                            & (board->W_Bishops | board->W_Queens))\
                                                            |\
                                                            (NCH_SCAN_KNIGHT_LIKE(sqr)\
                                                            & board->W_Knights)\
                                                            |\
                                                            (NCH_SCAN_PAWNATTACK_LIKE_B(sqr)\
                                                            & board->W_Pawns))

int _CBoard_MakeMove(CBoard* board, int turn, cuint64* piece_map, cuint64 from_, cuint64 to_, cuint64 cap_sqr){
    NCH_CNGFLG(*piece_map, from_, to_);
    int is_cap = 0;

    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->Black_Map, cap_sqr)){
            board->B_Pawns &= ~cap_sqr;
            board->B_Knights &= ~cap_sqr;
            board->B_Bishops &= ~cap_sqr;
            board->B_Rooks &= ~cap_sqr;
            board->B_Queens &= ~cap_sqr;
            is_cap = 1;
        }
    }
    else{
        if (NCH_CHKFLG(board->White_Map, cap_sqr)){
            board->W_Pawns &= ~cap_sqr;
            board->W_Knights &= ~cap_sqr;
            board->W_Bishops &= ~cap_sqr;
            board->W_Rooks &= ~cap_sqr;
            board->W_Queens &= ~cap_sqr;
            is_cap = 1;
        }
    }

    _NCH_ASSAIGN_BOARD_MAPS(board)
    return is_cap;
}

cuint64 _CBoard_GetPossibleSquares(CBoard* board, cuint64 king_vision, cuint64 king, int king_idx, cuint64 map){
    cuint64 psqrs = 0ull;
    if (NCH_B_IS_CHECK(board)){
        if (!NCH_B_IS_DOUBLECHECK(board)){
            int idx = NCH_SQRIDX(board->king_attackers);

            psqrs = NCH_SAME_COL(king_idx, idx) ? ( king_idx > idx ? NCH_SCAN_DOWN(map, king, king_idx) : NCH_SCAN_UP(map, king, king_idx))
                  : NCH_SAME_ROW(king_idx, idx) ? ( king_idx > idx ? NCH_SCAN_RIGHT(map, king, king_idx) : NCH_SCAN_LEFT(map, king, king_idx))
                  : NCH_SAME_MAIN_DG(idx, king_idx) ? ( king_idx > idx ? NCH_SCAN_DOWN_LEFT(map, king) : NCH_SCAN_UP_RIGHT(map, king, king_idx))
                  : NCH_SAME_ANTI_DG(idx, king_idx) ? ( king_idx > idx ? NCH_SCAN_DOWN_RIGHT(map, king) : NCH_SCAN_UP_LEFT(map, king, king_idx))
                  : board->king_attackers;
        }
    }
    else{
        psqrs = NCH_CUINT64_MAX;
    }
    return psqrs;
}

void _CBoard_PreventPinnedPieces(CBoard* board, int turn, cuint64 ply_map, cuint64 op_map, cuint64 king_vision){
    cuint64 king = turn == NCH_WHITE ? board->W_King : board->B_King;
    cuint64 rooks = turn == NCH_BLACK ? board->W_Rooks | board->W_Queens : board->B_Rooks | board->B_Queens;
    cuint64 bishops = turn == NCH_BLACK ? board->W_Bishops | board->W_Queens : board->B_Bishops | board->B_Queens;
    
    int king_idx = NCH_SQRIDX(king);

    cuint64* pmoves = board->possible_moves;

    cuint64 temp = ((ply_map &~ (king_vision | king)) | op_map);
    cuint64 scan;

    scan = NCH_SCAN_UP(temp, king, king_idx);
    if (NCH_CHKUNI(scan, rooks) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_DOWN(temp, king, king_idx);
    if (NCH_CHKUNI(scan, rooks) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_RIGHT(temp, king, king_idx);
    if (NCH_CHKUNI(scan, rooks) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_LEFT(temp, king, king_idx);
    if (NCH_CHKUNI(scan, rooks) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_UP_RIGHT(temp, king, king_idx);
    if (NCH_CHKUNI(scan, bishops) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_DOWN_RIGHT(temp, king);
    if (NCH_CHKUNI(scan, bishops) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_UP_LEFT(temp, king, king_idx);
    if (NCH_CHKUNI(scan, bishops) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }

    scan = NCH_SCAN_DOWN_LEFT(temp, king);
    if (NCH_CHKUNI(scan, bishops) && NCH_CHKUNI(scan, ply_map)){
        pmoves[NCH_SQRIDX(scan & ply_map)] &= scan;
    }
}

void _CBoard_CheckCastleAvailability(CBoard* board, int turn){
    if (turn == NCH_WHITE){
        if (!NCH_CHKFLG(board->castle_flags ,NCH_CF_WHITE_OO) && !NCH_CHKUNI(board->All_Map, 0x6ull)
            && (_NCH_WHITE_KING_ATTACKERS(board, board->All_Map, NCH_F1, 2) == 0ull)
            && (_NCH_WHITE_KING_ATTACKERS(board, board->All_Map, NCH_G1, 1) == 0ull)){
                NCH_SETFLG(board->castle_flags, NCH_CF_COULD_WHITE_OO);
            }
        if (!NCH_CHKFLG(board->castle_flags ,NCH_CF_WHITE_OOO) && !NCH_CHKUNI(board->All_Map, 0x112ull)
            && (_NCH_WHITE_KING_ATTACKERS(board, board->All_Map, NCH_C1, 5) == 0ull)
            && (_NCH_WHITE_KING_ATTACKERS(board, board->All_Map, NCH_D1, 4) == 0ull)){
                NCH_SETFLG(board->castle_flags, NCH_CF_COULD_WHITE_OOO);
            }
    }
    else{
        if (!NCH_CHKFLG(board->castle_flags ,NCH_CF_BLACK_OO) && !NCH_CHKUNI(board->All_Map, 0x0600000000000000ull)
            && (_NCH_BLACK_KING_ATTACKERS(board, board->All_Map, NCH_F8, 58) == 0ull)
            && (_NCH_BLACK_KING_ATTACKERS(board, board->All_Map, NCH_G8, 57) == 0ull)){
                NCH_SETFLG(board->castle_flags, NCH_CF_COULD_BLACK_OO);
            }
        if (!NCH_CHKFLG(board->castle_flags ,NCH_CF_BLACK_OOO) && !NCH_CHKUNI(board->All_Map, 0x7000000000000000ull)
            && (_NCH_BLACK_KING_ATTACKERS(board, board->All_Map, NCH_C8, 61)) == 0ull
            && (_NCH_BLACK_KING_ATTACKERS(board, board->All_Map, NCH_D8, 60)) == 0ull){
                NCH_SETFLG(board->castle_flags, NCH_CF_COULD_BLACK_OOO);
            }
    }
}

int _CBoard_SetPossibleMoves(CBoard* board){
    cuint64* pmoves = board->possible_moves;
    cuint64 ply_map, op_map, pmap;
    cuint64 all_map = board->All_Map;
    cuint64 temp, king;

    int turn = NCH_B_IS_WHITETURN(board) ? NCH_WHITE : NCH_BLACK;

    if (turn == NCH_WHITE){
        ply_map = board->White_Map;
        op_map = board->Black_Map;
        king = board->W_King;
    }
    else{
        ply_map = board->Black_Map;
        op_map = board->White_Map;
        king = board->B_King;
    }

    temp = all_map &~ king;
    int king_idx = NCH_SQRIDX(king);
    cuint64 king_vision = NCH_SCAN_QUEEN_LIKE(temp, king, king_idx); 

    cuint64 psqrs = _CBoard_GetPossibleSquares(board, king_vision, king, king_idx, temp);

    NCH_CF_RESET_COULDCASLTE(board);
    _CBoard_CLEAN_POSSIBLEMOVES(board);
    
    if (psqrs != 0ull){
        pmap = ~ply_map & psqrs;

        _generate_pawn_moves(board, turn, all_map, op_map, psqrs);
        _generate_non_pawn_moves(board, turn, all_map, pmap);
        _CBoard_PreventPinnedPieces(board, turn, ply_map, op_map, king_vision);
    }
    generate_moves(board, turn, ply_map, psqrs);

    if (!NCH_B_IS_CHECK(board)){ 
        _CBoard_CheckCastleAvailability(board, turn);
    }
}

void _CBoard_CheckKingIsChecked(CBoard* board){
    cuint64 king = NCH_B_IS_WHITETURN(board) ? board->W_King : board->B_King;
    int king_idx = NCH_SQRIDX(king);
    cuint64 map = board->All_Map &~ king;
    if (NCH_B_IS_WHITETURN(board)){
        board->king_attackers = _NCH_WHITE_KING_ATTACKERS(board, map, king, king_idx);
    }    
    else{
        board->king_attackers = _NCH_BLACK_KING_ATTACKERS(board, map, king, king_idx);
    }
    if (board->king_attackers != 0ull){
        NCH_SETFLG(board->flags, NCH_B_CHECK);
        if (NCH_POPCOUNTLL(board->king_attackers) > 1){
            NCH_SETFLG(board->flags, NCH_B_DOUBLECHECK);
        }
    }
}

int _CBoard_CheckThreeFold(CBoard* board){
    cuint64 key = _CBoard_ToKey(board);
    if (_NCH_Ht_AddValueToItem(board->GameDict ,key, 1) >= 3){
        NCH_SETFLG(board->flags, NCH_B_THREEFOLD);
        NCH_SETFLG(board->flags, NCH_B_DRAW);
        NCH_SETFLG(board->flags, NCH_B_GAMEEND);
        return 0;
    }
    return -1;
}

int _CBoard_CheckFiftyMoves(CBoard* board){
    if (NCH_CHKUNI(board->flags, NCH_B_MASK_GAMEACTIONS)){
        board->fifty_count = 0;
    }
    else{
        board->fifty_count += 1;
        if (board->fifty_count >= 50){
            NCH_SETFLG(board->flags, NCH_B_FIFTYMOVES);
            NCH_SETFLG(board->flags, NCH_B_DRAW);
            NCH_SETFLG(board->flags, NCH_B_GAMEEND);
            return 0;
        }
    }
    return -1;
}

int _CBoard_CheckMateAndStaleMate(CBoard* board){
    if (CBoard_HasNoPossibleMove(board)){
        NCH_SETFLG(board->flags, NCH_B_GAMEEND);
        if (NCH_B_IS_CHECK(board)){
            if (NCH_B_IS_WHITETURN(board)){
                NCH_RMVFLG(board->flags, NCH_B_WIN);
            }
            else{
                NCH_SETFLG(board->flags, NCH_B_WIN);
            }

        }
        else{
            NCH_SETFLG(board->flags, NCH_B_DRAW);
            NCH_SETFLG(board->flags, NCH_B_STALEMATE);
        }
        return 0;
    }
    return -1;
}

void _CBoard_CheckCastle(CBoard* board){
    if (NCH_B_IS_WHITETURN(board)){
        if (!NCH_CHKFLG(board->White_Map, 0x9ull)){
            NCH_SETFLG(board->castle_flags, NCH_CF_WHITE_OO);
        }
        if (!NCH_CHKFLG(board->White_Map, 0x88ull)){
            NCH_SETFLG(board->castle_flags, NCH_CF_WHITE_OOO);
        } 
    }
    else{
        if (!NCH_CHKFLG(board->Black_Map, 0x0900000000000000ull)){
            NCH_SETFLG(board->castle_flags, NCH_CF_BLACK_OO);
        }
        if (!NCH_CHKFLG(board->Black_Map, 0x8800000000000000ull)){
            NCH_SETFLG(board->castle_flags, NCH_CF_BLACK_OOO);
        } 
    }
}

int _CBoard_Update(CBoard* board){        
    _NCH_ASSAIGN_BOARD_MAPS(board)
    NCH_FLPFLG(board->flags, NCH_B_TURN);

    _CBoard_CheckCastle(board);

    if (_CBoard_CheckThreeFold(board) == 0){
        return -1;
    }
    if (_CBoard_CheckFiftyMoves(board) == 0){
        return -1;
    }

    _CBoard_CheckKingIsChecked(board);
    _CBoard_SetPossibleMoves(board);
    if (_CBoard_CheckMateAndStaleMate(board) == 0){
        return -1;
    }

    return 0;
}

int _CBoard_StepCastle(CBoard* board, NCH_SMoves smove){
    int done = -1;
    if (smove == NCH_OO){
        if (NCH_B_IS_WHITETURN(board)){
            if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OO)){
                NCH_CNGFLG(board->W_Rooks, NCH_H1, NCH_F1);
                NCH_CNGFLG(board->W_King, NCH_E1, NCH_G1);
                done = 0;
            }
        }
        else{
            if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OO)){
                NCH_CNGFLG(board->B_Rooks, NCH_H8, NCH_F8);
                NCH_CNGFLG(board->B_King, NCH_E8, NCH_G8);
                done = 0;
            }
        }
    }
    else if (smove == NCH_OOO){
        if (NCH_B_IS_WHITETURN(board)){
            if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OOO)){
                NCH_CNGFLG(board->W_Rooks, NCH_A1, NCH_D1);
                NCH_CNGFLG(board->W_King, NCH_E1, NCH_C1);
                done = 0;
            }
        }
        else{
            printf("Castle Flags After: 0x%x\n", board->castle_flags);
            if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OOO)){
                NCH_CNGFLG(board->B_Rooks, NCH_A8, NCH_D8);
                NCH_CNGFLG(board->B_King, NCH_E8, NCH_C8);
                done = 0;
            }
        }
    }

    _CBoard_Update(board);

    return done;
}

void _CBoard_MakePromotion(CBoard* board, cuint64 sqr, NCH_SMoves smove){
    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(NCH_ROW8, sqr)){
            NCH_RMVFLG(board->W_Pawns, sqr);
            switch (smove)
            {
            case NCH_Promote2Queen:
                NCH_SETFLG(board->W_Queens, sqr);
                break;
            
            case NCH_Promote2Rook:
                NCH_SETFLG(board->W_Rooks, sqr);
                break;
            
            case NCH_Promote2Knight:
                NCH_SETFLG(board->W_Knights, sqr);
                break;
            
            case NCH_Promote2Bishop:
                NCH_SETFLG(board->W_Bishops, sqr);
                break;
            
            default:
                NCH_SETFLG(board->W_Queens, sqr);
                break;
            }
        }
    }
    else{
        if (NCH_CHKFLG(NCH_ROW1, sqr)){
            NCH_RMVFLG(board->B_Pawns, sqr);
            switch (smove)
            {
            case NCH_Promote2Queen:
                NCH_SETFLG(board->B_Queens, sqr);
                break;
            
            case NCH_Promote2Rook:
                NCH_SETFLG(board->B_Rooks, sqr);
                break;
            
            case NCH_Promote2Knight:
                NCH_SETFLG(board->B_Knights, sqr);
                break;
            
            case NCH_Promote2Bishop:
                NCH_SETFLG(board->B_Bishops, sqr);
                break;
            
            default:
                NCH_SETFLG(board->B_Queens, sqr);
                break;
            }
        }
    }
}

int _CBoard_Step(CBoard* board, cuint64* piece_map, cuint64 from_, cuint64 to_, NCH_SMoves smove){
    if (NCH_CHKFLG(board->flags, NCH_B_GAMEEND)){
        return -1;
    }

    NCH_B_RESET_GAMEACTIONS(board);

    if (from_ == NCH_NONE || to_ == NCH_NONE || piece_map == NULL){
        if (smove == NCH_OO || smove == NCH_OOO){
            CBoard_Print(board);
            printf("Castle Flags Before 5: 0x%x\n", board->castle_flags);
            return _CBoard_StepCastle(board, smove);
        }
    }
    
    if (!NCH_CHKFLG(board->possible_moves[NCH_SQRIDX(from_)], to_)){
        return -1;
    }

    board->enpassant_sqr = 0;

    if (NCH_CHKUNI((board->W_Pawns | board->B_Pawns), *piece_map)){
        NCH_CNGFLG(*piece_map, from_, to_)
        NCH_SETFLG(board->flags, NCH_B_PAWNMOVED);
        if (NCH_CHKFLG(0xff000000000000ff, to_)){
            _CBoard_MakePromotion(board, to_, smove);
        }
        else if ((NCH_CHKFLG(0x00ff00000000ff00, from_) && NCH_CHKFLG(0x000000ffff000000, to_))){
            board->enpassant_sqr = to_;
            _CBoard_Update(board);
            return 0; 
        }
        else if (NCH_CHKFLG(0x0000ff0000ff0000, to_)){
            if (NCH_B_IS_WHITETURN(board)){
                if (!NCH_CHKFLG(board->Black_Map, to_) && NCH_CHKUNI(board->B_Pawns, NCH_NXTSQR_DOWN(to_))){                
                    NCH_RMVFLG(board->B_Pawns, NCH_NXTSQR_DOWN(to_));
                    _CBoard_Update(board);
                    return 0; 
                }
            }
            else{
                if (!NCH_CHKFLG(board->White_Map, to_) && NCH_CHKUNI(board->W_Pawns, NCH_NXTSQR_UP(to_))){
                    NCH_RMVFLG(board->W_Pawns, NCH_NXTSQR_UP(to_));
                    _CBoard_Update(board);
                    return 0; 
                }
            }
        }
    } 

    NCH_CNGFLG(*piece_map, from_, to_)
    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->Black_Map, to_)){
            board->B_Pawns &= ~to_;
            board->B_Knights &= ~to_;
            board->B_Bishops &= ~to_;
            board->B_Rooks &= ~to_;
            board->B_Queens &= ~to_;
        }
    }
    else{
        if (NCH_CHKFLG(board->White_Map, to_)){
            board->W_Pawns &= ~to_;
            board->W_Knights &= ~to_;
            board->W_Bishops &= ~to_;
            board->W_Rooks &= ~to_;
            board->W_Queens &= ~to_;
        }
    }

    _CBoard_Update(board);

    return 0;
}

int CBoard_Step(CBoard* board, char move[]){
    if (strcmp(move, "O-O") == 0 || strcmp(move, "O-O-O") == 0){
        return _CBoard_Step(board, NULL, NCH_NONE, NCH_NONE, strcmp(move, "O-O") == 0 ? NCH_OO : NCH_OOO);
    }
    
    int len = strlen(move);
    if (len < 4){
        return -1;
    }

    cuint64 from_ = NCH_SQR(( 'h' - move[0] ) + 8 * ( move[1] - '1'));
    cuint64 to_ = NCH_SQR(( 'h' - move[2] ) + 8 * ( move[3] - '1'));
    cuint64* piece_map;

    if (NCH_B_IS_WHITETURN(board)){
        if (NCH_CHKFLG(board->W_Pawns, from_)){
            piece_map = &board->W_Pawns;
        }
        else if (NCH_CHKFLG(board->W_Knights, from_)){
            piece_map = &board->W_Knights;
        }
        else if (NCH_CHKFLG(board->W_Bishops, from_)){
            piece_map = &board->W_Bishops;
        }
        else if (NCH_CHKFLG(board->W_Queens, from_)){
            piece_map = &board->W_Queens;
        }
        else if (NCH_CHKFLG(board->W_Rooks, from_)){
            piece_map = &board->W_Rooks;
        }
        else if (NCH_CHKFLG(board->W_King, from_)){
            piece_map = &board->W_King;
        }
        else{
            return -1;
        }
    }
    else{
        if (NCH_CHKFLG(board->B_Pawns, from_)){
            piece_map = &board->B_Pawns;
        }
        else if (NCH_CHKFLG(board->B_Knights, from_)){
            piece_map = &board->B_Knights;
        }
        else if (NCH_CHKFLG(board->B_Bishops, from_)){
            piece_map = &board->B_Bishops;
        }
        else if (NCH_CHKFLG(board->B_Queens, from_)){
            piece_map = &board->B_Queens;
        }
        else if (NCH_CHKFLG(board->B_Rooks, from_)){
            piece_map = &board->B_Rooks;
        }
        else if (NCH_CHKFLG(board->B_King, from_)){
            piece_map = &board->B_King;
        }
        else{
            return -1;
        }
    }

    NCH_SMoves smove = NCH_NoSM;

    if (len > 4){
        switch (move[4])
        {
        case 'q':
            smove = NCH_Promote2Queen;
            break;
        case 'n':
            smove = NCH_Promote2Knight;
            break;
        case 'b':
            smove = NCH_Promote2Bishop;
            break;
        case 'r':
            smove = NCH_Promote2Rook;
            break;
        default:
            smove = NCH_Promote2Queen;
            break;
        }
    }

    return _CBoard_Step(board, piece_map, from_, to_, smove);
}

int CBoard_StepAll(CBoard* board, CBoard boards[]){
    int idx = 0;
    cuint64* pmoves = board->possible_moves;

    if (NCH_B_IS_WHITETURN(board)){
        _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(W_Pawns, NCH_ROW8)
        _NCH_STEP_ALL_POSSIBLE_MOVES(W_Knights)
        _NCH_STEP_ALL_POSSIBLE_MOVES(W_Bishops)
        _NCH_STEP_ALL_POSSIBLE_MOVES(W_Rooks)
        _NCH_STEP_ALL_POSSIBLE_MOVES(W_Queens)
        _NCH_STEP_ALL_POSSIBLE_MOVES(W_King)

        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OO)){
            boards[idx] = *board;
            _CBoard_Step(board + idx, NULL, NCH_NONE, NCH_NONE, NCH_OO);
            idx++;
        }
        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_WHITE_OOO)){
            boards[idx] = *board;
            _CBoard_Step(board + idx, NULL, NCH_NONE, NCH_NONE, NCH_OOO);
            idx++;
        }
    }
    else{
        _NCH_STEP_ALL_POSSIBLE_MOVES_PAWN(B_Pawns, NCH_ROW1)
        _NCH_STEP_ALL_POSSIBLE_MOVES(B_Knights)
        _NCH_STEP_ALL_POSSIBLE_MOVES(B_Bishops)
        _NCH_STEP_ALL_POSSIBLE_MOVES(B_Rooks)
        _NCH_STEP_ALL_POSSIBLE_MOVES(B_Queens)
        _NCH_STEP_ALL_POSSIBLE_MOVES(B_King)

        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OO)){
            boards[idx] = *board;
            _CBoard_Step(board + idx, NULL, NCH_NONE, NCH_NONE, NCH_OO);
            idx++;
        }
        if (NCH_CHKFLG(board->castle_flags, NCH_CF_COULD_BLACK_OOO)){
            boards[idx] = *board;
            printf("Castle Flags Before: 0x%x\n", boards[idx].castle_flags);
            CBoard_Print(board + idx);
            _CBoard_Step(board + idx, NULL, NCH_NONE, NCH_NONE, NCH_OOO);
            idx++;
        }
    }

    return idx;
}

cuint64 _CBoard_PerftRecursive(CBoard* board, int depth){
    if (depth < 2){
        return CBoard_NumberPossibleMoves(board);
    }
    else{
        CBoard boards[256];
        int N = CBoard_StepAll(board, boards);
        cuint64 count = 0;

        for (int i = 0; i < N; i++){
            count += _CBoard_PerftRecursive(boards + i, depth - 1);
        }
        return count;
    }
}

cuint64 CBoard_Perft(CBoard* board, int depth){
    char moves[256][8];
    int N = CBoard_PossibleMovesAsString(board, moves);
    cuint64 total = 0;

    if (depth < 2){
        for (int i = 0; i < N; i++){
            printf("%s: 1\n", moves[i]);
        }
        total = (cuint64)N;
    }
    else{
        cuint64 count;

        CBoard boards[256];
        CBoard_StepAll(board, boards);

        for (int i = 0; i < N; i++){
            count = _CBoard_PerftRecursive(boards + i, depth - 1);
            printf("%s: %llu\n", moves[i], count);
            total += count;
        }
    }
    printf("Perft for depth %i is: %llu\n", depth, total);
}

CBoard* CBoard_New(){
    CBoard* board = malloc(sizeof(CBoard));
    if (!board){
        return NULL;
    }

    board->W_Pawns = NCH_WHITE_PAWNS_START_POS;
    board->B_Pawns = NCH_BLACK_PAWNS_START_POS;

    board->W_Knights = NCH_WHITE_KNIGHTS_START_POS;
    board->B_Knights = NCH_BLACK_KNIGHTS_START_POS;
    
    board->W_Bishops = NCH_WHITE_BISHOPS_START_POS;
    board->B_Bishops = NCH_BLACK_BISHOPS_START_POS;

    board->W_Rooks = NCH_WHITE_ROOKS_START_POS;
    board->B_Rooks = NCH_BLACK_ROOKS_START_POS;
    
    board->W_Queens = NCH_WHITE_QUEEN_START_POS;
    board->B_Queens = NCH_BLACK_QUEEN_START_POS;
    
    board->W_King = NCH_WHITE_KING_START_POS;
    board->B_King = NCH_BLACK_KING_START_POS;

    board->GameDict = _NCH_Ht_New(NCH_GAMEDICT_SIZE);
    if (!board->GameDict){
        free(board);
        return NULL;
    }

    board->flags = 0;
    board->castle_flags = 0;
    board->fifty_count = 0;
    board->move_count = 0;
    board->enpassant_sqr = 0ull;

    _CBoard_Update(board);

    return board;
}

void CBoard_Free(CBoard* board){
    if (board){
        _NCH_Ht_Free(board->GameDict);
        free(board);
    }
}

void _CBoard_FEN_Object2Map(CBoard* board, char* row_str, int row_len, int row){
    char current;
    cuint64 sqr, *piece_map;
    int idx, col = 7;
    for (int i = 0; i < row_len; i++){
        current = row_str[i];

        if (current < '9'){
            col -= current - '0';
            continue;
        }

        idx = row * 8 + col;
        sqr = NCH_SQR(idx); 

        switch (current)
        {
        case 'P':
            piece_map = &board->W_Pawns;
            break;
        
        case 'N':
            piece_map = &board->W_Knights;
            break;

        case 'B':
            piece_map = &board->W_Bishops;
            break;

        case 'R':
            piece_map = &board->W_Rooks;
            break;

        case 'Q':
            piece_map = &board->W_Queens;
            break;

        case 'K':
            piece_map = &board->W_King;
            break;

        case 'p':
            piece_map = &board->B_Pawns;
            break;
        
        case 'n':
            piece_map = &board->B_Knights;
            break;

        case 'b':
            piece_map = &board->B_Bishops;
            break;

        case 'r':
            piece_map = &board->B_Rooks;
            break;

        case 'q':
            piece_map = &board->B_Queens;
            break;

        case 'k':
            piece_map = &board->B_King;
            break;

        default:
            piece_map = NULL;
            break;
        }

        NCH_SETFLG(*piece_map, sqr);
        col--;
    }
}

int _CBoard_FEN_Board(CBoard* board, char* board_str){
    int i = 0;
    char current = board_str[i];
    int row = 7;
    int row_len = 0;
    int start = 0;

    while (current != ' ' && row > -1)
    {
        if (current == '/'){
            _CBoard_FEN_Object2Map(board, board_str + start, row_len, row);
            row--;
            start += row_len + 1;
            row_len = 0;
        }
        else{
            row_len++;
        }
        i++;
        current = board_str[i];
    }
    _CBoard_FEN_Object2Map(board, board_str + start, row_len, row);

    return i;
}

int _CBoard_FEN_Turn(CBoard* board, char turn_str){
    if (turn_str == 'b'){
        NCH_SETFLG(board->flags, NCH_B_TURN);
    }
    else{
        NCH_RMVFLG(board->flags, NCH_B_TURN);
    }
    return 1;
}

int _CBoard_FEN_CastleFlags(CBoard* board, char* flags_str){
    char current;

    for (int i = 0; i < 4; i++)
    {
        current = flags_str[i];

        switch (current)
        {
        case 'K':
            NCH_RMVFLG(board->castle_flags, NCH_CF_WHITE_OO);
            break;

        case 'Q':
            NCH_RMVFLG(board->castle_flags, NCH_CF_WHITE_OOO);
            break;

        case 'k':
            NCH_RMVFLG(board->castle_flags, NCH_CF_BLACK_OO);
            break;

        case 'q':
            NCH_RMVFLG(board->castle_flags, NCH_CF_BLACK_OOO);
            break;
        
        case '-':
            return 1;
            break;

        default:
            return i;
            break;
        }
    }

    return 4;
}

int _CBoard_FEN_EnPassant(CBoard* board, char* enp_str){
    if (enp_str[0] == '-'){
        board->enpassant_sqr = 0ull;
    }
    else{
        board->enpassant_sqr = (NCH_COL1 << ('h' - enp_str[0])) & (NCH_B_IS_WHITETURN(board) ? NCH_ROW5 : NCH_ROW4);
    }
    return 1;
}

int _CBoard_FEN_FiftyMoves(CBoard* board, char* fif_str){
    int idx = 0;
    char current = fif_str[idx];

    while (current != ' ')
    {
        idx++;
        current = fif_str[idx];
    }

    for (int i = idx - 1, j = 1; i > -1; i--, j *= 10){
        board->fifty_count += (j * (fif_str[i] - '0'));
    }
    board->fifty_count--;

    return idx;
}

int _CBoard_FEN_MovesNumber(CBoard* board, char* moves_str){
    int idx = 0;
    char current = moves_str[idx];

    while (current != '\0')
    {
        idx++;
        current = moves_str[idx];
    }

    for (int i = idx - 1, j = 1; i > -1; i--, j *= 10){
        board->move_count += (j * (moves_str[i] - '0'));
    }
    board->move_count;

    return idx;
}

CBoard* CBoard_FromFEN(char* FEN){
    CBoard* board = malloc(sizeof(CBoard));
    if (!board){
        return NULL;
    }

    board->GameDict = _NCH_Ht_New(NCH_GAMEDICT_SIZE);
    if (!board->GameDict){
        free(board);
    }

    board->W_Pawns = 0ull;
    board->W_Knights = 0ull;
    board->W_Bishops = 0ull;
    board->W_Rooks = 0ull;
    board->W_Queens = 0ull;
    board->W_King = 0ull;
    board->B_Pawns = 0ull;
    board->B_Knights = 0ull;
    board->B_Bishops = 0ull;
    board->B_Rooks = 0ull;
    board->B_Queens = 0ull;
    board->B_King = 0ull;

    board->flags = 0;
    board->fifty_count = 0;
    board->castle_flags = NCH_CUINT8_MAX;
    board->move_count = 0;

    int i = 0;
    i += _CBoard_FEN_Board(board, FEN) + 1;
    i += _CBoard_FEN_Turn(board, FEN[i]) + 1;
    i += _CBoard_FEN_CastleFlags(board, FEN + i) + 1;
    i += _CBoard_FEN_EnPassant(board, FEN + i);

    if (FEN[i++] != '\0'){
        i += _CBoard_FEN_FiftyMoves(board, FEN + i);
    }

    if (FEN[i++] != '\0'){
        _CBoard_FEN_MovesNumber(board, FEN + i);
    }
    
    board->White_Map = NCH_B_GET_WHITEMAP(board);
    board->Black_Map = NCH_B_GET_BLACKMAP(board);
    board->All_Map = board->Black_Map | board->White_Map;

    _CBoard_Update(board);

    return board;
}

#endif