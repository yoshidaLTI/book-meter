#pragma once
#include <sqlite3.h>
typedef struct{
    char *title;
    char **authors;
    int number_of_author;
    int page;
    char *publish_date;
    int progress;
}book_t;

typedef struct node_t{
    int start_page;
    int end_page;
    bool is_read;
    struct node_t *previous_node;
    struct node_t *next_node;
}node_t;

sqlite3* initialize_sqlite3();
void register_book_to_db(book_t book, sqlite3 *db);
void update_progress_to_db(sqlite3 *db,int id);

