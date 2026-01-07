#pragma once
/** @file SizingMainCaller.hpp
 * The headers and global variables from other package
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */

#include <algorithm>
#include <iostream>

#include "../../Models/Commons/Headers/POSIXErrors.hpp"
#include "./SizingMainControllerPrototype.hpp"

#ifdef __linux__  // Linux

// The macro variable is from the compiled process where is in the CMakeLists.txt file from the proper cpp file folder
// For more variable information, please refer to the file, namely .globalCmakeVariable.cmake
// When the OS_ID is not defined in the CMakeLists.txt file from the proper cpp file folder,
// the OS_ID will not be passed defined into compiled process
#ifndef OS_ID

#include "./LinuxSizingMainController.hpp"

#else
#include "./LinuxCentosSizingMainController.hpp"
#endif

#elif defined(_WIN32) || defined(_WIN64)

#ifdef OS_ID
#include "./WindowsWinDivertSizingMainController.hpp"
#else
#include "./WindowsNpcapSizingMainController.hpp"
#endif

#endif

namespace SizingControllers {
class SizingMainController {
   public:
    Commons::POSIXErrors start(int, char**);
};
}  // namespace SizingMainController