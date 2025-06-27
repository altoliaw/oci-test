# Linking information for Oracle application
target_link_directories(Models.OracleModel PUBLIC
    ${STATIC_VENDOR_BINARY_DIR_PATH}/Ocilib/Libs
    ${STATIC_VENDOR_BINARY_DIR_PATH}/Oracle/Libs
)


target_link_libraries(Models.OracleModel
    PUBLIC
        ocilib
        clntsh
        dl
        pthread
)


target_link_libraries(
		OracleMain
        Sources.OracleController
		Models.OracleModel
)