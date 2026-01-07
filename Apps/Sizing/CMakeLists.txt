# When the type does not belong to the unit test
if (NOT BUILD_TEST)

	add_executable(
		SizingMain
		SizingMain.cpp
	)
	
	# Add a post-build command to copy the executable to the Bin directory
	add_custom_command(
		TARGET SizingMain
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:SizingMain>"
			"${CMAKE_SOURCE_DIR}/Bin/"
		COMMENT "Copying executable to ${CMAKE_SOURCE_DIR}/Bin/"
	)

	# Linking to the headers
	target_include_directories(SizingMain PUBLIC ../Headers/SizingControllers)
	
	# Linking to the library
	set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

	# Linux compiling / Windows compiling
	if(OPERATING_SYSTEM STREQUAL "0") # Linux
		include(${CMAKE_CURRENT_SOURCE_DIR}/.Cmake/.linuxVersion.cmake)
	else() # Windows
		include(${CMAKE_CURRENT_SOURCE_DIR}/.Cmake/.windowsVersion.cmake)
	endif()
endif()