# When the type does not belong to the unit test
if (NOT BUILD_TEST)

	add_executable(
		SizingMain
		SizingMain.cpp
	)
	
	# Linking to the headers
	target_include_directories(SizingMain PUBLIC ../Headers/SizingController)

	# Linux compiling / Windows compiling
	if(OPERATING_SYSTEM STREQUAL "0") # Linux
		include(${CMAKE_CURRENT_SOURCE_DIR}/.Cmake/.linuxVersion.cmake)
	else() # Windows
		include(${CMAKE_CURRENT_SOURCE_DIR}/.Cmake/.windowsVersion.cmake)
	endif()
endif()