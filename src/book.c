#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    char *title;
    char **authors;
    int number_of_author;
    int page;
    char *publish_date;
    int progress;
}book_t;


enum input_mode{
    TITLE, AUTHOR, PAGE, PUBLISH_DATE
};

//1
void print_help() {
    printf("使用可能なコマンド:\n");
    printf("  add   - 本を追加\n");
    printf("  list  - 登録された本の一覧を表示\n");
    printf("  progress - 読書進捗を記録・表示\n");
    printf("  help  - ヘルプを表示\n");
    printf("  exit  - プログラムを終了\n");
}



void add_book(sqlite3 *db){

    enum input_mode mode;
    mode = TITLE;
    book_t book;

    book.authors = (char**)malloc(sizeof(char *) * 10);
    book.number_of_author = 0;

    bool exit_data = false;
    bool is_first = true;

    while(!exit_data){

        // prompt 
        switch(mode){
            case TITLE:
                printf("本のタイトルを入力してください：(終了時はquit)");
                break;
            case AUTHOR:
                printf("作者名を入力してください：(終了時はquit)");
                break;
            case PAGE:
                printf("ページ数を入力してください：(終了時はquit)");
                break;
            case PUBLISH_DATE:
                printf("発行日を入力してください（例：2025-04-23）：");
                break;
        }
    
        // get user's input 
        char *line = (char *)malloc(sizeof(char)*1024);

        if(is_first){
            int c;
            while((c = getchar()) !='\n'){}    
            is_first = false;
        }

        if(fgets(line, 1024, stdin) != NULL){
            line[strcspn(line, "\n")] = '\0';
        }

        // quit
        if(strcmp(line, "quit")==0)
            return;

        // store 
        switch(mode){
            case TITLE:
                book.title = (char *)malloc(sizeof(char)* 1024);
                if(book.title == NULL){
                    fprintf(stderr, "malloc error!\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(book.title, line);
                mode = AUTHOR;
                break;
            case AUTHOR:
                if(strlen(line) < 1){
                    mode = PAGE;
                    break;
                }
                // add author 
                book.authors[book.number_of_author] = (char *)malloc(sizeof(char)*1024);
                if(book.authors[book.number_of_author] == NULL){
                    fprintf(stderr, "malloc error!\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(book.authors[book.number_of_author], line);
                book.number_of_author++;
                break;
            case PAGE:
                book.page = atoi(line);
                mode = PUBLISH_DATE;                
                break;
            case PUBLISH_DATE:
                book.publish_date = (char *)malloc(sizeof(char)* 1024);
                if(book.publish_date == NULL){
                    fprintf(stderr, "malloc error!\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(book.publish_date, line);
                exit_data = true;
                break;
        }

        free(line);
    }

    if(exit_data){
    // store book information to db 

    }
}


//2
/*
void add_book(sqlite3 *db) {
    book_t book;
    char *tmp =(char*)malloc(1024);
    int flag = 1;
    book.title = (char*)malloc(1024);
    if(book.title==NULL){
        fprintf(stderr,"メモリ確保失敗");
        exit(EXIT_FAILURE);
    }    
    book.authors = (char**)malloc(sizeof(char*)*5);
    if(book.authors == NULL){
        fprintf(stderr,"メモリ確保失敗");
        exit(EXIT_FAILURE); 
    }

    book.publish_date = (char*)malloc(15);
    if(book.authors == NULL){
        fprintf(stderr,"メモリ確保失敗");
        exit(EXIT_FAILURE); 
    }
    
    book.number_of_author = 0;
    
    char *name =(char*)malloc(100);    
    if(name == NULL){
        fprintf(stderr,"メモリ確保失敗");
        exit(EXIT_FAILURE); 
    }


    
    
    printf("本のタイトルを入力してください：(終了時はquit)");
    getchar(); // 改行を消す
    fgets(tmp, sizeof(1024), stdin);
    if(strcmp(tmp,"quit")==0){
        flag = 0;
        free(tmp);
        free(book.title);
        free(book.authors);
        free(name);
        free(book.publish_date);
        return;
    }
    book.title = tmp;
        
    book.title[strcspn(book.title, "\n")] = '\0';
    
    
    while(1){   
        printf("作者名を入力してください：(終了時はquit)");
        getchar(); // 改行を消す
        fgets(tmp, sizeof(100), stdin);
        if(strcmp(tmp,"quit")==0){
            flag = 0;
            free(tmp);
            free(book.title);
            free(book.authors);
            free(name);
            free(book.publish_date);
            return;
        }  
        if(strcmp(tmp,"\n")==0){
            break;
        }  
        name = tmp;
        name[strcspn(name, "\n")] = '\0';
    }
    
    printf("ページ数を入力してください：(終了時はquit)");
    getchar(); // 改行を消す
    fgets(tmp,sizeof(100),stdin);
    if(strcmp(tmp,"quit")==0){
        flag = 0;
        free(tmp);
        free(book.title);
        free(book.authors);
        free(name);
        free(book.publish_date);
        return;
    }
    getchar(); // 改行を消す
    
    book.page = atoi(tmp);//atoi():*char型のtmpをint型へ変更する
    
    printf("発行日を入力してください（例：2025-04-23）：");
    getchar(); // 改行を消す
    fgets(tmp, sizeof(book.publish_date), stdin);
    if(strcmp(tmp,"quit")==0){
        flag = 0;
        free(tmp);
        free(book.title);
        free(book.authors);
        free(name);
        free(book.publish_date);
        return;
    } 
    book.publish_date = tmp; 
    book.publish_date[strcspn(book.publish_date, "\n")] = '\0';
    


    
    const char *sql = "INSERT INTO books (title, author, pages, publish_date, progress) VALUES (?, ?, ?, ?, 0);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    //addbook_db
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
*/
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

//3
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
//4
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


int main() {
    char command[256];
    sqlite3 *db = initialize_sqlite3();

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