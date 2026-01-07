add_library(Services.Sizing
	Transformer.cpp
)

target_include_directories(Services.Sizing PUBLIC ${CMAKE_SOURCE_DIR}/Services/Sizing/Headers)