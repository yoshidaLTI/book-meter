#pragma once
#include <sqlite3.h>
#include <stdbool.h>


// dbに格納する本情報をまとめた構造体
typedef struct{
    char *title;
    char **authors;
    int number_of_author;
    int page;
    char *publish_date;
    int progress;
}book_t;

// book_tの作成と削除をする関数群
book_t* create_new_book();
void destroy_book(book_t *destroyed_book);

// dbに本情報を追加する関数
void register_book_to_db(book_t book, sqlite3 *db);

typedef struct node_t{
    int start_page;
    int end_page;
    bool is_read;
    struct node_t *previous_node;
    struct node_t *next_node;
}node_t;
void update_progress_to_db(sqlite3 *db,int id);

sqlite3* initialize_sqlite3();
