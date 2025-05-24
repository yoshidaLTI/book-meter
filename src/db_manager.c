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

// book_t型のnew bookを作成して初期化した状態で返す関数
book_t* create_new_book(){

    book_t* new_book;
    new_book = (book_t*)malloc(sizeof(book_t));
    if(new_book == NULL){
        fprintf(stderr, "could not allocate sufficient memory for new_book\n");
        exit(EXIT_FAILURE);
    }

    // これ以降でnew_bookの初期化をする
   
    new_book->title = (char *)malloc(sizeof(char)* 1024);
    if(new_book->title == NULL){
        fprintf(stderr, "could not allocate sufficient memory for title!\n");
        exit(EXIT_FAILURE);
    }

    // 10は暫定的な値です 
    new_book->authors = (char**)malloc(sizeof(char *) * 10);
    if(new_book->authors == NULL){
        fprintf(stderr, "could not allocate sufficient memory for authors\n");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<10; i++){
        new_book->authors[i] = (char *)malloc(sizeof(char)*1024);
        if(new_book->authors[i] == NULL){
            fprintf(stderr, "malloc error!\n");
            exit(EXIT_FAILURE);
        }
    }

    new_book->number_of_author = 0;
    new_book->page = 0;
    
    new_book->publish_date = (char *)malloc(sizeof(char)* 1024);
    if(new_book->publish_date == NULL){
        fprintf(stderr, "could not allocate sufficient memory for publish_date\n");
        exit(EXIT_FAILURE);
    }

    new_book->progress = 0;

    return new_book;
}

// 不要になったbook_tを破棄するための関数です
void destroy_book(book_t *destroyed_book){

    free(destroyed_book->title);

    // 中身の要素に割り当てられているメモリを解放した後に，それを指している
    // ポインタ配列を解放します
    for(int i=0; i<destroyed_book->number_of_author; i++){
        if(destroyed_book->authors[i] != NULL)
            free(destroyed_book->authors[i]);
    }
    free(destroyed_book->authors);
    free(destroyed_book->publish_date);

    free(destroyed_book);
}
void set_node(int start, int end ,node_t *head){

}

/*void update_progress_to_db(sqlite3 *db, int id){*/
    /*char *read_today =(char *)malloc(sizeof(char)*4096);*/
    /*int current_progress, total_pages;*/
    /*sqlite3_stmt *stmt;*/
    /*const char *select_sql = "SELECT pages, progress FROM books WHERE id = ?;";*/
    /*if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL) != SQLITE_OK) {*/
        /*fprintf(stderr, "SQL準備失敗: %s\n", sqlite3_errmsg(db));*/
        /*return;*/
    /*}*/
    /*sqlite3_bind_int(stmt, 1, id); // id をバインド*/

    /*node_t *head;*/
    /*//本が全部で何ページか取得  すでに読まれている区間を取得*/
    /*if (sqlite3_step(stmt) == SQLITE_ROW) {*/
        /*total_pages = sqlite3_column_int(stmt, 0);*/

        /*current_progress = sqlite3_column_int(stmt, 1);*/
        /*sqlite3_finalize(stmt);*/



        /*printf("今日読んだページを入力\n：例）1ページと50ページのみ読んだ場合 1,50\n");*/
        /*printf("例）1ページから50ページの区間を読んだ場合 1-100\n");*/
        /*if(fgets(read_today, 4096, stdin) == NULL){*/
            /*return ;*/
        /*}*/
        /*int index = 0;*/
        /*int next = 0;*/
        /*int start = 0;*/
        /*int end = 0;*/
        /*while(index < 4096 && read_today[index] != '\0'){*/
            /*next = index;*/
            /*while(next <4096 && read_today[next] != '\0'){*/
                /*if(read_today[next]==',' || read_today[next] == '\0'){*/
                    /*read_today[next]='\0';*/
                    /*end = atoi(read_today[index]);*/
                    /*break;*/
                /*}*/
                /*if(read_today[next]=='-'){*/
                    /*read_today[next]='\0';*/
                    /*start = atoi(read_today[index]);*/
                    /*break;*/
                /*}*/
                /*next++;*/
            /*}*/
            /*if(start == 0){*/
                /*start = end;*/
            /*}*/
            /*if(end != 0){*/
                /*printf("start:%d,\n end:%d\n",start,end);*/
                /*set_node(start, end, head);*/
                /*start = 0;*/
                /*end = 0;                */
            /*}*/
            /*index = next+1;*/
            
        /*}*/
    /*}*/
/*
        int new_progress = current_progress + read_today;
        if (new_progress > total_pages) new_progress = total_pages;

        const char *update_sql = "UPDATE books SET progress = ? WHERE id = ?;";
        sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, new_progress);
        sqlite3_bind_int(stmt, 2, id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char date[32];
        snprintf(date, sizeof(date), "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

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
    




    //既読区間を追加する処理







    const char *select_sql = "SELECT pages, progress FROM books WHERE id = ?;";
    sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
*/


//}
