#pragma once
#include <vector>
#include <cstdio>
#include <cstring>
#include "../../DataModels/Headers/TestDataModel.hpp"
#include "DBConnector.hpp"
namespace DBSql {
void BatchInsertFromString(char* sql, char*** featrues, int count, int n_feature, int len);
void FetchDataTest(char* sql);
}