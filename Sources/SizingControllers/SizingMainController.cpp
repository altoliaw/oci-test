/**
 * @see SizingControllers.hpp
 */
#include "../../Headers/SizingControllers/SizingMainController.hpp"

namespace SizingControllers {

/**
 * The starting process, the entry of the process
 *
 * @param argC [int] The number of the argument
 * @param argV [char**] The array of the argument
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors SizingMainController::start(int argC, char** argV) {
    Commons::POSIXErrors result = Commons::POSIXErrors::OK;
    SizingMainControllerPrototype* caller;

#ifdef __linux__
// The macro variable is from the compiled process where is in the CMakeLists.txt file from the proper cpp file folder
// For more variable information, please refer to the file, namely .globalCmakeVariable.cmake
// When the OS_ID is not defined in the CMakeLists.txt file from the proper cpp file folder,
// the OS_ID will not be passed defined into compiled process
#ifndef OS_ID
    LinuxSizingMainController instance;
#else
    LinuxCentosSizingMainController instance;
#endif

    caller = &instance;
#elif defined(_WIN32)
    #ifdef OS_ID
        WindowsWinDivertSizingMainController instance;
        caller = &instance;
    #else // Default to NPCAP
        WindowsNpcapSizingMainController instance;
        caller = &instance;
    #endif
#endif
    
    result = caller->start(argC, argV);
    return result;
}
}  // namespace SizingMainController