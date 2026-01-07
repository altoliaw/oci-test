#pragma once
/** @file IOExporters.hpp
 * IO exporters; for
 *
 * @author Nick, Liao
 * @date 2025/09/17
 */

#include <stdio.h>
#include <unistd.h>  // For the POSIX API, such as dup and dup2

#include <cstdarg>
#include <functional>  // For the functional
#include <map>
#include <memory>
#include <string>

#include "./POSIXErrors.hpp"
#include "./CompatMakeUnique.hpp"

namespace Commons {
/**
 * The IO exporters
 */
class IOExporters {
   public:
    // The descriptor type in the system; when adding an element, the initialization in the same name .cpp file shall be modified, either (add/delete null).
    enum DescriptorType { STDIN,
                          STDOUT,
                          STDERR,
                          OTHERS };
    enum DescriptorBehavior { NONE,
                              SWAP };  // To determine whether two descriptors have been swapped
    // The structure for storing the descriptor information; for exapmle,
    // if users attempt to redirect the stdout to a file, the original descriptor is 1 (stdout),
    // and the redirected descriptor is the descriptor of the file that users want to redirect to
    struct DescriptorInformation {
       public:
        // The current descriptor; however, the device information in this descriptor may be swapped into the reservedDescriptorInformation but the value will not be changed
        int currentDescriptor;

        // The descriptor for reserved; the first is the new descriptor refers to the device from the descriptor, namely currentDescriptor,
        // and the second is for reserving the descriptor id which currentDescriptor wants to copy for
        std::pair<int, int> reservedDescriptorInformation;

        DescriptorInformation(int);
        ~DescriptorInformation();

       private:
    };

    // The array for descriptor pointers which are mapping to the enumeration; that implies that users can register the descriptor by using the enumeration
    static std::map<int, std::unique_ptr<DescriptorInformation>> descriptorSet;  // The descriptor set by using map, the first element implies the current descriptor

    static std::pair<Commons::POSIXErrors, long> printFromRegisteredDescriptor(const int, const unsigned char*, ...);
    static std::pair<Commons::POSIXErrors, long> printFromRegisteredDescriptor(const unsigned char*, ...);
    static std::pair<Commons::POSIXErrors, long> printFromRegisteredDescriptor(const int, const unsigned char*, va_list);
    static std::pair<Commons::POSIXErrors, long> printFromRegisteredDescriptor(const unsigned char*, va_list);
    
    static Commons::POSIXErrors changeDescriptor(const int, const int);
    static std::pair<Commons::POSIXErrors, IOExporters::DescriptorBehavior> recoverDescriptor(const int);
    static Commons::POSIXErrors releaseDescriptor(const int);
    static void resetDescriptorSet();

   private:
    IOExporters() = delete;
    ~IOExporters();
    static std::pair<Commons::POSIXErrors, long> printFromRegisteredDescriptorExecution(const int, const unsigned char*, va_list);
};

}  // namespace Commons
