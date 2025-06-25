#include "../Headers/Test_InitializedJsonFileParser.hpp"

#ifdef __linux__
TEST(FileParsers, Test_InitializedJsonFileParser_getInitializedFileParserInitialization) {
    std::unique_ptr<FileParsers::InitializedJsonFileParser>& instance =
        FileParsers::InitializedJsonFileParser::getInitializedFileParserInitialization();
    ASSERT_NE(nullptr, instance);
    FileParsers::InitializedJsonFileParser::releaseInitializedFileParserInitialization();
}

TEST(FileParsers, Test_InitializedJsonFileParser_parseInitializedFile) {
    const unsigned char* path = (const unsigned char*)"../../../../Settings/.Json/SizingMain.json";
    Commons::POSIXErrors error = FileParsers::InitializedJsonFileParser::parseInitializedFile(path);
    ASSERT_EQ(Commons::POSIXErrors::OK, error);

    unsigned char value[1024] = {'\0'};
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base", value);
    ASSERT_EQ(Commons::POSIXErrors::OK, error);

    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"\"base\"", value);
    ASSERT_NE(Commons::POSIXErrors::OK, error);

    cJSON* current = nullptr;
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.service.[0].port", value, &current);
    /** The types are included cJSON_Object, cJSON_Array, cJSON_String, cJSON_Number, cJSON_True,
     *  cJSON_False, cJSON_NULL and others.
     */
    ASSERT_EQ(Commons::POSIXErrors::OK, error);
    ASSERT_EQ(current->type, cJSON_Array);

    current = nullptr;
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.service.[0].port.[0]", value, &current);
    /** The types are included cJSON_Object, cJSON_Array, cJSON_String, cJSON_Number, cJSON_True,
     *  cJSON_False, cJSON_NULL and others.
     */
    ASSERT_EQ(Commons::POSIXErrors::OK, error);
    ASSERT_EQ(current->type, cJSON_String);
    ASSERT_STREQ((char*)value, "1521");

    current = nullptr;
    error = FileParsers::InitializedJsonFileParser::getValueFromFileParser((const unsigned char*)"base.writingFileSecond", value, &current);
    /** The types are included cJSON_Object, cJSON_Array, cJSON_String, cJSON_Number, cJSON_True,
     *  cJSON_False, cJSON_NULL and others.
     */
    ASSERT_EQ(Commons::POSIXErrors::OK, error);
    ASSERT_EQ(current->type, cJSON_Number);
    // ASSERT_STREQ((char*)value, "30");

	// Converting the unsigned char* to unsigned int
    unsigned int result = 0;
    std::stringstream stream;
    stream.clear();  // Removing the error flags
    stream.str("");  // Removing the value
    stream << value;
    stream >> result;

	ASSERT_EQ(result, 10);

    FileParsers::InitializedJsonFileParser::releaseInitializedFileParserInitialization();
}
#endif