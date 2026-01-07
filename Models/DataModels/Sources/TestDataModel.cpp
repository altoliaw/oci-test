/**
 * @see TestDataModel.hpp
 */
#include "../Headers/TestDataModel.hpp"

using namespace std;

/**
 * Constructor; the constructor will initialize the test data with 50 string values;
 * each value is a long string for testing database operations
 */
TestDataModel::TestDataModel() {
    // Initializing the feature vector with 50 test strings
    for (int i = 0; i < 50; i++) {
        features.push_back("extremely_long_value_of_column_with_extremely_long_name_number");
    }
}

/**
 * Obtaining the features from the data model
 *
 * @return [std::vector<const char*>] The vector containing the feature strings
 */
std::vector<const char*> TestDataModel::GetFeatures() {
    return features;
}
