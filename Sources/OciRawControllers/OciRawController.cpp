/**
 * @see OciRawController.hpp
 */

#ifdef __linux__

#include "../../Headers/OciRawControllers/OciRawController.hpp"

#include <iostream>
#include <cstring>

/**
 * Constructing the OciRawController instance; the OCI environment and handles
 * shall be initialized
 */
OciRawController::OciRawController()
    : ociEnvironment(nullptr), ociErrorHandle(nullptr), ociServiceContext(nullptr), isConnected(false) {
    initializeOciEnvironment();
}

/**
 * Destroying the OciRawController instance; the OCI resources shall be released
 */
OciRawController::~OciRawController() {
    if (isConnected) {
        disconnect();
    }
    cleanupOciResources();
}

/**
 * Initializing the OCI environment and error handle
 *
 * @return [bool] The value shall be true if the initialization is successful; otherwise false
 */
bool OciRawController::initializeOciEnvironment() {
    sword status = OCI_SUCCESS;

    // Creating the OCI environment
    status = OCIEnvCreate(&ociEnvironment, OCI_DEFAULT, nullptr, nullptr, nullptr, nullptr, 0, nullptr);
    if (!checkError(status, "OCIEnvCreate failed")) {
        return false;
    }

    // Allocating the error handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&ociErrorHandle, OCI_HTYPE_ERROR, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for error handle failed")) {
        OCIHandleFree(ociEnvironment, OCI_HTYPE_ENV);
        ociEnvironment = nullptr;
        return false;
    }

    return true;
}

/**
 * Cleaning up the OCI resources; the handles shall be freed
 */
void OciRawController::cleanupOciResources() {
    // Freeing the error handle
    if (ociErrorHandle != nullptr) {
        OCIHandleFree(ociErrorHandle, OCI_HTYPE_ERROR);
        ociErrorHandle = nullptr;
    }

    // Freeing the environment handle
    if (ociEnvironment != nullptr) {
        OCIHandleFree(ociEnvironment, OCI_HTYPE_ENV);
        ociEnvironment = nullptr;
    }
}

/**
 * Checking the OCI return status and printing the error message if an error occurs
 *
 * @param status [sword] The return status from the OCI function
 * @param errorMessage [const char*] The context message for identifying the error location
 * @return [bool] The value shall be true if the status is OCI_SUCCESS; otherwise false
 */
bool OciRawController::checkError(sword status, const char* errorMessage) {
    if (status == OCI_SUCCESS) {
        return true;
    }

    // Obtaining the error message from the OCI error handle
    if ((status == OCI_ERROR || status == OCI_SUCCESS_WITH_INFO) && ociErrorHandle != nullptr) {
        OraText errorBuffer[512];
        sb4 errorCode = 0;
        OCIErrorGet(ociErrorHandle, 1, nullptr, &errorCode, errorBuffer, sizeof(errorBuffer), OCI_HTYPE_ERROR);
        if (status == OCI_SUCCESS_WITH_INFO) {
            std::cerr << "[WARN] " << errorMessage << ": " << errorBuffer << " (Error Code: " << errorCode << ")" << std::endl;
            return false;
        }
        std::cerr << "[ERROR] " << errorMessage << ": " << errorBuffer << " (Error Code: " << errorCode << ")" << std::endl;
    } else {
        std::cerr << "[ERROR] " << errorMessage << ": OCI Status = " << status << std::endl;
    }

    return false;
}

/**
 * Connecting to the Oracle database with the specified credentials
 *
 * @param username [const std::string&] The database username
 * @param password [const std::string&] The database password
 * @param connectionString [const std::string&] The database connection string
 * @return [bool] The value shall be true if the connection is successful; otherwise false
 */
