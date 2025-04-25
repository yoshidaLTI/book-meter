#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include "add_book.h"
#include "print_help.h"
#include "update_progress.h"
#include "list_book.h"

int main() {
    char command[256];
    sqlite3 *db;

    if (sqlite3_open("books.db", &db) != SQLITE_OK) {
        printf("データベースを開けません：%s\n", sqlite3_errmsg(db));
        return 1;
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
        printf("テーブル作成エラー：%s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    printf("コマンドを入力してください（[Ctrl]+C または exit で終了）：\n");
    print_help();

    while (1) {
        printf("command>> ");
        if (scanf("%255s", command) != 1) {
            printf("入力エラーです。\n");
            continue;
        }

        if (strcmp(command, "add") == 0) {
            add_book(db);
        } else if (strcmp(command, "list") == 0) {
            list_books(db);
        } else if (strcmp(command, "progress") == 0) {
            update_progress(db);
        } else if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strcmp(command, "exit") == 0) {
            printf("終了します。\n");
            break;
        } else {
            printf("不明なコマンドです。helpで使用可能なコマンドを確認できます。\n");
        }
    }

    sqlite3_close(db);
    return 0;
}

