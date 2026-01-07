# Linux compiling / Windows compiling
if(OPERATING_SYSTEM STREQUAL "0" OR OPERATING_SYSTEM STREQUAL "0.1")

	add_library(Sources.SizingControllers
		SizingMainControllerPrototype.cpp
		SizingMainController.cpp
		LinuxSizingMainController.cpp
		LinuxCentosSizingMainController.cpp
	)

	set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)

	target_include_directories(Sources.SizingControllers PUBLIC 
		../../Headers/SizingControllers
		${STATIC_LIB_PATH}/libpcap/Includes)
	
	# To determine which code in the c++ file shall be joined because of different linux distributions
	if(OPERATING_SYSTEM STREQUAL "0.1")
		target_compile_definitions(Sources.SizingControllers PUBLIC OS_ID=${OS_ID})
	endif()

else() # Windows
	set(STATIC_LIB_PATH ${CMAKE_BINARY_DIR}/Vendors)
	# 1   -> Npcap
	# 1.1 -> WinDivert
	if(OPERATING_SYSTEM STREQUAL "1")
		add_library(Sources.SizingControllers
			SizingMainControllerPrototype.cpp
			SizingMainController.cpp
			WindowsNpcapSizingMainController.cpp
		)
		target_include_directories(Sources.SizingControllers PUBLIC 
			../../Headers/SizingControllers
			${STATIC_LIB_PATH}/npcap/Includes
		)
	elseif(OPERATING_SYSTEM STREQUAL "1.1")
		add_library(Sources.SizingControllers
			SizingMainControllerPrototype.cpp
			SizingMainController.cpp
			WindowsWinDivertSizingMainController.cpp
		)
		target_include_directories(Sources.SizingControllers PUBLIC 
			../../Headers/SizingControllers
			${STATIC_LIB_PATH}/WinDivert/Includes
		)
		target_compile_definitions(Sources.SizingControllers PUBLIC OS_ID=${OS})
	else()
		message(FATAL_ERROR "No valid OPERATING_SYSTEM specified.")
	endif()
endif()