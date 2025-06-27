#include "../../Headers/OracleController/OracleController.hpp"


OracleController::OracleController(){
    model = new OracleModel();
}

OracleController::~OracleController() {
}

void OracleController::run(){
    if(model->Initialize())
        if(model->Connection())
            model->SQLExecution();
    
    model->Cleanup();
}
