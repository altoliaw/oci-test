# Linking to the library
set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

# Linux Linking Information
if(OPERATING_SYSTEM STREQUAL "0.1") # CentOS version
target_link_libraries(
	SizingMain
	Sources.SizingControllers
	Services.Sizing
	Models.Commons
	Models.FileParsers
	Models.PCAP
	# Linux built-in libraries
	pthread
	# Vendor-specific libraries
	${STATIC_LIB_PATH}/cJson/Libs/libcjson.a
	${STATIC_LIB_PATH}/libpcap/Libs/libpcap.a
	-lnl-3 -lnl-genl-3
)
else() # Other Linux version
target_link_libraries(
	SizingMain
	Sources.SizingControllers
	Services.Sizing
	Models.Commons
	Models.FileParsers
	Models.PCAP
	# Linux built-in libraries
	pthread
	# Vendor-specific libraries
	${STATIC_LIB_PATH}/cJson/Libs/libcjson.a
	${STATIC_LIB_PATH}/libpcap/Libs/libpcap.a
)
endif()

# Changing the permission of the SizingMain
add_custom_command(TARGET SizingMain POST_BUILD
	COMMAND sudo chown root:root "${CMAKE_SOURCE_DIR}/Bin/SizingMain"
	COMMAND sudo chmod 4755 "${CMAKE_SOURCE_DIR}/Bin/SizingMain"
)