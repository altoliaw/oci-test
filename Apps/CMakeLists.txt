if (NOT BUILD_TEST)
	# For windows compiling because some third party libraries are made by MSVC
	# Creating an execution in each model
	set(CMAKE_EXE_LINKER_FLAGS "-static")
	add_executable(
		SizingMain_Windows
		SizingMain.cpp
	)
		
	# Linking to the library
	set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

	# Linking to the headers
	target_include_directories(SizingMain_Windows PUBLIC ../Headers/SizingController)

	# Linking to the gtest library
	target_link_libraries(
		SizingMain_Windows -static
		Main.Sources
		Models.Commons
		Models.FileParsers
		Models.PCAP
		# Windows built-in libraries
		ws2_32
		# Vendor-specific libraries
		${STATIC_LIB_PATH}/cJson/Libs/libcjson.a
		${STATIC_LIB_PATH}/nPcap/Libs/x64/Packet.lib
		${STATIC_LIB_PATH}/nPcap/Libs/x64/wpcap.lib
	)
endif()