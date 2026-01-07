if (NOT BUILD_TEST)
    add_executable( Oci
        Oci.cpp
    )
    target_include_directories(Oci PUBLIC ../Headers/OciController)
    target_link_directories(Oci PUBLIC
        ${STATIC_VENDOR_BINARY_DIR_PATH}/ocilib/Libs
        ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI/Libs
    )
    target_link_libraries( Oci
        ocilib
        Models.DBUtils
        Models.DataModels.TestDataModel
        Oci.Sources
    )
endif()