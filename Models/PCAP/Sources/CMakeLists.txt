set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

add_library(Models.PCAP
	PCAPPrototype.cpp
	WindowsPCAP.cpp
)

target_include_directories(Models.PCAP PUBLIC ../Headers ${STATIC_LIB_PATH}/nPcap/Includes)