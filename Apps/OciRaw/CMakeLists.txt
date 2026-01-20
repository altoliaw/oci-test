if (NOT BUILD_TEST)
    add_executable( OciRaw
        Oci19.cpp
    )

    # Adding a post-build command to copy the executable to the Bin directory
	add_custom_command(
		TARGET OciRaw
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:OciRaw>"
			"${CMAKE_SOURCE_DIR}/Bin/"
		COMMENT "Copying executable to ${CMAKE_SOURCE_DIR}/Bin/"
	)

    # Including the OciRawController header directory
    target_include_directories(OciRaw PUBLIC
        ../../Headers/OciRawControllers
    )

    # Linking the OciRaw.Sources controller library
    target_link_libraries( OciRaw
        OciRaw.Sources
    )
endif()
