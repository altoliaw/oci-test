# Linux compiling / Windows compiling
if(OPERATING_SYSTEM STREQUAL "0")

	add_library(Main.Sources
		SizingMainCallerPrototype.cpp
		SizingMainCaller.cpp
		LinuxSizingMainCaller.cpp
	)

	target_include_directories(Main.Sources PUBLIC 
		../../Headers/SizingController
		${STATIC_VENDOR_BINARY_DIR_PATH}/libpcap/Includes)

else()
	add_library(Main.Sources
		SizingMainCallerPrototype.cpp
		SizingMainCaller.cpp
		WindowsSizingMainCaller.cpp
	)

	target_include_directories(Main.Sources PUBLIC 
		../../Headers/SizingController
		${STATIC_VENDOR_BINARY_DIR_PATH}/nPcap/Includes)
endif()