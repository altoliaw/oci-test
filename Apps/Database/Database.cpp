#include "../../Headers/Controllers/DatabaseController.hpp"
#include "../../Models/Commons/Headers/POSIXErrors.hpp"

int main(int argc, char** argv) {
    Controllers::DatabaseController controller;
    controller.runBatchInsert();
    return 0;
}