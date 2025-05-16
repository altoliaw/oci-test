#include "../Headers/Test_HashTable.hpp"

#ifdef __linux__
TEST(Commons, HashTable_addNRemoveFirstElements) {
    Commons::HashTable table(1);
    std::string columnName = "default";
    std::string value = "./test/2024.log";
    size_t sizeOfMemory = strlen(value.c_str());
    Commons::HashTable::ElementType type = Commons::HashTable::ElementType::charStarType;
    // Adding an element
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 2nd element
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 3rd element
    columnName = "eyes";
    value = "./test/3343.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Predicting the results
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    char* predictedColumnName = nullptr;
    void* predictedValue = nullptr;
    size_t predictedSizeOfMemory = 0;
    char number = table.getValueByName((char*)(columnName.c_str()), &predictedColumnName, &predictedValue, &predictedSizeOfMemory, &type);

    ASSERT_EQ(number, 0x1);                                   // Return 0x1 (true)

    // Verifying the number in the hash table from the queue
    int numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 3);

    // Removing the element from the first element
    columnName = "default";
    Commons::POSIXErrors result = table.removeElementByName((char*)(columnName.c_str()));
    ASSERT_EQ(result, Commons::POSIXErrors::OK);

    // Verifying the number in the hash table from the queue
    numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 2);

    // Verifying the number in the hash table from the hash table
    numberInHash = 0;
    for (int i = 0; i < table.hashTableSize; i++) {
        for (Commons::HashTable::Element* current = table.hashTable[i]; current != nullptr; current = current->nextInHashTable) {
            numberInHash++;
        }
    }
    ASSERT_EQ(numberInHash, 2);
}

TEST(Commons, HashTable_addNRemoveSecondElements) {
    Commons::HashTable table(1);
    std::string columnName = "default";
    std::string value = "./test/2024.log";
    size_t sizeOfMemory = strlen(value.c_str());
    Commons::HashTable::ElementType type = Commons::HashTable::ElementType::charStarType;
    // Adding an element
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 2nd element
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 3rd element
    columnName = "eyes";
    value = "./test/3343.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Predicting the results
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    char* predictedColumnName = nullptr;
    void* predictedValue = nullptr;
    size_t predictedSizeOfMemory = 0;
    char number = table.getValueByName((char*)(columnName.c_str()), &predictedColumnName, &predictedValue, &predictedSizeOfMemory, &type);

    ASSERT_EQ(number, 0x1);                                   // Return 0x1 (true)
    std::string tmpPredictedColumnName(predictedColumnName);  // Transforming the string format
    ASSERT_EQ(columnName, tmpPredictedColumnName);
    std::string tmpPredictedValue((char*)predictedValue);
    ASSERT_EQ(value, tmpPredictedValue);
    ASSERT_EQ(sizeOfMemory, predictedSizeOfMemory);

    // Verifying the number in the hash table from the queue
    int numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 3);

    // Removing the element from the last element
    columnName = "decoder";
    Commons::POSIXErrors result = table.removeElementByName((char*)(columnName.c_str()));
    ASSERT_EQ(result, Commons::POSIXErrors::OK);

    // Verifying the number in the hash table from the queue
    numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 2);

    // Verifying the number in the hash table from the hash table
    numberInHash = 0;
    for (int i = 0; i < table.hashTableSize; i++) {
        for (Commons::HashTable::Element* current = table.hashTable[i]; current != nullptr; current = current->nextInHashTable) {
            numberInHash++;
        }
    }
    ASSERT_EQ(numberInHash, 2);
}

TEST(Commons, HashTable_addNRemoveLastElements) {
    Commons::HashTable table(1);
    std::string columnName = "default";
    std::string value = "./test/2024.log";
    size_t sizeOfMemory = strlen(value.c_str());
    Commons::HashTable::ElementType type = Commons::HashTable::ElementType::charStarType;
    // Adding an element
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 2nd element
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 3rd element
    columnName = "eyes";
    value = "./test/3343.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Predicting the results
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    char* predictedColumnName = nullptr;
    void* predictedValue = nullptr;
    size_t predictedSizeOfMemory = 0;
    char number = table.getValueByName((char*)(columnName.c_str()), &predictedColumnName, &predictedValue, &predictedSizeOfMemory, &type);

    ASSERT_EQ(number, 0x1);                                   // Return 0x1 (true)

    // Verifying the number in the hash table from the queue
    int numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 3);

    // Removing the element from the first element
    columnName = "eyes";
    Commons::POSIXErrors result = table.removeElementByName((char*)(columnName.c_str()));
    ASSERT_EQ(result, Commons::POSIXErrors::OK);

    // Verifying the number in the hash table from the queue
    numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 2);

    // Verifying the number in the hash table from the hash table
    numberInHash = 0;
    for (int i = 0; i < table.hashTableSize; i++) {
        for (Commons::HashTable::Element* current = table.hashTable[i]; current != nullptr; current = current->nextInHashTable) {
            numberInHash++;
        }
    }
    ASSERT_EQ(numberInHash, 2);
}

TEST(Commons, HashTable_addNRemoveNonExistedElements) {
    Commons::HashTable table(1);
    std::string columnName = "default";
    std::string value = "./test/2024.log";
    size_t sizeOfMemory = strlen(value.c_str());
    Commons::HashTable::ElementType type = Commons::HashTable::ElementType::charStarType;
    // Adding an element
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 2nd element
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Adding the 3rd element
    columnName = "eyes";
    value = "./test/3343.log";
    sizeOfMemory = strlen(value.c_str());
    type = Commons::HashTable::ElementType::charStarType;
    table.addElementIntoHashTable((char*)(columnName.c_str()), (void*)(value.c_str()), sizeOfMemory, type);

    // Predicting the results
    columnName = "decoder";
    value = "./test/334.log";
    sizeOfMemory = strlen(value.c_str());
    char* predictedColumnName = nullptr;
    void* predictedValue = nullptr;
    size_t predictedSizeOfMemory = 0;
    char number = table.getValueByName((char*)(columnName.c_str()), &predictedColumnName, &predictedValue, &predictedSizeOfMemory, &type);

    ASSERT_EQ(number, 0x1);                                   // Return 0x1 (true)

    // Verifying the number in the hash table from the queue
    int numberInHash = 0;
    for (Commons::HashTable::Element* start = table.queue; start != nullptr; start = start->nextInQueue) {
        numberInHash++;
    }
    ASSERT_EQ(numberInHash, 3);

    // Removing the element from the first element
    columnName = "jesus";
    Commons::POSIXErrors result = table.removeElementByName((char*)(columnName.c_str()));
    ASSERT_EQ(result, Commons::POSIXErrors::E_NOITEM);
}
#endif