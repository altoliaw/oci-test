#include "../../Headers/SizingControllers/SizingMainController.hpp"

int main(int argC, char** argV) {
    int result = Commons::POSIXErrors::OK;

    // Main process
    SizingControllers::SizingMainController caller;
    result = caller.start(argC, argV);

    return result;
}