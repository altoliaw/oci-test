# For different version of OCI, we can use a variable to 
# control the include and link directories and later we
# can build different version of OCI by using iteration
# OCI_FOLDER_NAME_SET shows different version of OCI folders
# names in Vendors directory
set(OCI_FOLDER_NAME_SET OCI_10.1.0.5 OCI_12.1.0.2 OCI_12.2.0.1 OCI_19.30.0.0)