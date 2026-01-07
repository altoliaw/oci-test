/**
 * @see DBConnector.hpp
 */
#include "../Headers/DBConnector.hpp"

#include <iostream>
#include <ocilib.h>
#include <stdio.h>

using namespace std;

// Initialization as nullptr to the static variable defined in the class
DBConnector* DBConnector::instance = nullptr;

/**
 * Obtaining the singleton instance of the DBConnector; if the static pointer is nullptr,
 * the instance will be created automatically
 *
 * @return [DBConnector&] The reference to the DBConnector instance
 */
DBConnector& DBConnector::GetInstance() {
    // If the static pointer is nullptr, the pointer shall be referred to a new instance
    if (instance == nullptr) {
        instance = new DBConnector();
    }
    return *instance;
}

/**
 * Initializing the database connection and creating the statement instance of the connection;
 * the function shall be called before executing any SQL operations
 *
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors DBConnector::Initialize() {
    // Initializing OCILIB
    if (!OCI_Initialize(DBConnector::Err_Handler, NULL, OCI_ENV_DEFAULT)) {
        printf("Failed to initialize OCILIB\n");
        return Commons::POSIXErrors::E_INVAL;
    }

    // Connecting to the database; the connection information shall be obtained from
    // the configuration file in the future implementation
    cn = OCI_ConnectionCreate("192.168.30.178/orcl", "alan", "alan", OCI_SESSION_DEFAULT);
    if (!cn) {
        printf("Connection failed!\n");
        OCI_Cleanup();
        return Commons::POSIXErrors::E_IO;
    }
    printf("Connection established successfully!\n");

    // Creating the statement instance
    st = OCI_StatementCreate(cn);
    if (!st) {
        return Commons::POSIXErrors::E_NOMEM;
    }

    return Commons::POSIXErrors::OK;
}

/**
 * The error handler for OCI operations; the function will be called automatically when
 * an error occurs during OCI operations
 *
 * @param err [OCI_Error*] The error object from OCILIB
 */
void DBConnector::Err_Handler(OCI_Error* err) {
    printf("%s\n", OCI_ErrorGetString(err));
}

/**
 * Setting the SQL statement which will be executed; the function shall be called before
 * binding parameters or executing the statement
 *
 * @param sql [char*] The SQL statement
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors DBConnector::SetSQLStatement(char* sql) {
    // Preparing the SQL statement; if the preparation fails, the error will be returned
    boolean success = OCI_Prepare(st, sql);
    if (!success) {
        cout << "Failed to prepare statement." << endl;
        OCI_Cleanup();
        return Commons::POSIXErrors::E_INVAL;
    }
    return Commons::POSIXErrors::OK;
}

/**
 * Setting the array size for batch operations; the function shall be called before
 * binding array parameters
 *
 * @param size [unsigned int] The number of elements in the array
 * @return [boolean] The success value; if the value is TRUE, the operation succeeds
 */
boolean DBConnector::BindArraySetSize(unsigned int size) {
    return OCI_BindArraySetSize(st, size);
}

/**
 * Binding an array of strings to the SQL statement; the function shall be called after
 * setting the array size
 *
 * @param name [const char*] The placeholder name (e.g., ":1", ":2")
 * @param value [char*] The starting address of the string array
 * @param len [unsigned int] The maximum length of each string
 * @param nb_elem [unsigned int] The number of elements in the array
 * @return [boolean] The success value; if the value is TRUE, the operation succeeds
 */
boolean DBConnector::BindArrayOfStrings(const char* name, char* value, unsigned int len, unsigned int nb_elem) {
    return OCI_BindArrayOfStrings(st, name, value, len, nb_elem);
}

/**
 * Executing the SQL statement which has been set before; the function will commit the
 * transaction if the execution succeeds; otherwise, the transaction will be rolled back
 *
 * @param success [std::string] The message which will be displayed when the execution succeeds
 * @param failed [std::string] The message which will be displayed when the execution fails
 */
void DBConnector::Execute(std::string success, std::string failed) {
    if (OCI_Execute(st)) {
        std::cout << success << std::endl;
        OCI_Commit(cn);
    } else {
        std::cout << failed << std::endl;
        OCI_Rollback(cn);
    }
    OCI_StatementFree(st);
}

/**
 * Releasing the database connection and cleaning up the OCILIB resources; the function
 * shall be called when the database operations are completed
 */
void DBConnector::Disconnect() {
    OCI_ConnectionFree(cn);
    OCI_Cleanup();
}

/**
 * Obtaining the pointer to the OCI statement instance
 *
 * @return [OCI_Statement*] The pointer to the statement instance
 */
OCI_Statement* DBConnector::GetStatement() {
    return st;
}

/**
 * Constructor
 */
DBConnector::DBConnector() {
    cn = nullptr;
    st = nullptr;
}

/**
 * Destructor; the static instance will be released when the program terminates
 */
DBConnector::~DBConnector() {
    // Note: The instance deletion shall be handled by the caller or a separate cleanup method,
    // not in the destructor to avoid self-deletion issues
    cn = nullptr;
    st = nullptr;
}

/**
 * Executing a SELECT query and obtaining the result set; the function will display
 * the success or failure message
 *
 * @param sql [char*] The SQL SELECT statement
 * @param success [std::string] The message which will be displayed when the fetch succeeds
 * @param failed [std::string] The message which will be displayed when the fetch fails
 */
void DBConnector::Fetch(char* sql, std::string success, std::string failed) {
    OCI_Resultset* rs;

    // Executing the SQL statement
    if (OCI_ExecuteStmt(this->st, sql)) {
        std::cout << success << std::endl;
        rs = OCI_GetResultset(this->st);

        // The result set can be processed here in the future implementation
    } else {
        std::cout << failed << std::endl;
    }
    OCI_StatementFree(st);
}
