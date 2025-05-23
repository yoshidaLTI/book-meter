#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdbool.h>
#include "db_manager.h"

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
        // TODO: データベースに保存する処理を書く
        register_book_to_db(book, db);
    }
}

void list_books(sqlite3 *db) {
    const char *sql = "SELECT id, title, author, pages, publish_date, progress FROM books;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    printf("登録された本の一覧：\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        printf("id %d\n", id);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        printf("title %s\n", title);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        printf("author %s\n", author);
        int pages = sqlite3_column_int(stmt, 3);
        const unsigned char *publish_date = sqlite3_column_text(stmt, 4);
        int progress = sqlite3_column_int(stmt, 5);

        printf("[%d] %s | %s | %dページ | 発行日: %s | 読了: %dページ\n", id, title, author, pages, publish_date, progress);
    }

    sqlite3_finalize(stmt);
}

void update_progress(sqlite3 *db) {
    int id = 0;
    char buf[4096];
    printf("対象の本のIDを入力してください：");
    if(fgets(buf, 4096, stdin) != NULL){
        id = atoi(buf);
    }else{
        fprintf(stderr,"err\n");
        return;
    }
    update_progress_to_db(db,id);//進捗のデータベース書き込み処理
}


int main() {
    char buffer[512];
    char command[256];
    sqlite3 *db = initialize_sqlite3();

    printf("コマンドを入力してください（[Ctrl]+C または exit で終了）：\n");
    print_help();

    while (1) {
        printf("command>> ");

        // fgetsで1行読み取り
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        // 改行を除去
        buffer[strcspn(buffer, "\n")] = '\0';
        if (sscanf(buffer, "%255s", command) != 1) {
            continue;
        }

        // コマンド分岐
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