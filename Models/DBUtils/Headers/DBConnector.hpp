#pragma once
/** @file DBConnector.hpp
 * The database connector class using ocilib library; this class will be implemented
 * in a singleton pattern for managing the Oracle database connection
 *
 * @author Nick, Liao
 * @date 2025/06/26
 *
 * @note The file is dependent to the third party software, ocilib where installed in Vendors folder
 */
#include <ocilib.h>
#include <string>

#include "../../Commons/Headers/POSIXErrors.hpp"

/**
 * The database connector class (singleton)
 */
class DBConnector {
   public:
    ~DBConnector();
    static DBConnector& GetInstance();
    static void Err_Handler(OCI_Error*);
    Commons::POSIXErrors Initialize();
    Commons::POSIXErrors SetSQLStatement(char*);
    void Execute(std::string, std::string);
    void Disconnect();
    boolean BindArraySetSize(unsigned int);
    boolean BindArrayOfStrings(const char*, char*, unsigned int, unsigned int);
    OCI_Statement* GetStatement();
    void Fetch(char*, std::string, std::string);

   private:
    // A static variable for the object from the class (singleton)
    static DBConnector* instance;
    // The OCI connection instance
    OCI_Connection* cn;
    // The OCI statement instance
    OCI_Statement* st;
    DBConnector();
};
