# [Global CMake Variable Information]
## This file contains global CMake variables used across the project.
## The file is a manual reference for developers to for the definition of the global variables.
## The definition is listed in:
## - BUILD_TEST: .testingInstruction.cmake
## - OPERATING_SYSTEM: .systemSupport.cmake
## - STATIC_VENDOR_BINARY_DIR_PATH: .globalCmakeVariable.cmake

# The path of Vendors' in the linking phase
set(STATIC_VENDOR_BINARY_DIR_PATH ${CMAKE_BINARY_DIR}/Vendors CACHE STRING "The path of Vendors' in the linking phase")