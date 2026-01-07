#include "../Headers/TestDataModel.hpp"
using namespace std;
TestDataModel::TestDataModel() {
    for ( int i = 0; i < 50; i++ ) {
        features.push_back("extremely_long_value_of_column_with_extremely_long_name_number");
    }
}
std::vector<char*> TestDataModel::GetFeatures() {
    return features;
}