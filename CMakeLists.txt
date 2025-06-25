cmake_minimum_required(VERSION 3.14)
project(native-sizing)

# [C++ Settings]
## GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# [Global CMake Variable Information]
## This file contains global CMake variables used across the project.
## The file is a manual reference for developers to for the definition of the global variables.
## Please refer to the file in the following:
include(Settings/.Cmake/.globalCmakeVariable.cmake)

# [System Support]
# This file sets the OPERATING_SYSTEM variable based on the platform. 
# If the platform does not match Linux or Windows, it will raise an error (exiting the process).
include(Settings/.Cmake/.systemSupport.cmake)

# [Testing Settings]
## Please refer to the file in the following:
## BUILD_TEST is used to determine whether the unit test is enabled (ON) or not (OFF)
include(Settings/.Cmake/.testingInstruction.cmake)

# [Involved Files When Compiling]
## The custom files which shall be copied to the binary directory
include(Settings/.Cmake/.customFileReference.cmake)

# [Included Directories, for Libraries and Controllers]
## The project's directories which shall be included in the compilation
include(Settings/.Cmake/.projectDirectoryIncluded.cmake)

# [Debugging & Testing/Releasing Settings]
include(Settings/.Cmake/.compiledRouter.cmake)