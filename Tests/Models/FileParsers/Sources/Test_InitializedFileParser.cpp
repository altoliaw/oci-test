#include "../Headers/Test_InitializedFileParser.hpp"

#ifdef __linux__
TEST(FileParsers, Test_InitializedFileParser_getInitializedFileParserInitialization) {
    std::unique_ptr<FileParsers::InitializedFileParser>& instance =
        FileParsers::InitializedFileParser::getInitializedFileParserInitialization();
    ASSERT_NE(nullptr, instance);
	FileParsers::InitializedFileParser::releaseInitializedFileParserInitialization();
}

TEST(FileParsers, Test_InitializedFileParser_parseInitializedFile) {
    const unsigned char* path = (const unsigned char*)"../../../../Settings/.Ini/globalCompiling.ini";
    FileParsers::InitializedFileParser::parseInitializedFile(path);
	unsigned char value [256] = {'\0'};
	Commons::POSIXErrors error= FileParsers::InitializedFileParser::getValueFromFileParser((const unsigned char*)"compiler.WALL", value);
	ASSERT_EQ(error, Commons::POSIXErrors::OK);
	ASSERT_STREQ((char*)"-Wall", (char*)value);

	error= FileParsers::InitializedFileParser::getValueFromFileParser((const unsigned char*)"compiler.FMSG", value);
	ASSERT_EQ(error, Commons::POSIXErrors::OK);
	ASSERT_STREQ((char*)"-fmessage-length=0 -pthread", (char*)value);
	FileParsers::InitializedFileParser::releaseInitializedFileParserInitialization();
}

TEST(FileParsers, Test_InitializedFileParser_getInitializedFileParserInitialization2) {
    std::unique_ptr<FileParsers::InitializedFileParser>& instance =
        FileParsers::InitializedFileParser::getInitializedFileParserInitialization();
    ASSERT_NE(nullptr, instance);

	unsigned char value [256] = {'\0'};
	Commons::POSIXErrors error= FileParsers::InitializedFileParser::getValueFromFileParser((const unsigned char*)"compiler.WALL", value);
	ASSERT_NE(error, Commons::POSIXErrors::OK);// This shows that the singleton memory can be released definitely.

	FileParsers::InitializedFileParser::releaseInitializedFileParserInitialization();
}


TEST(FileParsers, Test_InitializedFileParser_parseInitializedFile2) {
    const unsigned char* path = (const unsigned char*)"../../../../Settings/.Ini/globalCompiling.ini";
    FileParsers::InitializedFileParser::parseInitializedFile(path);
	unsigned char value [256] = {'\0'};
	Commons::POSIXErrors error= FileParsers::InitializedFileParser::getValueFromFileParser((const unsigned char*)"compiler.WALL", value);
	ASSERT_EQ(error, Commons::POSIXErrors::OK);
	ASSERT_STREQ((char*)"-Wall", (char*)value);

	error= FileParsers::InitializedFileParser::getValueFromFileParser((const unsigned char*)"compiler.FMSG", value);
	ASSERT_EQ(error, Commons::POSIXErrors::OK);
	ASSERT_STREQ((char*)"-fmessage-length=0 -pthread", (char*)value);
	FileParsers::InitializedFileParser::releaseInitializedFileParserInitialization();
}
#endif