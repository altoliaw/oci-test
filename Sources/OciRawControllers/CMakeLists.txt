add_library(OciRaw.Sources
    OciRawController.cpp
)
target_include_directories(OciRaw.Sources PUBLIC
    ../../Headers/OciRawControllers
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Includes
)
target_link_directories(OciRaw.Sources PUBLIC
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Libs
)
target_link_libraries(OciRaw.Sources PUBLIC
    clntsh
)
