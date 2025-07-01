#include "../Headers/DBConnector.hpp"
#include <iostream>
#include <ocilib.h>
#include <stdio.h>
using namespace std;

DBConnector* DBConnector::instance = nullptr;

/**
 * @brief Get instance of DBConnector.
 * 
 * @return DBConnector& 
 */
DBConnector& DBConnector::GetInstance() {
    if ( instance == nullptr ) {
        // if only one instance is nullptr new a instance.
        instance = new DBConnector();
    }
    return *instance;
}

/**
 * @brief Initialize the DB connection and create the statement instance of connection.
 * 
 * @return [int] The success of initialization
 */
int DBConnector::Initialize() {
    // Initialize OCILIB
    if (!OCI_Initialize(DBConnector::Err_Handler, NULL, OCI_ENV_DEFAULT)) {
        printf("Failed to initialize OCILIB\n");
        return EXIT_FAILURE;
    }
    // Connect to the database
    cn = OCI_ConnectionCreate("192.168.30.178/orcl", "alan", "alan", OCI_SESSION_DEFAULT);
    if (!cn) {
        printf("Connection failed!\n");
        OCI_Cleanup();
        return EXIT_FAILURE;
    }
    printf("Connection established successfully!\n");
    st = OCI_StatementCreate(cn);
}

/**
 * @brief Error handler of OCI shows the message of error.
 * 
 * @param err 
 */
void DBConnector::Err_Handler(OCI_Error* err) {
    printf("%s\n", OCI_ErrorGetString(err));
}

/**
 * @brief Set the sql statement that will be excuted.
 * 
 * @param sql [char*] The sql statement.
 * @return [int]
 */
int DBConnector::SetSQLStatement(char* sql) {
    // if success return 0;
    boolean success = OCI_Prepare(st, sql);
    if ( !success ) {
        cout << "Failed to prepare statement." << endl;
        OCI_Cleanup();
        return EXIT_FAILURE;
    }
    return 0;
}


boolean DBConnector::BindArraySetSize(unsigned int size) {
    return OCI_BindArraySetSize(st, size);
}


boolean DBConnector::BindArrayOfStrings(const char* name, char* value, unsigned int len, unsigned int nb_elem) {
    return OCI_BindArrayOfStrings(st, name, value, len, nb_elem);
}

/**
 * @brief Excute the sql statement you have set before(DBConnector::SetSQLStatement).
 * 
 */
void DBConnector::Execute() {
    if ( OCI_Execute(st) ) {
        printf("PreparedStatement Batch executed, INSERT done\n");
        OCI_Commit(cn);
    }
    else {
        printf("Batch execution failed. Rolling back.\n");
        OCI_Rollback(cn);
    }
    OCI_StatementFree(st);
}

/**
 * @brief Free the connection of DB.
 * 
 */
void DBConnector::Disconnect() {
    OCI_ConnectionFree(cn);
    OCI_Cleanup();
}

/**
 * @brief Get the pointer of OCI_Statement of DBConnector.
 * 
 * @return [OCI_Statement*]
 */
OCI_Statement* DBConnector::GetStatement() {
    return st;
}

/**
 * Constructor 
 */
DBConnector::DBConnector() {
}

/**
 * @brief Destructor, free the pointer of instance.
 * 
 */
DBConnector::~DBConnector() {
    delete instance;
}