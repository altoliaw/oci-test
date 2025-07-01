#pragma once
#include <vector>
class TestDataModel {
public:
    std::vector<char*> GetFeatures();
    TestDataModel();
private:   
    std::vector<char*> features;
};