bool OciRawController::connect(const std::string& username, const std::string& password, const std::string& connectionString) {
    if (ociEnvironment == nullptr || ociErrorHandle == nullptr) {
        std::cerr << "[ERROR] OCI environment is not initialized" << std::endl;
        return false;
    }

    if (isConnected) {
        std::cout << "[INFO] Already connected to database" << std::endl;
        return true;
    }

    std::cout << "[INFO] Connecting to database: " << connectionString << std::endl;

    // Connecting to the database using OCILogon2
    sword status = OCILogon2(
        ociEnvironment,
        ociErrorHandle,
        &ociServiceContext,
        (const OraText*)username.c_str(), (ub4)username.length(),
        (const OraText*)password.c_str(), (ub4)password.length(),
        (const OraText*)connectionString.c_str(), (ub4)connectionString.length(),
        OCI_DEFAULT
    );

    if (!checkError(status, "OCILogon2 failed")) {
        return false;
    }

    isConnected = true;
    std::cout << "[INFO] Connected to database successfully" << std::endl;
    return true;
}

/**
 * Disconnecting from the Oracle database; the connection resources shall be released
 */
void OciRawController::disconnect() {
    if (!isConnected || ociServiceContext == nullptr) {
        return;
    }

    std::cout << "[INFO] Disconnecting from database..." << std::endl;
    OCILogoff(ociServiceContext, ociErrorHandle);
    ociServiceContext = nullptr;
    isConnected = false;
    std::cout << "[INFO] Disconnected from database" << std::endl;
}

/**
 * The placeholder function for testing the query execution; 
 * users can implement their own test cases in this function
 */
