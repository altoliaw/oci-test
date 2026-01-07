/**
 * @see OciController.hpp
 */
#include "../../Headers/OciController/OciController.hpp"

#include <iostream>

/**
 * Testing the batch insertion with 50 columns and 1000 rows; the function will prepare
 * test data and insert them into the database table "wubai_table_actually_wushi"
 */
void OciController::start() {
    // The INSERT SQL statement with 50 columns
    char* sql =
        "INSERT INTO wubai_table_actually_wushi ("
        "extremely_long_column_name_one, extremely_long_column_name_two, extremely_long_column_name_three, "
        "extremely_long_column_name_four, extremely_long_column_name_five, extremely_long_column_name_six, "
        "extremely_long_column_name_seven, extremely_long_column_name_eight, extremely_long_column_name_nine, "
        "extremely_long_column_name_ten, extremely_long_column_name_eleven, extremely_long_column_name_twelve, "
        "extremely_long_column_name_thirteen, extremely_long_column_name_fourteen, extremely_long_column_name_fifteen, "
        "extremely_long_column_name_sixteen, extremely_long_column_name_seventeen, extremely_long_column_name_eighteen, "
        "extremely_long_column_name_nineteen, extremely_long_column_name_twenty, extremely_long_column_name_twenty_one, "
        "extremely_long_column_name_twenty_two, extremely_long_column_name_twenty_three, extremely_long_column_name_twenty_four, "
        "extremely_long_column_name_twenty_five, extremely_long_column_name_twenty_six, extremely_long_column_name_twenty_seven, "
        "extremely_long_column_name_twenty_eight, extremely_long_column_name_twenty_nine, extremely_long_column_name_thirty, "
        "extremely_long_column_name_thirty_one, extremely_long_column_name_thirty_two, extremely_long_column_name_thirty_three, "
        "extremely_long_column_name_thirty_four, extremely_long_column_name_thirty_five, extremely_long_column_name_thirty_six, "
        "extremely_long_column_name_thirty_seven, extremely_long_column_name_thirty_eight, extremely_long_column_name_thirty_nine, "
        "extremely_long_column_name_forty, extremely_long_column_name_forty_one, extremely_long_column_name_forty_two, "
        "extremely_long_column_name_forty_three, extremely_long_column_name_forty_four, extremely_long_column_name_forty_five, "
        "extremely_long_column_name_forty_six, extremely_long_column_name_forty_seven, extremely_long_column_name_forty_eight, "
        "extremely_long_column_name_forty_nine, extremely_long_column_name_fifty) "
        "VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,"
        ":21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,"
        ":41,:42,:43,:44,:45,:46,:47,:48,:49,:50)";

    // Preparing the test data; the dimensions are [50 columns][1000 rows][100 characters]
    char features[50][1000][100];
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 1000; j++) {
            sprintf(features[i][j], "extremely_long_value_of_column_with_extremely_long_name_number", 0);
        }
    }

    // Executing the batch insertion with 1000 rows, 50 columns, and maximum string length 99
    DBSql::BatchInsertFromString(sql, (char***)features, 1000, 50, 99);
}

/**
 * Testing the batch insertion with 2 columns; the function will prepare test data
 * and insert them into the database table "TEST"
 */
void OciController::Insert2Columns() {
    // The INSERT SQL statement with 2 columns
    char* sql =
        "INSERT INTO TEST(COLUMN1, COLUMN2) "
        "VALUES(:1, :2)";

    // The test value for the columns
    char* value = "extremely_long_value_of_column_with_extremely_long_name_number";

    // The feature array; the initialization shall be implemented before calling the function
    char*** features;

    // Executing the batch insertion with 2 rows, 2 columns
    DBSql::BatchInsertFromString(sql, features, 2, 2, 0);
}

/**
 * Testing the data fetching from the database; the function will execute a SELECT query
 * on the table "WUBAI_TABLE_ACTUALLY_WUSHI" and display the result
 */
void OciController::FetchDataTest() {
    // The SELECT SQL statement
    char* sql =
        "SELECT * FROM WUBAI_TABLE_ACTUALLY_WUSHI";

    // Executing the SELECT query
    DBSql::FetchDataTest(sql);
}
