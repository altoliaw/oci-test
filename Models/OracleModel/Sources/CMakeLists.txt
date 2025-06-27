add_library(Models.OracleModel
    OracleModel.cpp
)

target_include_directories(Models.OracleModel PUBLIC 
		../Headers
        ${STATIC_VENDOR_BINARY_DIR_PATH}/Ocilib/Includes
		${STATIC_VENDOR_BINARY_DIR_PATH}/Oracle/Includes
)

# target_link_directories(Models.OracleModel
#     PUBLIC
#         ${STATIC_VENDOR_BINARY_DIR_PATH}/Ocilib/Libs
# 		${STATIC_VENDOR_BINARY_DIR_PATH}/Oracle/Libs
# )

# target_link_libraries(Models.OracleModel
#     PUBLIC
# 		ocilib
# 		clntsh
# 		dl
# 		pthread
# )
