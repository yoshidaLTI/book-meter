#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include "add_book.h"
void add_book(sqlite3 *db) {
    char title[256], author[256], publish_date[64];
    int pages;

    printf("本のタイトルを入力してください：");
    getchar(); // 改行を消す
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("作者名を入力してください：");
    fgets(author, sizeof(author), stdin);
    author[strcspn(author, "\n")] = '\0';

    printf("ページ数を入力してください：");
    scanf("%d", &pages);
    getchar(); // 改行を消す
    
    printf("発行日を入力してください（例：2025-04-23）：");
    fgets(publish_date, sizeof(publish_date), stdin);
    publish_date[strcspn(publish_date, "\n")] = '\0';



    const char *sql = "INSERT INTO books (title, author, pages, publish_date, progress) VALUES (?, ?, ?, ?, 0);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, pages);
    sqlite3_bind_text(stmt, 4, publish_date, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
        printf("本を登録しました。\n");
    } else {
        printf("登録に失敗しました：%s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
}