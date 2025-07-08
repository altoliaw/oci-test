if (NOT BUILD_TEST)
    add_executable( Database
        Database.cpp
    )
    target_include_directories(Database PUBLIC ../Headers/Controllers)
    target_link_directories(Database PUBLIC
        ${STATIC_VENDOR_BINARY_DIR_PATH}/ocilib/Libs
        ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Libs
    )
    target_link_libraries( Database
        ocilib
        Sources.DatabaseController
        Models.Database
    )
endif()