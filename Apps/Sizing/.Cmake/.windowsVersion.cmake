# Creating an execution in each model
set(CMAKE_EXE_LINKER_FLAGS "-static")
# For windows compiling because some third party libraries are made by MSVC
# Linking to the all library
target_link_libraries(
	SizingMain -static
	Main.Sources
	Models.Commons
	Models.FileParsers
	Models.PCAP
	# Windows built-in libraries
	ws2_32
	# Vendor-specific libraries
	${STATIC_VENDOR_BINARY_DIR_PATH}/cJson/Libs/libcjson.a
	${STATIC_VENDOR_BINARY_DIR_PATH}/nPcap/Libs/x64/Packet.lib
	${STATIC_VENDOR_BINARY_DIR_PATH}/nPcap/Libs/x64/wpcap.lib
)