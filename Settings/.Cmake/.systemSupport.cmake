# [System Support]
# This file sets the OPERATING_SYSTEM variable based on the platform. 
# If the platform does not match Linux or Windows, it will raise an error (exiting the process).
if(UNIX)
    set(OPERATING_SYSTEM 0 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
elseif(WIN32)
    set(OPERATING_SYSTEM 1 CACHE STRING "Operating system flag (string format). 0 for Linux, 1 for Windows")
else()
    message(FATAL_ERROR "Unsupported platform! Only Linux and Windows are supported.")
endif()