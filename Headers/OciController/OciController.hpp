#pragma once
/** @file OciController.hpp
 * The controller for Oracle database operations; the class provides methods for testing
 * database insertion and data fetching
 *
 * @author Nick, Liao
 * @date 2025/06/26
 *
 * @note The file is dependent to the Models.DataModels
 * @note The file is dependent to the Models.DBUtils
 */
#include "../../Models/DataModels/Headers/TestDataModel.hpp"
#include "../../Models/DBUtils/Headers/DBSql.hpp"

/**
 * The Oracle database controller
 */
class OciController {
   public:
    /**
     * Testing the batch insertion with 50 columns and 1000 rows; the function will insert
     * test data into the database table "wubai_table_actually_wushi"
     */
    void start();

    /**
     * Testing the batch insertion with 2 columns; the function will insert test data
     * into the database table "TEST"
     */
    void Insert2Columns();

    /**
     * Testing the data fetching from the database; the function will execute a SELECT query
     * on the table "WUBAI_TABLE_ACTUALLY_WUSHI"
     */
    void FetchDataTest();
};
