# Creating an execution in each model
add_executable(
	Tests.FileParsers
	Test_InitializedFileParser.cpp
	Test_InitializedJsonFileParser.cpp
)

# linking to the headers
target_include_directories(
	Tests.FileParsers PUBLIC 
	../Headers 
	../../../../Models/Commons/Headers 
	../../../../Models/FileParsers/Headers
	${STATIC_VENDOR_BINARY_DIR_PATH}/cJson/Includes
)

# linking to the gtest library
target_link_libraries(
	Tests.FileParsers
	Models.FileParsers
	Models.Commons
	${STATIC_VENDOR_BINARY_DIR_PATH}/cJson/Libs/libcjson.a
	GTest::gtest_main

)
		
include(GoogleTest)

# Registering the tests
add_test(NAME Tests.FileParsers COMMAND Tests.FileParsers --gtest_color=yes)