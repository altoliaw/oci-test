add_library(OciMain.Sources
		OcilibController.cpp
	)

target_include_directories(OciMain.Sources PUBLIC 
    ../../Headers/OciController
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCILib/Includes)