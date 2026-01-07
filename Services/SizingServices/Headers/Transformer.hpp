#pragma once
/** @file Transformer.hpp
 * The data transformer for outputing with specified descriptor
 *
 * @author Nick, Liao
 * @date 2025/09/26
 */

#include <iostream>
#include <map>
#include <string>

#include "../../../Models/Commons/Headers/IOExporters.hpp"
#include "../../../Models/Commons/Headers/POSIXErrors.hpp"

namespace SizingServices {
/**
 * The data transformer for outputing with specified descriptor
 */
class Transformer {
   public:
    /**
     * The output content of the types for the outputs
     */
    enum LayoutType {
        DEFAULT = 0,  // Original content
        FLOWTYPE,     // User defined content
        OTHERS
    };

    /**
     * The enumeration for specifying which variable shall be chosen for the strcut, LayoutFormatAndString
     */
    enum LayoutFormatAndStringType {
        TITLE = 0,  // To specify the field, LayoutFormatAndString's title
        FORMAT,     // To specify the field, LayoutFormatAndString's format
        INFORMATION_END
    };
    /**
     * The output format rule
     */
    struct LayoutFormatAndString {
        std::string title;   // The title string for the output for the layout; for example, "category, size, ..."
        std::string format;  // The C-style formatted string for fprintf or printf function
    };

    static int newDescriptor;
    static int destinationDescriptor;
    static unsigned int defaultOutputLayoutType;
    static std::map<int, std::pair<int, LayoutFormatAndString>> layoutStringFormat;

    static Commons::POSIXErrors releaseDescriptors();
    static std::pair<Commons::POSIXErrors, long> printContent(const unsigned int, const unsigned int, const int, ...);

   protected:
    Transformer() = delete;
    ~Transformer();
    static std::pair<Commons::POSIXErrors, long> printContentExecution(const unsigned int, const unsigned int, const int, va_list);
};

}  // namespace SizingServices