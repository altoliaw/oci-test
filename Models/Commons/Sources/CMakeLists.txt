add_library(Models.Commons
	StringImplement.cpp
	IOExporters.cpp
	HashTable.cpp
	Time.cpp
)

target_include_directories(Models.Commons PUBLIC ${CMAKE_SOURCE_DIR}/Models/Commons/Headers)

# For old linux version (CentOS 7), in Commons, there exists a determination for OS_ID to insert the definition of "make_unique".
# As a result, here the OS_ID shall be passed as same as the CMakeLists.txt in the SizingControllers.
if(OPERATING_SYSTEM STREQUAL "0.1")
	target_compile_definitions(Models.Commons PUBLIC OS_ID=${OS_ID})
endif()