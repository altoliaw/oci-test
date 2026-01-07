#pragma once
/** @file TestDataModel.hpp
 * The test data model for database operations
 *
 * @author Nick, Liao
 * @date 2025/06/26
 */
#include <vector>

/**
 * The test data model class
 */
class TestDataModel {
   public:
    /**
     * Constructor; the constructor will initialize the test data with 50 string values
     */
    TestDataModel();

    /**
     * Obtaining the features from the data model
     *
     * @return [std::vector<const char*>] The vector containing the feature strings
     */
    std::vector<const char*> GetFeatures();

   private:
    // The feature strings which will be used for testing
    std::vector<const char*> features;
};
