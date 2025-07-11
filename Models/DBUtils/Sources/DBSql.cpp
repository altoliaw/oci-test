#include "../Headers/DBSql.hpp"
#include <iostream>
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
    conn->Execute();
    conn->Disconnect();
}