void OciRawController::testPlaceHolder(const std::string& tableName) {

    // Dropping the table if it already exists from a previous run; the return value
    // shall be ignored since the table may not exist
    testDropTable(tableName);

    // Creating the table for testing
    if (!testCreateTable(tableName)) {
        return;
    }

    std::cout << "\n=== Inserting test data into " << tableName << " ===" << std::endl;

    // Sample data to be inserted
    const struct {
        int         id;
        const char* name;
        const char* nickname;
        const char* address;
    } sampleRows[] = {
        { 1, "Alice", "Alice", "Taipei" },
        { 2, "Bob", "Bob", "Man"   },
        { 3, "Carol", "Carol", "Woman" },
    };
    const int rowCount = sizeof(sampleRows) / sizeof(sampleRows[0]);

    for (int i = 0; i < rowCount; ++i) {
        OCIStmt* statementHandle = nullptr;
        sword status = OCI_SUCCESS;

        // Allocating the statement handle
        status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
        if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
            testDropTable(tableName); // Cleaning up the table if statement handle allocation fails
            return;
        }

        // Constructing the INSERT SQL statement
        std::string sqlStatement = "INSERT INTO " + tableName +
            " (id, name, nickname, address) VALUES (" +
            std::to_string(sampleRows[i].id) + ", '" +
            sampleRows[i].name + "' , '" +
            sampleRows[i].nickname + "' , '" +
            sampleRows[i].address + "')";

        // Preparing the SQL statement
        status = OCIStmtPrepare(
            statementHandle,
            ociErrorHandle,
            (const OraText*)sqlStatement.c_str(),
            (ub4)sqlStatement.length(),
            OCI_NTV_SYNTAX,
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIStmtPrepare failed")) {
            OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }

        // Executing the DML statement; iters shall be 1 for non-SELECT statements
        status = OCIStmtExecute(
            ociServiceContext,
            statementHandle,
            ociErrorHandle,
            1,          // Number of iterations (1 for DML)
            0,          // Row offset
            nullptr,    // Snapshot in
            nullptr,    // Snapshot out
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIStmtExecute failed")) {
            OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }

        std::cout << "[INFO] Row inserted: id=" << sampleRows[i].id
                  << ", name=" << sampleRows[i].name << std::endl;

        // Freeing the statement handle
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    }

    // Committing the INSERT transaction; DDL auto-commits, but DML shall be committed explicitly
    sword commitStatus = OCITransCommit(ociServiceContext, ociErrorHandle, OCI_DEFAULT);
    if (!checkError(commitStatus, "OCITransCommit failed")) {
        testDropTable(tableName);
        return;
    }
    std::cout << "[INFO] Insert transaction committed successfully" << std::endl;
    std::cout << "==========================================\n" << std::endl;

    std::cout << "\n=== Updating test data in " << tableName << " ===" << std::endl;

    // Updated address values keyed by id
    const struct {
        const char* name;
        const char* newAddress;
    } updateRows[] = {
        { "Alice", "New Taipei" }
    };
    const int updateCount = sizeof(updateRows) / sizeof(updateRows[0]);

    // Preparing the UPDATE statement once with bind variable placeholders :address and :id;
    // the statement handle shall be reused across all iterations
    OCIStmt* updateHandle = nullptr;
    sword status = OCIHandleAlloc(ociEnvironment, (void**)&updateHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for update statement handle failed")) {
        testDropTable(tableName);
        return;
    }

    std::string updateSql = "UPDATE " + tableName + " SET address = :address WHERE name = :name and nickname = :name";
    status = OCIStmtPrepare(
        updateHandle,
        ociErrorHandle,
        (const OraText*)updateSql.c_str(),
        (ub4)updateSql.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare for UPDATE failed")) {
        OCIHandleFree(updateHandle, OCI_HTYPE_STMT);
        testDropTable(tableName);
        return;
    }

    for (int i = 0; i < updateCount; ++i) {
        OCIBind* bindAddress = nullptr;
        OCIBind* bindName    = nullptr;

        const char* bindAddr     = updateRows[i].newAddress;
        const char* bindNameVal  = updateRows[i].name;

        // Binding :address placeholder; the value shall be the new address string
        status = OCIBindByName(
            updateHandle,
            &bindAddress,
            ociErrorHandle,
            (const OraText*)":address", -1,
            (void*)bindAddr,
            (sb4)(strlen(bindAddr) + 1),
            SQLT_STR,
            nullptr, nullptr, nullptr, 0, nullptr,
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIBindByName for :address failed")) {
            OCIHandleFree(updateHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }

        // Binding :name placeholder; the value shall be used in both name
        status = OCIBindByName(
            updateHandle,
            &bindName,
            ociErrorHandle,
            (const OraText*)":name", -1,
            (void*)bindNameVal,
            (sb4)(strlen(bindNameVal) + 1),
            SQLT_STR,
            nullptr, nullptr, nullptr, 0, nullptr,
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIBindByName for :name failed")) {
            OCIHandleFree(updateHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }

        // Binding :name placeholder; the value shall be used in both name
        status = OCIBindByName(
            updateHandle,
            &bindName,
            ociErrorHandle,
            (const OraText*)":name", -1,
            (void*)bindNameVal,
            (sb4)(strlen(bindNameVal) + 1),
            SQLT_STR,
            nullptr, nullptr, nullptr, 0, nullptr,
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIBindByName for :name failed")) {
            OCIHandleFree(updateHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }


        // Executing the DML statement; iters shall be 1 for non-SELECT statements
        status = OCIStmtExecute(
            ociServiceContext,
            updateHandle,
            ociErrorHandle,
            1,          // Number of iterations (1 for DML)
            0,          // Row offset
            nullptr,    // Snapshot in
            nullptr,    // Snapshot out
            OCI_DEFAULT
        );
        if (!checkError(status, "OCIStmtExecute for UPDATE failed")) {
            OCIHandleFree(updateHandle, OCI_HTYPE_STMT);
            testDropTable(tableName);
            return;
        }

        std::cout << "[INFO] Row updated: name=" << updateRows[i].name
                  << ", address=" << updateRows[i].newAddress << std::endl;
    }

    // Freeing the reused UPDATE statement handle
    OCIHandleFree(updateHandle, OCI_HTYPE_STMT);

    // Committing the UPDATE transaction
    commitStatus = OCITransCommit(ociServiceContext, ociErrorHandle, OCI_DEFAULT);
    if (!checkError(commitStatus, "OCITransCommit failed")) {
        testDropTable(tableName);
        return;
    }
    std::cout << "[INFO] Update transaction committed successfully" << std::endl;
    std::cout << "==========================================\n" << std::endl;

    // Creating and dropping the procedure for testing; the procedure name shall be derived from the table name
    const std::string procedureName = "get" + tableName;
    if (!testCreateProcedure(procedureName, tableName)) {
        testDropTable(tableName);
        return;
    }

    // Searching the result by calling the procedure
    const std::string nickName = "Alice";
    testCallProcedure(procedureName, &nickName);

    testDropProcedure(procedureName);

    testDropTable(tableName);
}

/**
 * Testing the basic SELECT query; the function shall execute "SELECT SYSDATE FROM DUAL"
 * and display the result
 */
void OciRawController::testSelectSysdate() {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return;
    }

    std::cout << "\n=== Testing SELECT SYSDATE FROM DUAL ===" << std::endl;

    OCIStmt*   statementHandle = nullptr;
    OCIDefine* defineHandle    = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return;
    }

    // Preparing the SQL statement
    const char* sqlQuery = "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL";
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlQuery,
        (ub4)strlen(sqlQuery),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return;
    }

    // Defining the output variable for the result
    char dateResultBuffer[64];
    memset(dateResultBuffer, 0, sizeof(dateResultBuffer));

    status = OCIDefineByPos(
        statementHandle,
        &defineHandle,
        ociErrorHandle,
        1,                              // Position (1-based)
        dateResultBuffer,               // Output buffer
        sizeof(dateResultBuffer),       // Buffer size
        SQLT_STR,                       // Data type (string)
        nullptr,                        // Indicator variable
        nullptr,                        // Return length
        nullptr,                        // Return code
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIDefineByPos failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return;
    }

    // Executing the SQL statement
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        0,                              // Number of rows to fetch (0 for SELECT)
        0,                              // Row offset
        nullptr,                        // Snapshot in
        nullptr,                        // Snapshot out
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return;
    }

    // Fetching the result
    status = OCIStmtFetch2(
        statementHandle,
        ociErrorHandle,
        1,                              // Number of rows to fetch
        OCI_FETCH_NEXT,                 // Fetch orientation
        0,                              // Scroll offset
        OCI_DEFAULT
    );

    if (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
        std::cout << "SYSDATE: " << dateResultBuffer << std::endl;
    } else if (status == OCI_NO_DATA) {
        std::cout << "No data returned from query" << std::endl;
    } else {
        checkError(status, "OCIStmtFetch2 failed");
    }

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "========================================\n" << std::endl;
}

