# Creating an execution in each model
add_executable(
	Tests.FileParsers
	Test_InitializedFileParser.cpp
	Test_InitializedJsonFileParser.cpp
)

set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

# linking to the headers
target_include_directories(
	Tests.FileParsers PUBLIC 
	../Headers 
	../../../../Models/Commons/Headers 
	../../../../Models/FileParsers/Headers
	${STATIC_LIB_PATH}/cJson/Includes
)

# linking to the gtest library
target_link_libraries(
	Tests.FileParsers
	Models.FileParsers
	Models.Commons
	${STATIC_LIB_PATH}/cJson/Libs/libcjson.a
	GTest::gtest_main

)
		
include(GoogleTest)

# Registering the tests
add_test(NAME Tests.FileParsers COMMAND Tests.FileParsers --gtest_color=yes)