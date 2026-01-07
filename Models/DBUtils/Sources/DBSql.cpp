/**
 * @see DBSql.hpp
 */
#include "../Headers/DBSql.hpp"

/**
 * Executing batch insertion from string arrays; the function will connect to the database,
 * prepare the statement, bind the array parameters, and execute the batch insertion
 *
 * @param sql [char*] The INSERT SQL statement with placeholders (e.g., ":1", ":2")
 * @param features [char***] The 3D array containing the data; the dimensions are [n_feature][count][len+1]
 * @param count [int] The number of rows which will be inserted
 * @param n_feature [int] The number of columns which will be inserted
 * @param len [int] The maximum length of each string value
 */
void DBSql::BatchInsertFromString(char* sql, char*** features, int count, int n_feature, int len) {
    // Obtaining the singleton instance of the DBConnector
    DBConnector* conn = &DBConnector::GetInstance();

    // Initializing the database connection
    conn->Initialize();

    // Setting the SQL statement
    conn->SetSQLStatement(sql);

    // Setting the batch size for array binding
    conn->BindArraySetSize(count);

    // Binding the array of strings for each column
    for (int i = 1; i <= n_feature; ++i) {
        // Generating the placeholder name (e.g., ":1", ":2", ":3")
        char fmtstr[10];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);

        // Binding the array; the cast is for accessing the 3D array as [column][row][char]
        conn->BindArrayOfStrings(fmtstr, (char*)((char(*)[count][len + 1])features)[i - 1], len, count);
    }

    // Executing the prepared statement with batch insertion
    conn->Execute("PreparedStatement Batch executed, INSERT done\n", "Batch execution failed. Rolling back.\n");

    // Releasing the database connection
    conn->Disconnect();
}

/**
 * Executing a SELECT query for testing purposes; the function will connect to the database,
 * execute the query, and display the result
 *
 * @param sql [char*] The SELECT SQL statement
 */
void DBSql::FetchDataTest(char* sql) {
    // Obtaining the singleton instance of the DBConnector
    DBConnector* conn = &DBConnector::GetInstance();

    // Initializing the database connection
    conn->Initialize();

    // Executing the SELECT query and fetching the result
    conn->Fetch(sql, "Fetching data success.", "Fetching data failed.");

    // Releasing the database connection
    conn->Disconnect();
}
