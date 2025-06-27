#include "../../Headers/OciController/OcilibController.hpp"
#include <iostream>

int OCILibController::run() {
    OCILibModel model;
    if (!model.connect("192.168.30.178/orcl", "ann", "cobra8029")) return 1;
    if (!model.prepareStatement()) return 1;
    if (!model.bindAndExecute()) return 1;
    return 0;
}
