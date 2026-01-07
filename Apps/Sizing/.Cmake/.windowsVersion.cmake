# Linking to the library
set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

# Creating an execution in each model
set(CMAKE_EXE_LINKER_FLAGS "-static")
# For windows compiling because some third party libraries are made by MSVC
# Linking to the all library
target_link_libraries(
	SizingMain
	PUBLIC
		Sources.SizingControllers
		Services.Sizing
		Models.Commons
		Models.FileParsers
		Models.PCAP
		# Windows built-in libraries
		iphlpapi
		ws2_32
		# Vendor-specific libraries
		${STATIC_LIB_PATH}/cJson/Libs/libcjson.a
)

if(OPERATING_SYSTEM STREQUAL "1")
	target_link_libraries(SizingMain PUBLIC ${STATIC_LIB_PATH}/nPcap/Libs/x64/wpcap.lib)
elseif(OPERATING_SYSTEM STREQUAL "1.1")
	target_link_libraries(SizingMain PUBLIC ${STATIC_LIB_PATH}/winDivert/Libs/WinDivert.lib)
endif()