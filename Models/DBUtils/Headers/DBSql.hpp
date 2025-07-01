#pragma once
#include <vector>
#include <cstdio>
#include "../../DataModels/Headers/TestDataModel.hpp"
#include "DBConnector.hpp"
namespace DBSql {

void BatchInsert(char* sql, std::vector<TestDataModel> models, int n_feature);

}