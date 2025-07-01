add_library(Oci.Sources
    OciController.cpp
)
target_include_directories(Oci.Sources PUBLIC 
    ../../Headers/OciController
    ../../Models/DataModels/Headers
    ../../Models/DBUtils/Headers
)
target_link_libraries(Oci.Sources PUBLIC
    Models.DataModels.TestDataModel
    Models.DBUtils
)