/**
 * Testing the data fetching from a specified table; the function shall execute a SELECT
 * query and display the results
 *
 * @param tableName [const std::string&] The name of the table to query
 */
void OciRawController::testFetchData(const std::string& tableName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return;
    }

    std::cout << "\n=== Testing SELECT * FROM " << tableName << " ===" << std::endl;

    // Constructing the SQL query
    std::string sqlStatement = "SELECT * FROM " + tableName + " WHERE ROWNUM <= 10";
    executeQuery(sqlStatement);

    std::cout << "==========================================\n" << std::endl;
}

/**
 * Testing the CREATE TABLE DDL statement; the function shall create a table with the
 * specified name and a predefined set of columns
 *
 * @param tableName [const std::string&] The name of the table to be created
 * @return [bool] The value shall be true if the table is created successfully; otherwise false
 */
bool OciRawController::testCreateTable(const std::string& tableName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    std::cout << "\n=== Testing CREATE TABLE " << tableName << " ===" << std::endl;

    OCIStmt* statementHandle = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Constructing the CREATE TABLE SQL statement
    std::string sqlStatement = "CREATE TABLE " + tableName + " ("
        "id NUMBER PRIMARY KEY, "
        "name VARCHAR2(50), "
        "nickname VARCHAR2(50), "
        "address VARCHAR2(100), "
        "created_at DATE DEFAULT SYSDATE"
        ")";

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the DDL statement; iters shall be 1 for non-SELECT statements
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        1,          // Number of iterations (1 for DDL)
        0,          // Row offset
        nullptr,    // Snapshot in
        nullptr,    // Snapshot out
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtExecute failed")) {
        testDropTable(tableName); // Cleaning up the table if statement handle allocation fails
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    std::cout << "[INFO] Table " << tableName << " is created successfully" << std::endl;

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "==========================================\n" << std::endl;
    return true;
}

