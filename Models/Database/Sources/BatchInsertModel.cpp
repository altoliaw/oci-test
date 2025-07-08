/** @file BatchInsertModel.cpp
 * @see BatchInsertModel.hpp
 */

#include "../Headers/BatchInsertModel.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Database {

BatchInsertModel::BatchInsertModel() 
    : batchData(), batchSize(0), fakeData(nullptr) {
    memset(batchData, 0, sizeof(batchData));
    initializeFakeData();
}

BatchInsertModel::~BatchInsertModel() {
}

void BatchInsertModel::initializeFakeData() {
    fakeData = "extremely_long_value_of_column_with_extremely_long_name_number";
}

const char* BatchInsertModel::generateInsertSQL() {
    static const char* sql = 
        "INSERT INTO wubai_table_actually_wushi ("
        "extremely_long_column_name_one, extremely_long_column_name_two, extremely_long_column_name_three, "
        "extremely_long_column_name_four, extremely_long_column_name_five, extremely_long_column_name_six, "
        "extremely_long_column_name_seven, extremely_long_column_name_eight, extremely_long_column_name_nine, "
        "extremely_long_column_name_ten, extremely_long_column_name_eleven, extremely_long_column_name_twelve, "
        "extremely_long_column_name_thirteen, extremely_long_column_name_fourteen, extremely_long_column_name_fifteen, "
        "extremely_long_column_name_sixteen, extremely_long_column_name_seventeen, extremely_long_column_name_eighteen, "
        "extremely_long_column_name_nineteen, extremely_long_column_name_twenty, extremely_long_column_name_twenty_one, "
        "extremely_long_column_name_twenty_two, extremely_long_column_name_twenty_three, extremely_long_column_name_twenty_four, "
        "extremely_long_column_name_twenty_five, extremely_long_column_name_twenty_six, extremely_long_column_name_twenty_seven, "
        "extremely_long_column_name_twenty_eight, extremely_long_column_name_twenty_nine, extremely_long_column_name_thirty, "
        "extremely_long_column_name_thirty_one, extremely_long_column_name_thirty_two, extremely_long_column_name_thirty_three, "
        "extremely_long_column_name_thirty_four, extremely_long_column_name_thirty_five, extremely_long_column_name_thirty_six, "
        "extremely_long_column_name_thirty_seven, extremely_long_column_name_thirty_eight, extremely_long_column_name_thirty_nine, "
        "extremely_long_column_name_forty, extremely_long_column_name_forty_one, extremely_long_column_name_forty_two, "
        "extremely_long_column_name_forty_three, extremely_long_column_name_forty_four, extremely_long_column_name_forty_five, "
        "extremely_long_column_name_forty_six, extremely_long_column_name_forty_seven, extremely_long_column_name_forty_eight, "
        "extremely_long_column_name_forty_nine, extremely_long_column_name_fifty) "
        "VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,"
        ":21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,"
        ":41,:42,:43,:44,:45,:46,:47,:48,:49,:50)";
    
    return sql;
}

Commons::POSIXErrors BatchInsertModel::prepareBatchData(int size) {
    batchSize = size;
    for (int i = 0; i < batchSize; ++i) {
        batchData[i] = const_cast<char*>(fakeData);
    }
    return Commons::POSIXErrors::OK;
}

Commons::POSIXErrors BatchInsertModel::bindArrayData(OCI_Statement* statement) {
    if (!OCI_BindArraySetSize(statement, batchSize)) {
        return Commons::POSIXErrors::E_IO;
    }
    
    for (int i = 1; i <= COLUMN_COUNT; ++i) {
        char paramName[100];
        snprintf(paramName, sizeof(paramName), ":%d", i);
        
        OCI_BindArrayOfStrings(statement, paramName, (char*)batchData, sizeof(fakeData), batchSize);
    }
    
    return Commons::POSIXErrors::OK;
}

bool BatchInsertModel::validateData() const {
    return batchData != nullptr && batchSize > 0 && fakeData != nullptr;
}

}