#pragma once
/** @file DatabaseController.hpp
 * Database controller
 * 
 * @author johnny.gu
 * @date 2025/06/20
 */

#include "../../Models/Database/Headers/DatabaseModel.hpp"
#include "../../Models/Database/Headers/BatchInsertModel.hpp"
#include "../../Models/Commons/Headers/POSIXErrors.hpp"

namespace Controllers {

/**
 * Database controller class
 */
class DatabaseController {
public:
    DatabaseController();
    ~DatabaseController();
    
    /**
     * Execute batch insert operation
     * @return [Commons::POSIXErrors] Execution result
     */
    Commons::POSIXErrors runBatchInsert();
    
    /**
     * Execute batch insert operation with custom connection parameters
     * @param connectionString [const char*] Database connection string
     * @param username [const char*] Username
     * @param password [const char*] Password
     * @param batchSize [int] Batch size
     * @return [Commons::POSIXErrors] result
     */
    Commons::POSIXErrors runBatchInsert(const char* connectionString,
                                       const char* username,
                                       const char* password,
                                       int batchSize = 80);

};

}