/**
 * Testing the CREATE PROCEDURE DDL statement; the function shall create a procedure with the
 * specified name and a predefined set of columns
 *
 * @param tableName [const std::string&] The name of the table to be created
 * @return [bool] The value shall be true if the table is created successfully; otherwise false
 */
bool OciRawController::testCreateProcedure(const std::string& procedureName, 
    const std::string& tableName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    std::cout << "\n=== Testing CREATE PROCEDURE " << procedureName << " ===" << std::endl;

    OCIStmt* statementHandle = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Constructing the CREATE TABLE SQL statement
    std::string sqlStatement =
        "CREATE OR REPLACE PROCEDURE " + procedureName + " (\n"
        "    p_userName OUT SYS_REFCURSOR,\n"
        "    p_userNickName IN VARCHAR2\n"
        ")\n"
        "IS\n"
        "BEGIN\n"
        "    OPEN p_userName FOR\n"
        "        SELECT * FROM " + tableName + "\n"
        "        WHERE nickname = p_userNickName;\n"
        "END " + procedureName + ";";

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the DDL statement; iters shall be 1 for non-SELECT statements
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        1,          // Number of iterations (1 for DDL)
        0,          // Row offset
        nullptr,    // Snapshot in
        nullptr,    // Snapshot out
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    std::cout << "[INFO] PROCEDURE " << procedureName << " is created successfully" << std::endl;

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "==========================================\n" << std::endl;
    return true;
}

/**
 * Testing the DROP TABLE DDL statement; the function shall drop the table with the
 * specified name if it exists
 *
 * @param tableName [const std::string&] The name of the table to be dropped
 * @return [bool] The value shall be true if the table is dropped successfully; otherwise false
 */
bool OciRawController::testDropTable(const std::string& tableName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    std::cout << "\n=== Testing DROP TABLE " << tableName << " ===" << std::endl;

    OCIStmt* statementHandle = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Constructing the DROP TABLE SQL statement
    std::string sqlStatement = "DROP TABLE " + tableName + " PURGE";

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the DDL statement; iters shall be 1 for non-SELECT statements
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        1,          // Number of iterations (1 for DDL)
        0,          // Row offset
        nullptr,    // Snapshot in
        nullptr,    // Snapshot out
        OCI_DEFAULT
    );
    if (status == OCI_ERROR) {
        sb4 errorCode = 0;
        OraText errorBuffer[512];
        OCIErrorGet(ociErrorHandle, 1, nullptr, &errorCode, errorBuffer, sizeof(errorBuffer), OCI_HTYPE_ERROR);

        // ORA-00942: table or view does not exist; the error shall be ignored silently
        if (errorCode == 942) {
            OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
            return true;
        }
        std::cerr << "[ERROR] OCIStmtExecute failed: " << errorBuffer << " (Error Code: " << errorCode << ")" << std::endl;
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    } else if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    std::cout << "[INFO] Table " << tableName << " is dropped successfully" << std::endl;

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "==========================================\n" << std::endl;
    return true;
}

/**
 * Testing the execution of a stored procedure; the function shall invoke the specified
 * procedure with the provided username and nickname parameters via an anonymous PL/SQL block
 *
 * @param procedureName [const std::string&] The name of the procedure to be executed
 * @param userNickName [const std::string&] The value to be bound to the p_userNickName parameter
 * @return [bool] The value shall be true if the execution is successful; otherwise false
 */
