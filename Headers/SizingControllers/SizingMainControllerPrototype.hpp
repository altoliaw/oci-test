#pragma once
/** @file SizingMainCallerPrototype.hpp
 * The prototypes of SizingMainCaller
 *
 * @author Nick, Liao
 * @date 2024/08/15
 */

#include <vector>
#include "../../Models/Commons/Headers/POSIXErrors.hpp"

namespace SizingControllers {
struct unitService {
   public:
    char interfaceName[256];
    std::vector<int> port;
    unitService();
    ~unitService();
};

class SizingMainControllerPrototype {
   public:
    virtual Commons::POSIXErrors start(int, char**) = 0;
};
}  // namespace SizingMainCaller