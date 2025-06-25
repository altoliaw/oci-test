set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)


if(OPERATING_SYSTEM STREQUAL "0")
	add_library(Models.PCAP
		PCAPPrototype.cpp
		LinuxPCAP.cpp
	)

	target_include_directories(Models.PCAP PUBLIC ../Headers ${STATIC_LIB_PATH}/libpcap/Includes)
else()
	add_library(Models.PCAP
		PCAPPrototype.cpp
		WindowsPCAP.cpp
	)

	target_include_directories(Models.PCAP PUBLIC ../Headers ${STATIC_LIB_PATH}/nPcap/Includes)
endif()