bool OciRawController::testCallProcedure(const std::string& procedureName, const std::string* userNickName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    std::cout << "\n=== Testing CALL PROCEDURE " << procedureName << " ===" << std::endl;

    OCIStmt* statementHandle = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Constructing the anonymous PL/SQL block to invoke the stored procedure
    std::string sqlStatement = "BEGIN " + procedureName + "(:v_cursor, :p_userNickName); END;";

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    OCIBind* bindCursor       = nullptr;
    OCIBind* bindUserNickName = nullptr;

    // Allocating the cursor handle for the OUT SYS_REFCURSOR parameter; the address of
    // the handle shall be passed to OCIBindByName so that Oracle can write the cursor back
    OCIStmt* cursorHandle = nullptr;
    status = OCIHandleAlloc(ociEnvironment, (void**)&cursorHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for cursor handle failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Binding :v_cursor placeholder; the buffer shall be the address of the cursor handle
    status = OCIBindByName(
        statementHandle,
        &bindCursor,
        ociErrorHandle,
        (const OraText*)":v_cursor", -1,
        (dvoid*)NULL,
        (sb4)0,
        SQLT_RSET,
        (dvoid*)0, (ub2*)0, (ub2*)0, (ub4)0, (ub4*)0,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIBindByName for :v_cursor failed")) {
        OCIHandleFree(cursorHandle, OCI_HTYPE_STMT);
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Preparing the userNickName buffer and indicator; the indicator shall be OCI_IND_NULL (-1)
    // when userNickName is nullptr, allowing SQL NULL to be passed for the IN VARCHAR2 parameter
    char nickBuf[256]       = {};
    sb2  nickIndicator      = OCI_IND_NOTNULL;
    sb4  nickLen            = 1;
    if (userNickName != nullptr) {
        userNickName->copy(nickBuf, sizeof(nickBuf) - 1);
        nickLen = (sb4)(userNickName->length() + 1);
    } else {
        nickIndicator = OCI_IND_NULL;
    }

    // Binding :p_userNickName placeholder; the indicator shall signal NULL when userNickName is nullptr
    status = OCIBindByName(
        statementHandle,
        &bindUserNickName,
        ociErrorHandle,
        (const OraText*)":p_userNickName", -1,
        (void*)nickBuf,
        nickLen,
        SQLT_STR,
        &nickIndicator, nullptr, nullptr, 0, nullptr,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIBindByName for :p_userNickName failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the anonymous PL/SQL block; iters shall be 1 for non-SELECT statements
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        1,          // Number of iterations (1 for PL/SQL block)
        0,          // Row offset
        nullptr,    // Snapshot in
        nullptr,    // Snapshot out
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(cursorHandle, OCI_HTYPE_STMT);
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    std::cout << "[INFO] PROCEDURE " << procedureName << " executed successfully"
              << " (userNickName=" << (userNickName ? *userNickName : "NULL") << ")" << std::endl;

    // Freeing the cursor handle and statement handle
    OCIHandleFree(cursorHandle, OCI_HTYPE_STMT);
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "==========================================\n" << std::endl;
    return true;
}

/**
 * Testing the DROP PROCEDURE DDL statement; the function shall drop the procedure with the
 * specified name if it exists
 *
 * @param procedureName [const std::string&] The name of the procedure to be dropped
 * @return [bool] The value shall be true if the procedure is dropped successfully; otherwise false
 */
bool OciRawController::testDropProcedure(const std::string& procedureName) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    std::cout << "\n=== Testing DROP PROCEDURE " << procedureName << " ===" << std::endl;

    OCIStmt* statementHandle = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Constructing the DROP PROCEDURE SQL statement
    std::string sqlStatement = "DROP PROCEDURE " + procedureName;

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the DDL statement; iters shall be 1 for non-SELECT statements
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        1,          // Number of iterations (1 for DDL)
        0,          // Row offset
        nullptr,    // Snapshot in
        nullptr,    // Snapshot out
        OCI_DEFAULT
    );
    if (status == OCI_ERROR) {
        sb4 errorCode = 0;
        OraText errorBuffer[512];
        OCIErrorGet(ociErrorHandle, 1, nullptr, &errorCode, errorBuffer, sizeof(errorBuffer), OCI_HTYPE_ERROR);

        // ORA-04043: object does not exist; the error shall be ignored silently
        if (errorCode == 4043) {
            OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
            return true;
        }
        std::cerr << "[ERROR] OCIStmtExecute failed: " << errorBuffer << " (Error Code: " << errorCode << ")" << std::endl;
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    } else if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    std::cout << "[INFO] PROCEDURE " << procedureName << " is dropped successfully" << std::endl;

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    std::cout << "==========================================\n" << std::endl;
    return true;
}

/**
 * Executing a custom SQL query; the function shall execute the specified SQL statement
 * and display the results
 *
 * @param sqlStatement [const std::string&] The SQL statement to execute
 * @return [bool] The value shall be true if the execution is successful; otherwise false
 */
bool OciRawController::executeQuery(const std::string& sqlStatement) {
    if (!isConnected) {
        std::cerr << "[ERROR] Not connected to database" << std::endl;
        return false;
    }

    OCIStmt*   statementHandle = nullptr;
    OCIDefine* defineHandle    = nullptr;
    sword status = OCI_SUCCESS;

    // Allocating the statement handle
    status = OCIHandleAlloc(ociEnvironment, (void**)&statementHandle, OCI_HTYPE_STMT, 0, nullptr);
    if (!checkError(status, "OCIHandleAlloc for statement handle failed")) {
        return false;
    }

    // Preparing the SQL statement
    status = OCIStmtPrepare(
        statementHandle,
        ociErrorHandle,
        (const OraText*)sqlStatement.c_str(),
        (ub4)sqlStatement.length(),
        OCI_NTV_SYNTAX,
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtPrepare failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Defining the output variable (single column for simplicity)
    char queryResultBuffer[256];
    memset(queryResultBuffer, 0, sizeof(queryResultBuffer));

    status = OCIDefineByPos(
        statementHandle,
        &defineHandle,
        ociErrorHandle,
        1,                              // Position (1-based)
        queryResultBuffer,              // Output buffer
        sizeof(queryResultBuffer),      // Buffer size
        SQLT_STR,                       // Data type (string)
        nullptr,                        // Indicator variable
        nullptr,                        // Return length
        nullptr,                        // Return code
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIDefineByPos failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Executing the SQL statement
    status = OCIStmtExecute(
        ociServiceContext,
        statementHandle,
        ociErrorHandle,
        0,                              // Number of rows to fetch (0 for SELECT)
        0,                              // Row offset
        nullptr,                        // Snapshot in
        nullptr,                        // Snapshot out
        OCI_DEFAULT
    );
    if (!checkError(status, "OCIStmtExecute failed")) {
        OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
        return false;
    }

    // Fetching and displaying results
    std::cout << "Query: " << sqlStatement << std::endl;
    std::cout << "Results:" << std::endl;

    int rowCount = 0;
    while ((status = OCIStmtFetch2(statementHandle, ociErrorHandle, 1, OCI_FETCH_NEXT, 0, OCI_DEFAULT)) == OCI_SUCCESS ||
           status == OCI_SUCCESS_WITH_INFO) {
        std::cout << "  Row " << (++rowCount) << ": " << queryResultBuffer << std::endl;
        memset(queryResultBuffer, 0, sizeof(queryResultBuffer));
    }

    if (status != OCI_NO_DATA && status != OCI_SUCCESS) {
        checkError(status, "OCIStmtFetch2 failed");
    }

    std::cout << "Total rows fetched: " << rowCount << std::endl;

    // Freeing the statement handle
    OCIHandleFree(statementHandle, OCI_HTYPE_STMT);
    return true;
}

#endif // __linux__
