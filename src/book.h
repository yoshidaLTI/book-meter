#pragma once 

#include <sqlite3.h>

// book_meterの主要な機能を提供する関数群
void print_help();
void add_book(sqlite3 *db);
void list_books(sqlite3 *db); 
void update_progress(sqlite3 *db);



// update_progressにおいて，既読ページを管理するための構造体
typedef struct range_node{
    int start_page;
    int end_page;
    struct range_node *previous;
    struct range_node *next;
}range_node_t;

// update_progressにおいて，既読ページのリストを操作するための関数群
range_node_t* seek_last_node(range_node_t *range_node);
range_node_t* append_range_node(range_node_t *head, range_node_t *new_node);
range_node_t* seek_swap_node(range_node_t *range_node, int start_page);
void sort_range_list(range_node_t *head, range_node_t *current_node);


// update_progressのデバッグに利用している表示関数 
void dump_page_range_list(range_node_t *range_node);
void dump_page_range_list_reverse(range_node_t *range_node);

