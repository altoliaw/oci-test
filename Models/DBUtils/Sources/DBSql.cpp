#include <cstdlib>
#include "../Headers/DBSql.hpp"
/**
 * @brief Execute batch of insertion.
 * 
 * @param sql [char*] Insert sql statement
 * @param n_feature [int] The number of features data model have.
 */
void DBSql::BatchInsert(char* sql, std::vector<TestDataModel> models, int n_feature) {
    DBConnector* conn = &DBConnector::GetInstance();
    conn->Initialize();
    conn->SetSQLStatement(sql);
    for ( int i = 1; i <= n_feature; ++i ) {
        char fmtstr[100];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);
        char** features = (char**)malloc(models.size() * sizeof(char*));
        int maxLen = 0;
        for ( int j = 0; j < models.size(); j++ ) {
            // jth model's ith feature
            features[j] = models.at(j).GetFeatures().at(i - 1);
            if ( maxLen < (sizeof(models.at(j).GetFeatures().at(i - 1)) / sizeof(char*)) ) {
                maxLen = (sizeof(models.at(j).GetFeatures().at(i - 1)) / sizeof(char*));
            }
        }
        conn->BindArrayOfStrings(fmtstr, (char*)features, sizeof(maxLen), models.size());
    }
    conn->BindArraySetSize(models.size());
    conn->Execute();
    conn->Disconnect();
}