#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>
#include "db_manager.h"
sqlite3* initialize_sqlite3(){
    sqlite3 *db;
    if (sqlite3_open("books.db", &db) != SQLITE_OK) {
        fprintf(stderr,"データベースを開けません：%s\n", sqlite3_errmsg(db));
        exit(1);
    }
    const char *create_books_sql =
        "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT,"
        "author TEXT,"
        "pages INTEGER,"
        "publish_date TEXT,"
        "progress INTEGER DEFAULT 0);";

    const char *create_log_sql =
        "CREATE TABLE IF NOT EXISTS progress_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "book_id INTEGER,"
        "date TEXT,"
        "pages INTEGER,"
        "FOREIGN KEY(book_id) REFERENCES books(id));";

    char *err_msg = NULL;
    if (sqlite3_exec(db, create_books_sql, 0, 0, &err_msg) != SQLITE_OK ||
        sqlite3_exec(db, create_log_sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr,"テーブル作成エラー：%s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
    return db;

}
void register_book_to_db(book_t book, sqlite3 *db){
    const char *sql = "INSERT INTO books (title, author, pages, publish_date, progress) VALUES (?, ?, ?, ?, 0);";
    sqlite3_stmt *stmt;

    char *author_str = (char*)malloc(sizeof(char)*256);
    author_str[0] = '\0';
    if(author_str == NULL){
        fprintf(stderr,"malloc err");
    }
    for(int i=0;i < book.number_of_author; i++){
        author_str = strcat(author_str, book.authors[i]);
        if(i != book.number_of_author-1){
            author_str = strcat(author_str,", ");
        }
    }
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, book.title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author_str, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, book.page);
    sqlite3_bind_text(stmt, 4, book.publish_date, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("本を登録しました。\n");
    } else {
        printf("登録に失敗しました：%s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    free(author_str);
}