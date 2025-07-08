#pragma once
/** @file BatchInsertModel.hpp
 * Batch insert data
 * 
 * @author johnny.gu
 * @date 2025/06/20
 */

#include <ocilib.h>
#include "../../Commons/Headers/POSIXErrors.hpp"

namespace Database {

/**
 * Batch insert data model class
 */
class BatchInsertModel {
public:
    BatchInsertModel();
    ~BatchInsertModel();
    
    /**
     * Generate batch insert SQL statement
     * @return [const char*] SQL statement
     */
    const char* generateInsertSQL();
    
    /**
     * Prepare batch data
     * @param batchSize [int] Batch size
     * @return [Commons::POSIXErrors] Preparation result
     */
    Commons::POSIXErrors prepareBatchData(int batchSize = 80);
    
    /**
     * Bind array data to SQL statement
     * @param statement [OCI_Statement*] SQL statement
     * @return [Commons::POSIXErrors] Binding result
     */
    Commons::POSIXErrors bindArrayData(OCI_Statement* statement);
    
    /**
     * Validate data integrity
     * @return [bool] Validation result
     */
    bool validateData() const;

private:
    static const int COLUMN_COUNT = 50;
    static const int DEFAULT_BATCH_SIZE = 80;
    
    char* batchData[DEFAULT_BATCH_SIZE];
    int batchSize;
    const char* fakeData;
    
    /**
     * Initialize fake data
     */
    void initializeFakeData();
};

}
