if (NOT BUILD_TEST)
    # [OCI Set for Different OCI Version]
    include(../../Settings/.Cmake/OciRaw/.ociRawVersion.cmake)

    # For different version of OCI, we can use a variable to 
    # control the include and link directories. 

    foreach(OCI_FOLDER_NAME ${OCI_FOLDER_NAME_SET})
        add_executable( OciRaw.${OCI_FOLDER_NAME}
            OciRaw.cpp
        )

        # Adding a post-build command to copy the executable to the Bin directory
        add_custom_command(
            TARGET OciRaw.${OCI_FOLDER_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:OciRaw.${OCI_FOLDER_NAME}>"
                "${CMAKE_SOURCE_DIR}/Bin/"
            COMMENT "Copying executable to ${CMAKE_SOURCE_DIR}/Bin/"
        )

        # Including the OciRawController header directory
        target_include_directories(OciRaw.${OCI_FOLDER_NAME} PUBLIC
            ../../Headers/OciRawControllers
        )

        # Linking the OciRaw.Sources controller library
        target_link_libraries(OciRaw.${OCI_FOLDER_NAME}
            OciRaw.${OCI_FOLDER_NAME}.Sources
        )
    endforeach()
endif()
