# Opening the subdirectory for the next works
add_subdirectory(Commons)
add_subdirectory(FileParsers)
add_subdirectory(DataModels)
add_subdirectory(DBUtils)

if (NOT BUILD_TEST)
	add_subdirectory(PCAP)
endif()