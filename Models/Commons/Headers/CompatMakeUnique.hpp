#pragma once
/** @file CompatMakeUnique.hpp
 * This common is for the old g++ and library version;
 * generally, the "make_unique" will be enable in g++ 4.9 with C++ 14;
 * for baseline of centos, the template shall be supported
 *
 * @author Nick, Liao
 * @date 2025/10/01
 */

#include <memory>
#include <utility>

#ifdef __linux__

// The macro variable is from the compiled process where is in the CMakeLists.txt file from the proper cpp file folder
// For more variable information, please refer to the file, namely .globalCmakeVariable.cmake
// When the OS_ID is not defined in the CMakeLists.txt file from the proper cpp file folder,
// the OS_ID will not be passed defined into compiled process
#ifdef OS_ID

// compat_make_unique.hpp
#pragma once
#include <memory>
#include <utility>

namespace std {

    // Case 1: General (non-array) types
    // Provides std::make_unique<T>(args...) which forwards arguments
    // to T's constructor and returns a std::unique_ptr<T>.
    template <typename T, typename... Args>
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    // Case 2: Dynamic arrays with unknown bound (e.g., make_unique<int[]>(n))
    // Allocates an array of n elements, value-initialized,
    // and returns std::unique_ptr<T> where T is an array type.
    template <typename T>
    typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0,
                            std::unique_ptr<T>>::type
    make_unique(std::size_t n) {
        using U = typename std::remove_extent<T>::type;
        return std::unique_ptr<T>(new U[n]());
    }

    // Case 3: Arrays with known bound (e.g., T[10])
    // Disabled: it is ill-formed to use make_unique with fixed-size arrays.
    template <typename T, typename... Args>
    typename std::enable_if<std::extent<T>::value != 0, void>::type
    make_unique(Args&&...) = delete;
}// end namespace std

#endif
#endif