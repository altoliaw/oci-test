/** @file DatabaseController.cpp
 * @see DatabaseController.hpp
 */

#include "../../Headers/Controllers/DatabaseController.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace Controllers {

DatabaseController::DatabaseController() {
}

DatabaseController::~DatabaseController() {
}

Commons::POSIXErrors DatabaseController::runBatchInsert() {
    return runBatchInsert("192.168.30.178/orcl", "johnny", "password", 80);
}

Commons::POSIXErrors DatabaseController::runBatchInsert(const char* connectionString,
                                                       const char* username,
                                                       const char* password,
                                                       int batchSize) {
    Commons::POSIXErrors result = Commons::POSIXErrors::OK;
    
    Database::DatabaseModel dbModel;
    Database::BatchInsertModel batchModel;
    
    // Initialize database
    result = dbModel.initialize();
    if (result != Commons::POSIXErrors::OK) {
        return result;
    }
    
    // Create connection
    result = dbModel.connect(connectionString, username, password);
    if (result != Commons::POSIXErrors::OK) {
        return result;
    }
    
    // Set manual transaction
    dbModel.setAutoCommit(false);
    
    // Prepare batch data 
    result = batchModel.prepareBatchData(batchSize);
    if (result != Commons::POSIXErrors::OK) {
        return result;
    }
    
    // Validate data 
    if (!batchModel.validateData()) {
        return Commons::POSIXErrors::E_INVAL;
    }
    
    // Create SQL statement
    OCI_Statement* statement = dbModel.createStatement();
    if (!statement) {
        return Commons::POSIXErrors::E_IO;
    }
    
    // Prepare SQL statement
    const char* sql = batchModel.generateInsertSQL();
    if (!OCI_Prepare(statement, sql)) {
        printf("Failed to prepare statement.\n");
        OCI_Cleanup();
        return Commons::POSIXErrors::E_IO;
    }
    
    // Bind batch data 
    result = batchModel.bindArrayData(statement);
    if (result != Commons::POSIXErrors::OK) {
        OCI_StatementFree(statement);
        return result;
    }
    
    // Execute batch insert
    if (OCI_Execute(statement)) {
        printf("PreparedStatement Batch executed, INSERT done\n");
        result = dbModel.commit();
        if (result == Commons::POSIXErrors::OK) {
        } else {
        }
    } else {
        printf("Batch execution failed. Rolling back.\n");
        dbModel.rollback();
        result = Commons::POSIXErrors::E_IO;
    }
    
    // Cleanup resources
    OCI_StatementFree(statement);
    
    return result;
}

}
