# Linux Linking Information
target_link_libraries(
	SizingMain
	Main.Sources
	Models.Commons
	Models.FileParsers
	Models.PCAP
	# Linux built-in libraries
	pthread
	# Vendor-specific libraries
	${STATIC_VENDOR_BINARY_DIR_PATH}/cJson/Libs/libcjson.a
	${STATIC_VENDOR_BINARY_DIR_PATH}/libpcap/Libs/libpcap.a
)