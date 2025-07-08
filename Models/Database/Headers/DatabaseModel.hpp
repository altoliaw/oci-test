#pragma once
/** @file DatabaseModel.hpp
 * Database connection management
 * 
 * @author johnny.gu
 * @date 2025/06/20
 */

#include <ocilib.h>
#include "../../Commons/Headers/POSIXErrors.hpp"

namespace Database {

/**
 * Database connection management
 */
class DatabaseModel {
public:
    DatabaseModel();
    ~DatabaseModel();
    
    /**
     * Initialize OCILIB
     * @return [Commons::POSIXErrors] Initialization result
     */
    Commons::POSIXErrors initialize();
    
    /**
     * Create database connection
     * @param connectionString [const char*] Connection string
     * @param username [const char*] Username
     * @param password [const char*] Password
     * @return [Commons::POSIXErrors] Connection result
     */
    Commons::POSIXErrors connect(const char* connectionString, const char* username, const char* password);
    
    /**
     * Create SQL statement
     * @return [OCI_Statement*] SQL statement pointer
     */
    OCI_Statement* createStatement();
    
    /**
     * Set auto commit
     * @param autoCommit [bool] Auto commit flag
     */
    void setAutoCommit(bool autoCommit);
    
    /**
     * Commit transaction
     * @return [Commons::POSIXErrors] Commit result
     */
    Commons::POSIXErrors commit();
    
    /**
     * Rollback transaction
     * @return [Commons::POSIXErrors] Rollback result
     */
    Commons::POSIXErrors rollback();
    
    /**
     * Cleanup resources
     */
    void cleanup();

private:
    OCI_Connection* connection;
    bool initialized;
    
    /**
     * Error handler
     * @param err [OCI_Error*] Error object
     */
    static void errorHandler(OCI_Error* err);
};
}
