/** @file Oci19.cpp
 * The main entry point for Oracle database testing application using raw OCI API
 *
 * This application demonstrates the usage of Oracle Call Interface (OCI) without
 * the ocilib wrapper library. The program shall create an OciRawController instance
 * and execute test operations.
 *
 * @author Nick, Liao
 * @date 2025/06/26
 */

#ifdef __linux__

#include "../../Headers/OciRawControllers/OciRawController.hpp"
#include <iostream>

/**
 * The main entry point of the Oracle database testing application; the function
 * shall create an instance of OciRawController and execute test operations
 *
 * @param argc [int] The number of command-line arguments
 * @param argv [char**] The array of command-line arguments
 * @return [int] The exit status; 0x0 indicates success, otherwise 0x1
 */
int main(int argc, char** argv) {
    std::cout << "=== OCI 19c Raw API Test Application ===" << std::endl;

    // Creating an instance of the OciRawController
    OciRawController controller;

    // Database connection parameters
    // Users shall modify these values according to their environment
    const std::string username = "system";
    const std::string password = "oracle";
    const std::string dbname   = "192.168.150.101:1521/ws";

    // Connecting to the Oracle database
    if (!controller.connect(username, password, dbname)) {
        std::cerr << "[ERROR] Failed to connect to database" << std::endl;
        return 1;
    }

    // Testing the basic SELECT SYSDATE query
    controller.testSelectSysdate();

    // Testing data fetching from a table (uncomment to use)
    // controller.testFetchData("DUAL");

    // Executing a custom query (uncomment to use)
    // controller.executeQuery("SELECT USER FROM DUAL");

    // Disconnecting from the database (optional, destructor will handle it)
    controller.disconnect();

    std::cout << "Application terminated successfully." << std::endl;
    return 0;
}

#elif defined(_WIN32) || defined(_WIN64)

#include <iostream>

/**
 * The main entry point for Windows platform; the OCI functionality is not
 * supported on Windows in this implementation
 *
 * @param argc [int] The number of command-line arguments
 * @param argv [char**] The array of command-line arguments
 * @return [int] The exit status; 0x1 indicating the platform is not supported
 */
int main(int argc, char** argv) {
    std::cerr << "[ERROR] OCI Raw API is not supported on Windows in this implementation." << std::endl;
    std::cerr << "Please use Linux platform for OCI functionality." << std::endl;
    return 1;
}

#endif
