#include <stdio.h>
#include "debug_utils.h"

/* ページ範囲リストの中身を標準出力にダンプするデバッグ用の関数 */
void dump_page_range_list(range_node_t *range_node){

    if(range_node->start_page == range_node->end_page){
        if(range_node->start_page != -1)
            printf("| %d |", range_node->start_page);
    }else{
        printf("| %d - %d |", range_node->start_page, range_node->end_page);
    }

    if(range_node->next != NULL){
        printf("->");
        dump_page_range_list(range_node->next);
    }
}

/* ページ範囲リストの中身を標準出力に逆順にダンプするデバッグ用の関数 */
void dump_page_range_list_reverse(range_node_t *range_node){

    if(range_node->start_page == range_node->end_page){
        if(range_node->start_page != -1)
            printf("| %d |", range_node->start_page);
    }else{
        printf("| %d - %d |", range_node->start_page, range_node->end_page);
    }
    
    if(range_node->previous != NULL){
        printf("->");
        dump_page_range_list_reverse(range_node->previous);
    }

}

