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

sqlite3* initialize_sqlite3();
void register_book_to_db(book_t book, sqlite3 *db);

