#include <stdio.h>
#include <sqlite3.h>

void insert_db(sqlite3 *db) {
    char *zErrMsg = 0;
    char *sql = "insert into tbl_test values('test', 'test@mail.com')"; 
    int rc;

    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void select_db(sqlite3 *db) {
    char *zErrMsg = 0;
    sqlite3_stmt *stmt;
    int col, cols;
    char *sql = "select * from tbl_test"; 
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    cols = sqlite3_column_count(stmt);
    printf("cols: %d\n", cols); 

    while(1) {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            for (col = 0; col < cols; col++) {
                const char *val = (const char*)sqlite3_column_text(stmt,col);
                printf("%s = %s\n",sqlite3_column_name(stmt,col),val);
            }
        } else if (rc == SQLITE_DONE){
            break; 
        }
    }

    sqlite3_free(stmt);
}

int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    sqlite3_stmt *stmt;
    int col, cols;

    if( argc!=2 ){
        fprintf(stderr, "Usage: %s DATABASE\n", argv[0]);
        return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }

    printf("====== BEFORE ======\n");
    select_db(db);
    insert_db(db);
    printf("====== AFTER  ======\n");
    select_db(db);

    sqlite3_close(db);
    return 0;
}
