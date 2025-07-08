set(COMMON_INCLUDE_DIRS
    ../Headers
    ${STATIC_VENDOR_BINARY_DIR_PATH}/ocilib/Includes
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Includes
)

add_library(Models.Database
    DatabaseModel.cpp
    BatchInsertModel.cpp
)

target_include_directories(Models.Database PUBLIC ${COMMON_INCLUDE_DIRS})
