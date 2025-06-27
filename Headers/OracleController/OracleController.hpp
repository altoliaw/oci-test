#pragma once

#include "../../Models/OracleModel/Headers/OracleModel.hpp"

class OracleController{
    public:
        OracleController();
        ~OracleController();
        void run();
    private:
        OracleModel* model;
};
