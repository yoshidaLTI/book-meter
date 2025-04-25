#include <stdio.h>
#include <sqlite3.h>
#include <time.h>
void update_progress(sqlite3 *db) {
    int id, read_today, current_progress, total_pages;
    sqlite3_stmt *stmt;

    printf("対象の本のIDを入力してください：");
    scanf("%d", &id);

    const char *select_sql = "SELECT pages, progress FROM books WHERE id = ?;";
    sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total_pages = sqlite3_column_int(stmt, 0);
        current_progress = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);

        printf("今日読んだページ数を入力してください：");
        scanf("%d", &read_today);

        int new_progress = current_progress + read_today;
        if (new_progress > total_pages) new_progress = total_pages;

        const char *update_sql = "UPDATE books SET progress = ? WHERE id = ?;";
        sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, new_progress);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // 日付取得
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char date[32];
        snprintf(date, sizeof(date), "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        // 進捗履歴に記録
        const char *insert_log = "INSERT INTO progress_log (book_id, date, pages) VALUES (?, ?, ?);";
        sqlite3_prepare_v2(db, insert_log, -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_text(stmt, 2, date, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, read_today);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        float percent = (float)new_progress / total_pages * 100;
        int bars = (int)(percent / 5);

        printf("現在の進捗状況: %d/%dページ (%.2f%%)\n", new_progress, total_pages, percent);
        printf("[");
        for (int i = 0; i < 20; i++) {
            if (i < bars) printf("#");
            else printf(" ");
        }
        printf("]\n");
    } else {
        printf("本が見つかりませんでした。\n");
        sqlite3_finalize(stmt);
    }
}