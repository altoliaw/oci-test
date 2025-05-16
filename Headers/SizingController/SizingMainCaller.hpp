#pragma once
/** @file SizingMainCaller.hpp
 * The headers and global variables from other package
 *
 * @author Nick, Liao
 * @date 2024/05/15
 */

#include <algorithm>

#include "../../Models/Commons/Headers/POSIXErrors.hpp"
#include "./SizingMainCallerPrototype.hpp"

#ifdef __linux__  // Linux

#include "./LinuxSizingMainCaller.hpp"

#elif defined(_WIN32) || defined(_WIN64)

#include "./WindowsSizingMainCaller.hpp"

#endif

namespace SizingMainCaller {
class SizingMainCaller {
   public:
    Commons::POSIXErrors start(int, char**);
};
}  // namespace SizingMainCaller