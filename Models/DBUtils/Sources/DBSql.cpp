#include "../Headers/DBSql.hpp"
/**
 * @brief Execute batch of insertion.
 * 
 * @param sql [char*] Insert sql statement
 * @param features
 * @param count
 * @param n_feature [int] The number of column are inserted.
 */
void DBSql::BatchInsertFromString(char* sql, char** features[], int count, int n_feature) {
    DBConnector* conn = &DBConnector::GetInstance();
    conn->Initialize();
    conn->SetSQLStatement(sql);
    for ( int i = 1; i <= n_feature; ++i ) {
        char** feature;
        char fmtstr[100];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);
        feature = features[i - 1];
        int max_len = 0;
        for ( int j = 0; j < count; j++ ) {
            int len = strlen(feature[j]);
            if ( max_len < len ) max_len = len;
        }
        conn->BindArrayOfStrings(fmtstr, (char*)feature, max_len + 1, count);
    }
    conn->BindArraySetSize(count);
    conn->Execute();
    conn->Disconnect();
}