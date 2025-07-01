add_library(Models.DBUtils
    DBConnector.cpp
    DBSql.cpp
)

target_include_directories(Models.DBUtils PUBLIC 
    ../Headers
    ../../DataModels/Headers
    ${STATIC_VENDOR_BINARY_DIR_PATH}/ocilib/Includes
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Includes
)

# target_link_directories(Models.DBUtils PUBLIC
#     ${STATIC_VENDOR_BINARY_DIR_PATH}/ocilib/Libs
#     ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Libs
# )