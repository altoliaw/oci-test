/** @file DatabaseModel.cpp
 * @see DatabaseModel.hpp
 */

#include "../Headers/DatabaseModel.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace Database {

DatabaseModel::DatabaseModel() : connection(nullptr), initialized(false) {
}

DatabaseModel::~DatabaseModel() {
    cleanup();
}

void DatabaseModel::errorHandler(OCI_Error* err) {
    printf("%s\n", OCI_ErrorGetString(err));
}

Commons::POSIXErrors DatabaseModel::initialize() {
    if (initialized) {
        return Commons::POSIXErrors::OK;
    }
    
    if (!OCI_Initialize(errorHandler, NULL, OCI_ENV_DEFAULT)) {
        printf("Failed to initialize OCILIB\n");
        return Commons::POSIXErrors::E_IO;
    }
    
    initialized = true;
    return Commons::POSIXErrors::OK;
}

Commons::POSIXErrors DatabaseModel::connect(const char* connectionString, 
                                           const char* username, 
                                           const char* password) {
    connection = OCI_ConnectionCreate(connectionString, username, password, OCI_SESSION_DEFAULT);
    if (!connection) {
        printf("Connection failed!\n");
        return Commons::POSIXErrors::E_IO;
    }
    
    printf("Connection established!\n");
    return Commons::POSIXErrors::OK;
}

OCI_Statement* DatabaseModel::createStatement() {
    return OCI_StatementCreate(connection);
}

void DatabaseModel::setAutoCommit(bool autoCommit) {
    OCI_SetAutoCommit(connection, autoCommit ? TRUE : FALSE);
}

Commons::POSIXErrors DatabaseModel::commit() {
    if (OCI_Commit(connection)) {
        return Commons::POSIXErrors::OK;
    }
    return Commons::POSIXErrors::E_IO;
}

Commons::POSIXErrors DatabaseModel::rollback() {
    if (OCI_Rollback(connection)) {
        return Commons::POSIXErrors::OK;
    }
    return Commons::POSIXErrors::E_IO;
}

void DatabaseModel::cleanup() {
    if (connection) {
        OCI_ConnectionFree(connection);
        connection = nullptr;
    }
    
    if (initialized) {
        OCI_Cleanup();
        initialized = false;
    }
}

}
