#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "book.h"
#include "db_manager.h"
#include "book_processor.h"
#include "debug_utils.h"


int main() {
    char user_input[512];
    char command[256];
    sqlite3 *db = initialize_sqlite3();

    printf("コマンドを入力してください（[Ctrl]+C または exit で終了）：\n");
    print_help();

    /* メインループ */
    while (true) {
        printf("command>> ");

        /* ユーザの入力を受け付ける */
        if (fgets(user_input, sizeof(user_input), stdin) == NULL) 
            continue;

        /* 改行を除去 */
        user_input[strcspn(user_input, "\n")] = '\0';
        if (sscanf(user_input, "%255s", command) != 1) 
            continue;

        /* コマンドに対応した機能を呼び出し */
        if (strcmp(command, "add") == 0) 
            add_book(db);
        else if (strcmp(command, "list") == 0) 
            list_books(db);
        else if (strcmp(command, "progress") == 0) 
            update_progress(db);
        else if (strcmp(command, "help") == 0) 
            print_help();
        else if (strcmp(command, "exit") == 0) {
            printf("終了します。\n");
            break;
        } else 
            printf("不明なコマンドです。helpで使用可能なコマンドを確認\
                    できます。\n");
    }
    sqlite3_close(db);
    return 0;
}

// 使用可能なコマンド一覧を提供する関数
void print_help() {
    printf("使用可能なコマンド:\n");
    printf("  add   - 本を追加\n");
    printf("  list  - 登録された本の一覧を表示\n");
    printf("  progress - 読書進捗を記録・表示\n");
    printf("  help  - ヘルプを表示\n");
    printf("  exit  - プログラムを終了\n");
}


// 新しい本をデータベースに追加する関数
void add_book(sqlite3 *db){

    /* 空の本の新規作成 */
    book_t *book;
    book = create_new_book();

    /* 入力モード */
    enum input_mode{
        TITLE,
        AUTHOR,
        PAGE,
        PUBLISH_DATE
    };
    enum input_mode mode;
    mode = TITLE;

    bool exit_data = false;
    while(!exit_data){

        /* プロンプトを表示 */
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
    
        /* ユーザの入力を受け付ける*/
        char *line = (char *)malloc(sizeof(char)*1024);
        if(fgets(line, 1024, stdin) != NULL){
            line[strcspn(line, "\n")] = '\0';
        }

        /* quitコマンド．格納処理を中断*/
        if(strcmp(line, "quit")==0){
            destroy_book(book);
            return;
        }

        /* book_tへのデータの格納 */
        switch(mode){
            case TITLE:
                strcpy(book->title, line);
                mode = AUTHOR;
                break;

            case AUTHOR:

                /* 空行が打たれたら著者の入力を終了 */ 
                if(strlen(line) < 1){
                    mode = PAGE;
                    break;
                }
                strcpy(book->authors[book->number_of_author], line);
                book->number_of_author++;
                break;

            case PAGE:
                book->page = atoi(line);
                mode = PUBLISH_DATE;                
                break;

            case PUBLISH_DATE:
                strcpy(book->publish_date, line);
                exit_data = true;
                break;
        }
        free(line);
    }

    if(exit_data)
        register_book_to_db(*book, db);

    destroy_book(book);
}

/* 登録された本の情報を一覧する関数 */
void list_books(sqlite3 *db) {
    const char *sql = "SELECT id, title, author, pages, publish_date,\
     progress, progress_str FROM books;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    printf("登録された本の一覧：\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        //printf("id %d\n", id);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        //printf("title %s\n", title);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        //printf("author %s\n", author);
        int pages = sqlite3_column_int(stmt, 3);
        const unsigned char *publish_date = sqlite3_column_text(stmt, 4);
        int progress = sqlite3_column_int(stmt, 5);
        const unsigned char *progress_str = sqlite3_column_text(stmt, 6);
        
        printf("[%d] %s | %s | %dページ | 発行日: %s | 読了: %dページ(%s)\n",
             id, title, author, pages, publish_date, progress, progress_str);
    }

    sqlite3_finalize(stmt);
}

/* 本の読書進捗状況を更新するための関数*/
void update_progress(sqlite3 *db ) {
    
    /* IDを閲覧するために一覧を表示 */
    list_books(db);
    printf("\n");

    /* 操作対象のIDをユーザ選択させる */
    printf("対象の本のIDを入力してください：");
    int  book_id = 0;
    char book_id_buffer[128];
    if(fgets(book_id_buffer, 128, stdin) != NULL){
        book_id = atoi(book_id_buffer);
    }else{
        fprintf(stderr,"入力されたIDに該当する本が見つかりませんでした.\n");
        return;
    }
    
    printf("今日読んだページを入力\n\
            ：例）1ページと50ページのみ読んだ場合 1,50\n \
              例）1ページから50ページの区間を読んだ場合 1-100\n");

    char read_page_str[4096];
    if(fgets(read_page_str, 4096, stdin) == NULL){
        return ;
    }

    char **page_ranges = (char **)malloc(sizeof(char*)*4096);
    int number_of_ranges = 0;
    page_ranges[number_of_ranges] = strtok(read_page_str, ",");

    do{
        number_of_ranges++;
    }while((page_ranges[number_of_ranges] = strtok(NULL, ",")) != NULL);

    int start_pages[4096];
    int end_pages[4096];

    for(int i=0; i<number_of_ranges; i++){

        char *start_page_str = strtok(page_ranges[i], "-");

        int start_page;
        if((start_page = atoi(start_page_str)) != 0){
            start_pages[i] = start_page;
        }else{
            fprintf(stderr, "数値以外が入力されました.\n");
            return;
        }

        char *end_page_str = strtok(NULL, "-");
        
        if(end_page_str == NULL){ // it is not a range
            end_pages[i] = start_page;

        }else{ // it is a range
            int end_page;
            if((end_page = atoi(end_page_str)) != 0){
                end_pages[i] = end_page;
            }else{
                fprintf(stderr, "数値以外が入力されました.\n");
                return;
            }
        }
    }

    for(int i=0; i<number_of_ranges; i++){
        printf("start: %d, end :%d\n", start_pages[i], end_pages[i]);
    }

	/* rangeの初期化用関数を作っておくべき */
    range_node_t *head_node = (range_node_t*)malloc(sizeof(range_node_t));
    head_node->start_page = -1;
    head_node->end_page = -1;
    head_node->previous = NULL;
    head_node->next = NULL;


    for(int i=0; i<number_of_ranges; i++){
        range_node_t* new_node = (range_node_t*)malloc(sizeof(range_node_t));
        new_node->start_page = start_pages[i];
        new_node->end_page = end_pages[i];
        new_node->previous = NULL;
        new_node->next = NULL;

        head_node = append_range_node(head_node, new_node);
    }

    // unsorted
    dump_page_range_list(head_node);
    printf("\n");

    // sorted
    head_node = sort_range_list(head_node, number_of_ranges);
    dump_page_range_list(head_node);
    printf("\n");

    // merged
    head_node = eliminate_redundant_nodes(head_node, number_of_ranges);
    dump_page_range_list(head_node);
    printf("\n");

    char *range_str = (char*)malloc(sizeof(char)*1024);
    range_str[0] = '\0'; 
    if(range_str == NULL){
        fprintf(stderr, "could not allocate sufficient memory for range_str\n");
        exit(EXIT_FAILURE);
    }

    //for debuging
    // dump_page_range_list(head_node);
    //printf("\n");
    
    char *new_progress = cat_range_string(head_node, range_str);
    update_progress_to_db(db, book_id , new_progress);//進捗のデータベース書き込み処理
    free(range_str);

}
