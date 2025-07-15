#include "../Headers/DBSql.hpp"
/**
 * @brief Execute batch of insertion.
 * 
 * @param sql [char*] Insert sql statement
 * @param features
 * @param count
 * @param n_feature [int] The number of column are inserted.
 */
void DBSql::BatchInsertFromString(char* sql, char*** features, int count, int n_feature, int len) {
    DBConnector* conn = &DBConnector::GetInstance();
    conn->Initialize();
    conn->SetSQLStatement(sql);
    conn->BindArraySetSize(count);
    for ( int i = 1; i <= n_feature; ++i ) {
        char fmtstr[10];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);
        conn->BindArrayOfStrings(fmtstr, (char*)((char (*)[count][len + 1])features)[i - 1], len, count);
    }
    conn->Execute("PreparedStatement Batch executed, INSERT done\n", "Batch execution failed. Rolling back.\n");
    conn->Disconnect();
}

/**
 * @brief Test function that excute SELECT cmd.
 * 
 * @param sql [char*] SQL statement be excuted.
 */
void DBSql::FetchDataTest(char* sql) {
    DBConnector* conn = &DBConnector::GetInstance();
    conn->Initialize();
    conn->Fetch(sql, "Fetching data success.", "Fetching data failed.");
    conn->Disconnect();
}