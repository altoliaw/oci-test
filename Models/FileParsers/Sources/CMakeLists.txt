add_library(Models.FileParsers
	InitializedFileParser.cpp
	InitializedJsonFileParser.cpp
)

target_include_directories(Models.FileParsers PUBLIC ../Headers ../../Commons/Headers)