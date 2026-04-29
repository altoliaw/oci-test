# [OCI Set for Different OCI Version]
include(../../Settings/.Cmake/OciRaw/.ociRawVersion.cmake)

# For different version of OCI, we can use a variable to 
# control the include and link directories.
foreach(OCI_FOLDER_NAME ${OCI_FOLDER_NAME_SET})
    add_library(OciRaw.${OCI_FOLDER_NAME}.Sources
        OciRawController.cpp
    )

    target_include_directories(OciRaw.${OCI_FOLDER_NAME}.Sources PUBLIC
        ../../Headers/OciRawControllers
        ${STATIC_VENDOR_BINARY_DIR_PATH}/${OCI_FOLDER_NAME}/Includes
    )
    target_link_directories(OciRaw.${OCI_FOLDER_NAME}.Sources PUBLIC
        ${STATIC_VENDOR_BINARY_DIR_PATH}/${OCI_FOLDER_NAME}/Libs
    )
    target_link_libraries(OciRaw.${OCI_FOLDER_NAME}.Sources PUBLIC
    # cd /home/dbsecure/oci-test/Vendors/OCI_10.1.0.5/Libs && ln -sf libclntsh.so.10.1 libclntsh.so && ln -sf libocci.so.10.1 libocci.so && ls -la
        clntsh
    )
endforeach()