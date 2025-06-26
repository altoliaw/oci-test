# [System Support]
## This file sets the OPERATING_SYSTEM variable based on the platform. 
## If the platform does not match Linux or Windows, it will raise an error (exiting the process).
## To compile the windows version, users shall use MINGW64 from the MSYS2 environment. However, the platform is still detected as a Linux platform 
## in Cmake when compiling; as the result, the OS conditional shall be determine whether RHEL and MSYS2.
if(UNIX)
    ###########################################################################################################
    # Verifying if the system is Linux(RHEL series) or MSYS2 (which is a POSIX-compatible environment)
    if(CMAKE_SYSTEM_NAME STREQUAL "LINUX")
        set(OPERATING_SYSTEM 0 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "MSYS")
        # MSYS2 is a POSIX-compatible environment, but users shall treat it as Windows for the purposes
        set(OPERATING_SYSTEM 1 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
    else()
        set(OPERATING_SYSTEM 0 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
    endif()
    ###########################################################################################################
elseif(WIN32)
    set(OPERATING_SYSTEM 1 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
else()
    message(FATAL_ERROR "Unsupported platform! Only Linux and Windows are supported.")
endif()