#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>

#include "db_manager.h"

#define DB_NAME "book.db"

/* 本の記録用のデータベースを作成する初期化関数*/
sqlite3* initialize_sqlite3(){
    sqlite3 *db;
    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) {
        fprintf(stderr,"データベースを開けません：%s\n", sqlite3_errmsg(db));
        exit(1);
    }

    /* FIXME: 2種類のテーブルを管理している理由が不明 */

    /* add_book()で利用する本の追加用テーブル*/
    const char *create_books_sql =
        "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "   // 本のid 
        "title TEXT, "                             // タイトル
        "author TEXT, "                            // 著者 (複数の可能性あり)
        "pages INTEGER, "                          // ページ数
        "publish_date TEXT, "                      // 出版日
        "progress INTEGER DEFAULT 0, "             // 進捗率
        "progress_str TEXT"                        // 進捗ページ
        ");";

    /* update_progress()で読書進捗更新用テーブル*/
    const char *create_log_sql =
        "CREATE TABLE IF NOT EXISTS progress_log (" 
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"    
        "book_id INTEGER,"                         // 本のid
        "date TEXT,"                               // ?? 不明　これは何?
        "pages INTEGER,"                           // 進捗率
        "FOREIGN KEY(book_id) REFERENCES books(id)"
        ");";

    /* tableの作成 */
    char *err_msg = NULL;
    if ((sqlite3_exec(db, create_books_sql, 0, 0, &err_msg) != SQLITE_OK)
            || (sqlite3_exec(db, create_log_sql, 0, 0, &err_msg) != SQLITE_OK))
    {
        fprintf(stderr,"テーブル作成エラー：%s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(1);
    }
    return db;
}


/* 本情報をテーブルに登録する関数 */
void register_book_to_db(book_t book, sqlite3 *db){

    /* FIXME : この処理を書くべき場所は明らかにここではない */
    /* 著者情報のシリアライズ */
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

    /* 本の追加用SQL */
    const char *register_book_sql = "INSERT INTO books (title, author, pages,\
                                     publish_date, progress, progress_str)\
                                     VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    /* 値のセット */
    sqlite3_prepare_v2(db, register_book_sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, book.title,        -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author_str,        -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 3, book.page);
    sqlite3_bind_text(stmt, 4, book.publish_date, -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 5, 0);
    sqlite3_bind_text(stmt, 6, "",                -1, SQLITE_STATIC);

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

void update_progress_to_db(sqlite3 *db, int id ,char *new_progress){
    // idは 0
    // sqlite3_bind_text(stmt, 1, book.title, -1, SQLITE_STATIC);
    // sqlite3_bind_text(stmt, 2, author_str, -1, SQLITE_STATIC);
    // sqlite3_bind_int(stmt,  3, book.page);
    // sqlite3_bind_text(stmt, 4, book.publish_date, -1, SQLITE_STATIC);
    // sqlite3_bind_int(stmt,  5, 0);
    // sqlite3_bind_text(stmt, 6, "", -1, SQLITE_STATIC);
    
    int total_pages = 0;
    int flag;

    char *current_progress = (char*)malloc(sizeof(char)*1024);
    if(current_progress == NULL){
        fprintf(stderr, "could not allocate sufficient memory for current_progress\n");
        exit(EXIT_FAILURE);
    }
    char *done_pages = (char*)malloc(sizeof(char)*1024);
    if(done_pages == NULL){
        fprintf(stderr, "could not allocate sufficient memory for done_pages\n");
        exit(EXIT_FAILURE);
    }
    sqlite3_stmt *stmt;

    const char *select_sql = "SELECT pages, progress_str FROM books WHERE id = ?;";
    sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    if(sqlite3_step(stmt) == SQLITE_ROW){
        total_pages = sqlite3_column_int(stmt, 0);
        current_progress = sqlite3_column_text(stmt, 1);
        sqlite3_finalize(stmt);
    
    strcat(new_progress,current_progress); //範囲の足し合わせ
    
    //TODO　統合後の読了ページ数を算出する関数を用意
    //totalpages = somefunc();

    const char *update_sql = "UPDATE books SET progress_str = ? WHERE id = ?;";
    sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, new_progress, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    }
    else
    {
        printf("本が見つかりませんでした。\n");
        sqlite3_finalize(stmt);
    }

}
