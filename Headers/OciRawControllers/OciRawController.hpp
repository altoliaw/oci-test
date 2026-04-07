#pragma once
/** @file OciRawController.hpp
 * The controller for Oracle database operations using raw OCI API; the class provides
 * methods for database connection, query execution, and result fetching without using
 * the ocilib wrapper library
 *
 * @author Nick, Liao
 * @date 2025/06/26
 *
 * @note The file is dependent on the Oracle OCI library (libclntsh)
 */

#ifdef __linux__

#include <oci.h>
#include <string>

/**
 * The Oracle database controller using raw OCI API
 */
class OciRawController {
   public:
    /**
     * Constructing the OciRawController instance; the OCI environment and handles
     * shall be initialized
     */
    OciRawController();

    /**
     * Destroying the OciRawController instance; the OCI resources shall be released
     */
    ~OciRawController();

    /**
     * Connecting to the Oracle database with the specified credentials
     *
     * @param username [const std::string&] The database username
     * @param password [const std::string&] The database password
     * @param connectionString [const std::string&] The database connection string (e.g., "//localhost:1521/ORCLCDB")
     * @return [bool] The value shall be true if the connection is successful; otherwise false
     */
    bool connect(const std::string& username, const std::string& password, const std::string& connectionString);

    /**
     * Disconnecting from the Oracle database; the connection resources shall be released
     */
    void disconnect();

    /**
     * Testing the basic SELECT query; the function shall execute "SELECT SYSDATE FROM DUAL"
     * and display the result
     */
    void testSelectSysdate();

    /**
     * Testing the placeholder query execution
     */
    void testPlaceHolder(const std::string& tableName);

    /**
     * Testing the data fetching from a specified table; the function shall execute a SELECT
     * query and display the results
     *
     * @param tableName [const std::string&] The name of the table to query
     */
    void testFetchData(const std::string& tableName);

    /**
     * Testing the CREATE TABLE DDL statement; the function shall create a table with the
     * specified name and a predefined set of columns
     *
     * @param tableName [const std::string&] The name of the table to be created
     * @return [bool] The value shall be true if the table is created successfully; otherwise false
     */
    bool testCreateTable(const std::string& tableName);

    /**
     * Testing the DROP PROCEDURE DDL statement; the function shall drop the procedure with the
     * specified name if it exists
     *
     * @param procedureName [const std::string&] The name of the procedure to be created
     * @param tableName [const std::string&] The name of the table to be queried in the procedure
     * @return [bool] The value shall be true if the table is created successfully; otherwise false
     */
    bool testCreateProcedure(const std::string& procedureName, const std::string& tableName);

    /**
     * Testing the DROP TABLE DDL statement; the function shall drop the table with the
     * specified name if it exists
     *
     * @param tableName [const std::string&] The name of the table to be dropped
     * @return [bool] The value shall be true if the table is dropped successfully; otherwise false
     */
    bool testDropTable(const std::string& tableName);

    /**
     * Testing the DROP PROCEDURE DDL statement; the function shall drop the procedure with the
     * specified name if it exists
     *
     * @param procedureName [const std::string&] The name of the procedure to be dropped
     * @return [bool] The value shall be true if the procedure is dropped successfully; otherwise false
     */
    bool testDropProcedure(const std::string& procedureName);

    /**
     * Testing the execution of a stored procedure; the function shall invoke the specified
     * procedure with the provided username and nickname parameters via an anonymous PL/SQL block
     *
     * @param procedureName [const std::string&] The name of the procedure to be executed
     * @param userNickName [const std::string*] The value to be bound to the :v1 parameter;
     *                     the value shall be nullptr if SQL NULL is to be passed
     * @return [bool] The value shall be true if the execution is successful; otherwise false
     */
    bool testCallProcedure(const std::string& procedureName, const std::string* userNickName);

    /**
     * Executing a custom SQL query; the function shall execute the specified SQL statement
     * and display the results
     *
     * @param sqlStatement [const std::string&] The SQL statement to execute
     * @return [bool] The value shall be true if the execution is successful; otherwise false
     */
    bool executeQuery(const std::string& sqlStatement);

   private:
    OCIEnv*     ociEnvironment;     ///< The OCI environment handle
    OCIError*   ociErrorHandle;     ///< The OCI error handle
    OCISvcCtx*  ociServiceContext;  ///< The OCI service context handle
    bool        isConnected;        ///< The connection status flag

    /**
     * Checking the OCI return status and printing the error message if an error occurs
     *
     * @param status [sword] The return status from the OCI function
     * @param errorMessage [const char*] The context message for identifying the error location
     * @return [bool] The value shall be true if the status is OCI_SUCCESS; otherwise false
     */
    bool checkError(sword status, const char* errorMessage);

    /**
     * Initializing the OCI environment and error handle
     *
     * @return [bool] The value shall be true if the initialization is successful; otherwise false
     */
    bool initializeOciEnvironment();

    /**
     * Cleaning up the OCI resources; the handles shall be freed
     */
    void cleanupOciResources();
};

#endif // __linux__
