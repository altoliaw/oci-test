if (NOT BUILD_TEST)
    add_executable( Oci
        Oci.cpp
    )

    # Add a post-build command to copy the executable to the Bin directory
	add_custom_command(
		TARGET Oci
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:Oci>"
			"${CMAKE_SOURCE_DIR}/Bin/"
		COMMENT "Copying executable to ${CMAKE_SOURCE_DIR}/Bin/"
	)

    target_include_directories(Oci PUBLIC ../Headers/OciControllers)
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