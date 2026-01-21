add_library(OciRaw.Sources
    OciRawController.cpp
)
target_include_directories(OciRaw.Sources PUBLIC
    ../../Headers/OciRawControllers
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI_12.1.0.2/Includes
)
target_link_directories(OciRaw.Sources PUBLIC
    ${STATIC_VENDOR_BINARY_DIR_PATH}/OCI_12.1.0.2/Libs
)
target_link_libraries(OciRaw.Sources PUBLIC
   # cd /home/dbsecure/oci-test/Vendors/OCI_10.1.0.5/Libs && ln -sf libclntsh.so.10.1 libclntsh.so && ln -sf libocci.so.10.1 libocci.so && ls -la
    clntsh
)
