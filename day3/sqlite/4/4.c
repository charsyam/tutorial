#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql = "select * from tbl_test"; 
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

    sqlite3_close(db);
    return 0;
}
