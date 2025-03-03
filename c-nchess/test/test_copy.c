#include "nchess.h"
#include "./base.h"

int
is_movenodes_same(const MoveNode* n1, const MoveNode* n2){
    int res =  !memcmp(&n1->pos_info, &n2->pos_info, sizeof(n1->pos_info))
            && (n1->move == n2->move);

    return res;
}

int
is_dictnodes_same(const BoardNode* n1, const BoardNode* n2){
    int res =  !memcmp(n1->bitboards, n2->bitboards, sizeof(n1->bitboards))
            && (n1->count == n2->count)
            && (n1->empty == n2->empty);

    return res;
}

int
check_dict_same_recursive(const BoardNode* n1, const BoardNode* n2){
    if (!n1 && !n2)
        return 1;
    
    if (n1 && n2){
        if (!is_dictnodes_same(n1, n2))
            return 0;

        if (n1->next)
            return check_dict_same_recursive(n1->next, n2->next);

        return 1;
    }

    return 0;
}

int
is_boards_same(Board* b1, Board* b2){
    // check movelist
    if (b1->movelist.len != b2->movelist.len)
        return 0;
    
    int max_len = NCH_MOVELIST_SIZE > b1->movelist.len ? 
                  b1->movelist.len : NCH_MOVELIST_SIZE;

    MoveNode *mn_1, *mn_2;
    for (int i = 0; i < max_len; i++){
        mn_1 = &b1->movelist.nodes[i];
        mn_2 = &b2->movelist.nodes[i];

        if (!is_movenodes_same(mn_1, mn_2))
            return 0;
    }

    if (b1->movelist.extra){
        if (!b2->movelist.extra)
            return 0;
    
        mn_1 = b1->movelist.extra;
        mn_2 = b2->movelist.extra;

        while (mn_1 && mn_2)
        {
            if (!is_movenodes_same(mn_1, mn_2))
                return 0;
            
            mn_1 = mn_1->next;
            mn_2 = mn_2->next;
        }

        if (mn_1 || mn_2)
            return 0;
    }

    // check dict
    int res;
    const BoardNode *bn_1, *bn_2;
    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
        bn_1 = b1->dict.nodes + i;
        bn_2 = b2->dict.nodes + i;

        if (!is_dictnodes_same(bn_1, bn_1))
            return 0;

        if (bn_1->next){
            res = check_dict_same_recursive(&b1->dict.nodes[i], &b2->dict.nodes[i]);
            if (!res)
                return 0;
        }
    }

    res  = (!memcmp(b1->bitboards, b2->bitboards, sizeof(b1->bitboards)))
        && (!memcmp(b1->occupancy, b2->occupancy, sizeof(b1->occupancy)))
        && (!memcmp(b1->piecetables, b2->piecetables, sizeof(b1->piecetables)))
        && (!memcmp(&b1->info, &b2->info, sizeof(PositionInfo)))
        && (b1->nmoves == b2->nmoves);

    return res;
}

int
test_copy_1(){
    Board* board = Board_New();
    if (!board)
        return 0;

    Board* copy_board = Board_NewCopy(board);
    if (!copy_board){
        Board_Free(board);
        return 0;
    }

    int res = is_boards_same(board, copy_board);

    Board_Free(board);
    Board_Free(copy_board);

    return res;
}

int
test_copy_2(){
    Board* board = Board_New();
    if (!board)
        return 0;

    Board* copy_board = Board_NewCopy(board);
    if (!copy_board){
        Board_Free(board);
        return 0;
    }

    Move moves[256];
    Board_GenerateLegalMoves(copy_board, moves);

    Board_StepByMove(board, moves[0]);

    int res = !is_boards_same(board, copy_board);

    Board_Free(board);
    Board_Free(copy_board);

    return res;
}

int
test_copy_3(){
    Board* board = Board_New();
    if (!board)
        return 0;

    Board* copy_board = Board_NewCopy(board);
    if (!copy_board){
        Board_Free(board);
        return 0;
    }

    Move moves[256], move;
    int nmoves;
    for (int i = 0; i < 500; i++){
        nmoves = Board_GenerateLegalMoves(copy_board, moves);
        if (!nmoves)
            break;

        move = moves[0];

        Board_StepByMove(board, move);
        Board_StepByMove(copy_board, move);
    }

    int res = is_boards_same(board, copy_board);

    Board_Free(board);
    Board_Free(copy_board);

    return 1;
}

int
test_copy_4(){
    Board* board = Board_New();
    if (!board)
        return 0;

    Board* copy_board = Board_NewCopy(board);
    if (!copy_board){
        Board_Free(board);
        return 0;
    }

    Board_BB(copy_board, NCH_WPawn) = NCH_BOARD_B_QUEEN_STARTPOS; 

    int res = !is_boards_same(board, copy_board);

    Board_Free(board);
    Board_Free(copy_board);

    return res;
}

void test_copy_main(int init_bb){
    if (init_bb)
        NCH_Init();

    testfunc funcs[] = {
        test_copy_1,
        test_copy_2,
        test_copy_3,
        test_copy_4,
    };

    test_all("Copy", funcs, 4);
}