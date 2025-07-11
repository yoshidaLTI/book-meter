#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "book.h"
#include "db_manager.h"


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

    enum input_mode{ //mode で管理
        TITLE, AUTHOR, PAGE, PUBLISH_DATE
    };

    enum input_mode mode;
    mode = TITLE;

    book_t *book;
    book = create_new_book();

    bool exit_data = false;

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

        if(fgets(line, 1024, stdin) != NULL){
            line[strcspn(line, "\n")] = '\0';
        }

        // quit
        if(strcmp(line, "quit")==0){
            destroy_book(book);
            return;
        }

        // store 
        switch(mode){
            case TITLE:
                strcpy(book->title, line);
                mode = AUTHOR;
                break;

            case AUTHOR:
                if(strlen(line) < 1){
                    mode = PAGE;
                    break;
                }
                // add author 
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


range_node_t* seek_last_node(range_node_t *range_node){

    range_node_t *target_node = NULL;
    
    if(range_node->next == NULL){// last
        target_node = range_node; 
    }else{
        target_node =  seek_last_node(range_node->next);            
    }
    
   return target_node;
}

range_node_t* append_range_node(range_node_t *head, range_node_t *new_node){
    
    range_node_t* last_node;
    last_node = seek_last_node(head);
    last_node->next = new_node;
    new_node->previous = last_node;

    return head;
}

range_node_t* seek_swap_node(range_node_t *range_node, int start_page){

    range_node_t *target_node = NULL;
    
    if((range_node->start_page != -1) &&
            ((range_node->start_page) < start_page)){
        target_node = range_node; 
    }else if(range_node-> next != NULL){
        target_node =  seek_swap_node(range_node->next, start_page);            
    }

    return target_node;
}

void sort_range_list(range_node_t *head, range_node_t *current_node){
   
    if(current_node->next == NULL){
        return;
    }

    range_node_t* swapped_node;
    swapped_node = seek_swap_node(current_node, current_node->start_page);

    if(swapped_node != NULL){
        int temp_start_page, temp_end_page;
        temp_start_page = current_node->start_page;
        temp_end_page = current_node->end_page;

        current_node->start_page =  swapped_node->start_page;
        current_node ->end_page =  swapped_node -> end_page;

        swapped_node->start_page = temp_start_page;
        swapped_node->end_page = temp_end_page;
    }

    sort_range_list(head, current_node->next);
}


// for debugging
void dump_page_range_list(range_node_t *range_node){

    if(range_node->start_page == range_node->end_page){
        if(range_node->start_page != -1)
            printf("| %d |", range_node->start_page);
    }else{
        printf("| %d - %d |", range_node->start_page, range_node->end_page);
    }

    if(range_node->next != NULL){
        printf("->");
        dump_page_range_list(range_node->next);
    }
}

// ノードの範囲を文字列型にする
char* cat_range_string(range_node_t *range_node, char *range_str){
    char page_range_buckups[1024] = {}; 
    strcpy(page_range_buckups,range_str);
    if(range_node->start_page == range_node->end_page){ //not range
        if(range_node->start_page != -1)
            sprintf(range_str," %s,%d", page_range_buckups,range_node->start_page);
    }else{
        sprintf(range_str,"%s,%d-%d", page_range_buckups,range_node->start_page, range_node->end_page);
    }
    if(range_node->next != NULL){
        cat_range_string(range_node->next, range_str);
    }
    return(range_str);
}

// for debugging
void dump_page_range_list_reverse(range_node_t *range_node){

    if(range_node->start_page == range_node->end_page){
        if(range_node->start_page != -1)
            printf("| %d |", range_node->start_page);
    }else{
        printf("| %d - %d |", range_node->start_page, range_node->end_page);
    }
    
    if(range_node->previous != NULL){
        printf("->");
        dump_page_range_list_reverse(range_node->previous);
    }

}

range_node_t *seek_marge_node(range_node_t *current_node, range_node_t *next_node){
    range_node_t* target_node = NULL;

    if((current_node->end_page+1) >= next_node->start_page ){
        target_node = next_node;
        return target_node;
    }
    if(next_node->next != NULL){
        //next range
        target_node = seek_marge_node(current_node->next, next_node->next);
    }
    return target_node;
}

int marge_range_list(range_node_t *current_node, int number_of_ranges){
    if(current_node->start_page == -1)
        number_of_ranges = marge_range_list(current_node->next, number_of_ranges);
        
    //not exit next node & finish
    if(current_node->next == NULL){
        return number_of_ranges;
    }
    //seek marge node
    range_node_t *marged_node =NULL;
    marged_node = seek_marge_node(current_node, current_node->next);

    if(marged_node != NULL){
        //should marge
        if(current_node->start_page <= marged_node->start_page 
            && current_node->end_page >= marged_node->end_page){
            //内包
            current_node->next = marged_node->next;
            return --number_of_ranges;
        }
        if(current_node->end_page+1 >= marged_node->start_page){
            //一部重なり、隣接の場合
            current_node->end_page = marged_node->end_page;
            current_node->next = marged_node->next;
            
            return --number_of_ranges;
        }
    }
    //next marge prosess
    number_of_ranges = marge_range_list(current_node->next, number_of_ranges);
    return number_of_ranges;
}


void update_progress(sqlite3 *db ) {
    
    list_books(db);
    printf("対象の本のIDを入力してください：");

    int book_id = 0;
    char book_id_buffer[128];
    if(fgets(book_id_buffer, 128, stdin) != NULL){
        book_id = atoi(book_id_buffer);
    }else{
        fprintf(stderr,"入力されたIDに該当する本が見つかりませんでした.\n");
        return;
    }
    
    (void)book_id; // silent compiler warnings 

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

    for(int i=0; i<number_of_ranges; i++)
        sort_range_list(head_node, head_node);

    printf("\n");

    // sorted
    dump_page_range_list(head_node);

    printf("\n");

    // marge
    for(int i=0; i <  number_of_ranges+1; i++){
        number_of_ranges = marge_range_list(head_node, number_of_ranges);
    }
    

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