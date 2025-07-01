#pragma once
/**
 * @file DBConnector.hpp
 * DBConnector class using oci library.
 * @date 2025-06-26
 */
#include <ocilib.h>

class DBConnector {
public:
    ~DBConnector();
    static DBConnector& GetInstance();
    void static Err_Handler(OCI_Error* err);
    int Initialize();
    int SetSQLStatement(char* sql);
    void Execute();
    void Disconnect();
    boolean BindArraySetSize(unsigned int size);
    // OuOp
    boolean BindArrayOfStrings(const char* name, char* data, unsigned int len, unsigned int nb_elem);
    OCI_Statement* GetStatement();
private:
    static DBConnector* instance;
    OCI_Connection* cn;
    OCI_Statement* st;
    DBConnector();
};