#pragma once 

#include "book.h"
#include "db_manager.h"

/* ページ範囲リストの終端要素を返す関数 */
range_node_t* seek_last_node(range_node_t *range_node);

/* ページ範囲リストの末尾に新しいページ範囲要素を追加する関数 */
range_node_t* append_range_node(range_node_t *head, range_node_t *new_node);

/* 
	NOTE: リスト全体をソートするためには，sort_range_list_one_roundを
		  リストの要素数回実行する必要がある．この処理はsort_range_listに実装
		　されているので，通常はsort_range_litを使えば良い．
*/

/* ページ範囲リストを昇順にソートする関数*/
range_node_t* sort_range_list(range_node_t *head, int number_of_ranges);

/* ページ範囲リストを一巡して，要素を入れ替えていく関数 */ 
void sort_range_list_one_round(range_node_t *head, range_node_t *current_node);

/* ソート中に現在のノードを入れ替えるべきノードを探す関数 */
range_node_t* seek_swappable_node(range_node_t *range_node, int pivot_page);

char* cat_range_string(range_node_t *range_node, char *range_str);


/*
    NOTE: リスト全体の重複ノードを排除するためには，
    eliminate_redundant_node_one_roundをリストの要素数回実行する必要がある．
    この処理は，eliminate_redundant_nodesに実装されているので，通常は
    eliminate_redundant_nodesを使えば良い．
 */

/* ページ範囲リストのページの重複を排除する関数 */
range_node_t* eliminate_redundant_nodes(range_node_t* head,
                                        int initial_number_of_ranges);

/* ページ範囲リストを一巡して，ページの重複を排除していく関数 */
int eliminate_redundant_nodes_one_round(range_node_t *current_node,
                                        int number_of_ranges);

/* 重複要素の排除処理中に，重複が発生しているノードを探す関数 */ 
range_node_t *seek_redundant_node(range_node_t *current_node,
							  	  range_node_t *next_node);
