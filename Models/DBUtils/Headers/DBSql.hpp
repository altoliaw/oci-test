#pragma once
/** @file DBSql.hpp
 * The SQL utility functions for batch operations and data fetching
 *
 * @author Nick, Liao
 * @date 2025/06/26
 *
 * @note The file is dependent to the Models.DataModels
 * @note The file is dependent to the DBConnector
 */
#include <cstdio>
#include <cstring>
#include <vector>

#include "../../DataModels/Headers/TestDataModel.hpp"
#include "DBConnector.hpp"

namespace DBSql {
/**
 * Executing batch insertion from string arrays; the function will insert multiple rows
 * into the database using prepared statements with array binding
 *
 * @param sql [char*] The INSERT SQL statement with placeholders (e.g., ":1", ":2")
 * @param features [char***] The 3D array containing the data; the dimensions are [n_feature][count][len+1]
 * @param count [int] The number of rows which will be inserted
 * @param n_feature [int] The number of columns which will be inserted
 * @param len [int] The maximum length of each string value
 */
void BatchInsertFromString(char* sql, char*** features, int count, int n_feature, int len);

/**
 * Executing a SELECT query for testing purposes; the function will fetch data from
 * the database and display the success or failure message
 *
 * @param sql [char*] The SELECT SQL statement
 */
void FetchDataTest(char* sql);
}  // namespace DBSql
