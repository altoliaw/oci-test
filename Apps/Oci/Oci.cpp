/** @file Oci.cpp
 * The main entry point for Oracle database testing application
 *
 * @author Nick, Liao
 * @date 2025/06/26
 */
#include "../../Headers/OciControllers/OciController.hpp"

/**
 * The main entry point of the Oracle database testing application; the function will
 * create an instance of OciController and execute test operations
 *
 * @param argc [int] The number of arguments
 * @param argv [char**] The array of arguments
 * @return [int] The exit status; 0 indicates success
 */
int main(int argc, char** argv) {
    // Creating an instance of the OciController
    OciController controller;

    // Testing the batch insertion (commented out)
    // controller.start();
    // controller.Insert2Columns();

    // Testing the data fetching from the database
    controller.FetchDataTest();

    return 0;
}
