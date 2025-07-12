#include<stdio.h>
#include<string.h>

#include "book_processor.h"

/* ページ範囲リストの終端要素を返す関数 */
range_node_t* seek_last_node(range_node_t *range_node){
    range_node_t *target_node = NULL;
    
    /* リストを末尾まで辿る */ 
    if(range_node->next == NULL){
        target_node = range_node; 
    }else{
        target_node =  seek_last_node(range_node->next);            
    }
   return target_node;
}

/* ページ範囲リストの末尾に新しいページ範囲要素を追加する関数 */
range_node_t* append_range_node(range_node_t *head, range_node_t *new_node){
    range_node_t* last_node;

    /* リストの末尾に追加 */
    last_node          = seek_last_node(head);
    last_node->next    = new_node;
    new_node->previous = last_node;

    return head;
}

/* ページ範囲リストを昇順にソートする関数*/
range_node_t* sort_range_list(range_node_t *head, int number_of_ranges){

	/* 要素数の分だけsort_range_list_one_round()を呼び，リスト全体をソート */
    for(int i=0; i<number_of_ranges; i++)
        sort_range_list_one_round(head, head);
   return head;
}

/* ページ範囲リストを一巡して，要素を入れ替えていく関数 */ 
void sort_range_list_one_round(range_node_t *head, range_node_t *current_node){
   
    /* 要素がない場合はソートしない */
    if(current_node->next == NULL){
        return;
    }

    range_node_t* swapped_node;
    swapped_node = seek_swappable_node(current_node, current_node->start_page);

    if(swapped_node != NULL){
        int temp_start_page, temp_end_page;
        temp_start_page = current_node->start_page;
        temp_end_page = current_node->end_page;

        current_node->start_page =  swapped_node->start_page;
        current_node ->end_page =  swapped_node -> end_page;

        swapped_node->start_page = temp_start_page;
        swapped_node->end_page = temp_end_page;
    }

    sort_range_list_one_round(head, current_node->next);
}

/* ソート中に現在のノードを入れ替えるべきノードを探す関数 */
range_node_t* seek_swappable_node(range_node_t *range_node,
								  int pivot_page){
    range_node_t *target_node = NULL;
    
	/*
	  ページ範囲リストは昇順にソートされるべきなので，置き換え元
	  のページ(pivot_page)より小さければ入れ替え可能ということになる
 	*/
    if((range_node->start_page != -1) 
			&& ((range_node->start_page) < pivot_page)){
        target_node = range_node; 

    }else if(range_node-> next != NULL){
        target_node = seek_swappable_node(range_node->next, pivot_page);            
    }

    return target_node;
}

// ノードの範囲を文字列型にする
char* cat_range_string(range_node_t *range_node, char *range_str){
    char page_range_buckups[1024] = {}; 
    strcpy(page_range_buckups,range_str);
    if(range_node->start_page == range_node->end_page){ //not range
        if(range_node->start_page != -1)
            sprintf(range_str," %s,%d", page_range_buckups,range_node->start_page);
    }else{
        sprintf(range_str,"%s,%d-%d", page_range_buckups,range_node->start_page, range_node->end_page);
    }
    if(range_node->next != NULL){
        cat_range_string(range_node->next, range_str);
    }
    return(range_str);
}

/* ページ範囲リストのページの重複を排除する関数 */
range_node_t* eliminate_redundant_nodes(range_node_t* head,
                                        int initial_number_of_ranges){
    int number_of_ranges = initial_number_of_ranges;

    /* 要素数分だけeliminate_redundant_nodes_one_round()を呼び，重複を排除 */
    for(int i=0; i<number_of_ranges+1; i++){
        number_of_ranges
            = eliminate_redundant_nodes_one_round(head, 
                                                  number_of_ranges);
    }
    return head;
}

/* ページ範囲リストを一巡して，ページの重複を排除していく関数 */
int eliminate_redundant_nodes_one_round(range_node_t *current_node,
                                        int number_of_ranges){
    /* head_nodeについては読み飛ばす*/
    if(current_node->start_page == -1)
        number_of_ranges
            = eliminate_redundant_nodes_one_round(current_node->next,
                                                  number_of_ranges);
        
    /* リストの末端に辿り着いたら終了 */
    if(current_node->next == NULL){
        return number_of_ranges;
    }


    range_node_t *redundant_node =NULL;
    redundant_node = seek_redundant_node(current_node, current_node->next);

    /* 重複の排除*/ 
    if(redundant_node != NULL){

        /* 内包のケース */ 
        if(current_node->start_page <= redundant_node->start_page 
            && current_node->end_page >= redundant_node->end_page){
            current_node->next = redundant_node->next;
            return --number_of_ranges;
        }

        /* 隣接ノード間で重複があるケース */
        if(current_node->end_page+1 >= redundant_node->start_page){
            current_node->end_page = redundant_node->end_page;
            current_node->next = redundant_node->next;
            return --number_of_ranges;
        }
    }

    /* リストの末端まで再起的に実行 */
    number_of_ranges
        = eliminate_redundant_nodes_one_round(current_node->next,
                                              number_of_ranges);
    return number_of_ranges;
}

/* 重複要素の排除処理中に，重複が発生しているノードを探す関数 */ 
range_node_t *seek_redundant_node(range_node_t *current_node,
							  range_node_t *next_node){
    range_node_t* target_node = NULL;

    if((current_node->end_page+1) >= next_node->start_page ){
        target_node = next_node;
        return target_node;
    }
    if(next_node->next != NULL){
        //next range
        target_node = seek_redundant_node(current_node->next, next_node->next);
    }
    return target_node;
}
