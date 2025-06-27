# When the type does not belong to the unit test
if (NOT BUILD_TEST)

	add_executable(
		OracleMain
		OracleMain.cpp
	)
	
	# Linking to the headers
	target_include_directories(OracleMain PUBLIC ../Headers/OracleController)
	
	include(${CMAKE_CURRENT_SOURCE_DIR}/.Cmake/.linkingInformation.cmake)
endif()