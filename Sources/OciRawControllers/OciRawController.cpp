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
    if (status == OCI_ERROR && ociErrorHandle != nullptr) {
        OraText errorBuffer[512];
        sb4 errorCode = 0;
        OCIErrorGet(ociErrorHandle, 1, nullptr, &errorCode, errorBuffer, sizeof(errorBuffer), OCI_HTYPE_ERROR);
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
