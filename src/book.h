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


