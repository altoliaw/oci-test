# [Involved Files When Compiling]
# Copying the dependent files of the project (cfg, ini files or files users feel like opening)
if (BUILD_TEST)
	configure_file(${CMAKE_SOURCE_DIR}/Settings/.Ini/globalCompiling.ini ${CMAKE_BINARY_DIR}/Settings/.Ini/globalCompiling.ini COPYONLY)
endif()

# Others involved in the testing process and compiling process
configure_file(${CMAKE_SOURCE_DIR}/Settings/.Json/SizingMain.json ${CMAKE_BINARY_DIR}/Settings/.Json/SizingMain.json COPYONLY)

## Copying the third party software into the Vendors folder and the Outputs for testing
file(COPY ${CMAKE_SOURCE_DIR}/Vendors DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Outputs DESTINATION ${CMAKE_BINARY_DIR})