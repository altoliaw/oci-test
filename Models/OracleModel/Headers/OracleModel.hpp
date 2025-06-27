#pragma once
#include <ocilib.h>



class OracleModel {
    public:
        bool Initialize();
        bool Connection();
        bool SQLExecution();
        bool PrepareStatement(const char* sql);
        void bindData();
        bool Cleanup();
        static void err_handler(OCI_Error *err);

    private:
        OCI_Connection *cn = nullptr;
        OCI_Statement *st = nullptr;
        bool success = false;
};

