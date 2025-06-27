if (NOT BUILD_TEST)

	add_executable(
		OciMain
		OciMain.cpp
	)
	
	# Linking to the headers
	target_include_directories(OciMain PUBLIC 
		../Headers/OciController
		${STATIC_VENDOR_BINARY_DIR_PATH}/OCILib/Includes
		${STATIC_VENDOR_BINARY_DIR_PATH}/OracleInstantClient/Includes
	)
	target_link_directories(OciMain PUBLIC 
		${STATIC_VENDOR_BINARY_DIR_PATH}/OCILib/Libs
		${STATIC_VENDOR_BINARY_DIR_PATH}/OracleInstantClient/Libs
	)

	# Linux compiling / Windows compiling
	target_link_libraries(
		OciMain 
		OciMain.Sources
		Models.OCILib
		# Linux built-in libraries
		pthread
		# Vendor-specific libraries
		
		ocilib
		clntsh
		dl
	)
	
endif()