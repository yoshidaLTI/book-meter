#pragma once 

#include "book.h"
#include "db_manager.h"

/* ページ範囲リストの中身を標準出力にダンプするデバッグ用の関数 */
void dump_page_range_list(range_node_t *range_node);

/* ページ範囲リストの中身を標準出力に逆順にダンプするデバッグ用の関数 */
void dump_page_range_list_reverse(range_node_t *range_node);
