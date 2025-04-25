#include <stdio.h>
#include <sqlite3.h>
void list_books(sqlite3 *db) {
    const char *sql = "SELECT id, title, author, pages, publish_date, progress FROM books;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    printf("登録された本の一覧：\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        int pages = sqlite3_column_int(stmt, 3);
        const unsigned char *publish_date = sqlite3_column_text(stmt, 4);
        int progress = sqlite3_column_int(stmt, 5);

        printf("[%d] %s | %s | %dページ | 発行日: %s | 読了: %dページ\n", id, title, author, pages, publish_date, progress);
    }

    sqlite3_finalize(stmt);
}