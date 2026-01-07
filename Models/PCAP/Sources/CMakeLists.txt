set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

# Linux compiling / Windows compiling
if(OPERATING_SYSTEM STREQUAL "0" OR OPERATING_SYSTEM STREQUAL "0.1")
	add_library(Models.PCAP
		PCAPPrototype.cpp
		LinuxPCAP.cpp
	)

	target_include_directories(Models.PCAP 
		PUBLIC 
		${CMAKE_SOURCE_DIR}/Models/PCAP/Headers 
		${STATIC_LIB_PATH}/libpcap/Includes
	)
else() # For Windows
	# 1   -> Npcap
	# 1.1 -> WinDivert
	if(OPERATING_SYSTEM STREQUAL "1")
		add_library(Models.PCAP
			PCAPPrototype.cpp
			WindowsNpcapPCAP.cpp
		)
		target_include_directories(Models.PCAP PUBLIC
			${CMAKE_SOURCE_DIR}/Models/PCAP/Headers
			${STATIC_LIB_PATH}/npcap/Includes
		)
	elseif(OPERATING_SYSTEM STREQUAL "1.1")
		add_library(Models.PCAP
			PCAPPrototype.cpp
			WindowsWinDivertPCAP.cpp
		)
		target_include_directories(Models.PCAP PUBLIC
			${CMAKE_SOURCE_DIR}/Models/PCAP/Headers
			${STATIC_LIB_PATH}/winDivert/Includes
		)
		target_compile_definitions(Models.PCAP PUBLIC OS_ID=${OS})
	else()
		message(FATAL_ERROR "No valid OPERATING_SYSTEM specified.")
	endif()
endif()