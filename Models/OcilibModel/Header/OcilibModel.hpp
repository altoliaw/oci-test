#pragma once
#include <vector>
#include <string>
#include <ocilib.h>

class OCILibModel {
public:
    OCILibModel();
    ~OCILibModel();

    bool connect(const std::string& db, const std::string& user, const std::string& pwd);
    bool prepareStatement();
    bool bindAndExecute();
    void cleanup();

private:
    OCI_Connection* conn;
    OCI_Statement* stmt;
    std::vector<const char*> fakeData;
};