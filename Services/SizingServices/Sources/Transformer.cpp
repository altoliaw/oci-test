/**
 * @see Transformer.hpp
 */
#include "../Headers/Transformer.hpp"

namespace SizingServices {

/**
 * The buffer for the default descriptor;
 * the initialization of the static descriptor; this descriptor is to be the new descriptor in the "Commons::POSIXErrors IOExporters::changeDescriptor"
 */
int Transformer::newDescriptor = Commons::IOExporters::DescriptorType::STDOUT;

/**
 * The buffer for the default descriptor;
 * the initialization of the static descriptor; this descriptor is to be the destination descriptor in the "Commons::POSIXErrors IOExporters::changeDescriptor"
 */
int Transformer::destinationDescriptor = Commons::IOExporters::DescriptorType::STDOUT;

/**
 * The buffer for the default output layout type from the enumeration, "LayoutType";
 * the initialization of the static output layout format
 */
unsigned int Transformer::defaultOutputLayoutType = Transformer::LayoutType::DEFAULT;

/**
 * The layout of the formatted string; the first element is a key in the map and the value is from the enumeration "LayoutType";
 * the second element is a pair stuff; the first one is the number of the parameter displayed in the variable, format, in the second one;
 * the second one is the formatted rule object; the initialization of the layout of the number of the parameters and the formatted string,
 * this map will be added when the element of the "enum LayoutType" has been inserted.
 */
std::map<int, std::pair<int, Transformer::LayoutFormatAndString>> Transformer::layoutStringFormat = [] {
    std::map<int, std::pair<int, Transformer::LayoutFormatAndString>> initialMap;  // The initialization of the string map
    initialMap.emplace(Transformer::DEFAULT,
                       std::make_pair<int, Transformer::LayoutFormatAndString>(11,
                                                                               {"UTC\tType\tInterface\tPort\tNumber(amount)\tSize(bytes)\tMaxSize(bytes)\t"
                                                                                "SQL number in the time interval\tSQL size(bytes) in the time interval\tAverage SQL number per sec(eps)\tPeak SQL number per sec(eps)\n",
                                                                                "%lu\t%s\t%s\t%d\t%lu\t%llu\t%lu\t%lu\t%llu\t%llu\t%llu\n"}));
    initialMap.emplace(Transformer::FLOWTYPE,
                       std::make_pair<int, Transformer::LayoutFormatAndString>(9,
                                                                               {"UTC\tType\tInterface\tPort\tInterval\tAverage Size(Mbps)\tMaxSize(Mbps)\t"
                                                                                "Average SQL number per sec(eps)\tPeak SQL number per sec(eps)\n",
                                                                                "%lu\t%s\t%s\t%d\t%d\t%lu\t%lu\t%llu\t%llu\n"}));
    return initialMap;
}();

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor
 *
 * @param layoutFormatAndStringType [IN, const unsigned int] To specify the type for outputing the title or format from the structure, LayoutFormatAndString
 * @param argumentsNumber [IN, const unsigned int] The number of the arguments "..."
 * @param descriptor [IN, const int, DescriptorType::STDOUT] The descriptor that users want to redirect to
 * @param ... [IN] The arguments for the parameter, for the C-style formatted string
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> Transformer::printContent(const unsigned int layoutFormatAndStringType, const unsigned int argumentsNumber, const int descriptor, ...) {
    std::pair<Commons::POSIXErrors, long> returnedValue = {Commons::POSIXErrors::OK, -1};  // The returned value
    va_list arguments;
    va_start(arguments, descriptor);  // Stating from the location from the variable, descriptor defined in the parameter list
    returnedValue = Transformer::printContentExecution(layoutFormatAndStringType, argumentsNumber, descriptor, arguments);
    va_end(arguments);  // The obtained arguments' process end
    return returnedValue;
}

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor;
 * the implement of the overload function, printContent
 *
 * @param layoutFormatAndStringType [IN, const unsigned int] To specify the type for outputing the title or format from the structure, LayoutFormatAndString
 * @param argumentsNumber [IN, const unsigned int] The number of the arguments "..."
 * @param descriptor [IN, const int, DescriptorType::STDOUT] The descriptor that users want to redirect to
 * @param ... [IN] The arguments for the parameter, for the C-style formatted string
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> Transformer::printContentExecution(const unsigned int layoutFormatAndStringType, const unsigned int argumentsNumber, const int descriptor, va_list arguments) {
    std::pair<Commons::POSIXErrors, long> returnedValue = {Commons::POSIXErrors::OK, -1};

    // To ensure if the descriptor has been change, that implies, the device of the default descriptor has been changed to the one of the parameter's descriptor:
    // The first one records the flag which reserves if the destination flag has been changed (false: no change; true: changed)
    // The second one displays the previous descriptor (newDescriptor).
    static std::pair<bool, int> pastHistory = std::make_pair<bool, int>(false, (int)descriptor);
    if (pastHistory.first == false) {  // The change has not occur
        // Changing the device of the newDescriptor to the specified descriptor (from parameter)
        if (Transformer::destinationDescriptor != descriptor) {                                      // When the two descriptors are not equal, ...
            Commons::IOExporters::changeDescriptor(Transformer::destinationDescriptor, descriptor);  // The change will occur
        }
        pastHistory.first = true;
        pastHistory.second = descriptor;
    } else {
        if (pastHistory.second == (int)descriptor) {  // When the past descriptor is equal to the previous called one
            // Doing nothing
        } else {  // Releasing the previous one from the map
            // Releasing the descriptor from the map
            Commons::IOExporters::releaseDescriptor(pastHistory.second);
            // Recovering the destination descriptor
            Commons::IOExporters::recoverDescriptor(Transformer::destinationDescriptor);
            // Changing the new one if the new descriptor is not equal to the destination descriptor
            if (Transformer::destinationDescriptor != descriptor) {
                Commons::IOExporters::changeDescriptor(Transformer::destinationDescriptor, descriptor);
            }
            pastHistory.second = descriptor;
        }
    }

    // Reserving the information of the descriptor from the parameter
    if (Transformer::newDescriptor != (int)descriptor) {
        Transformer::newDescriptor = (int)descriptor;
    }

    // Obtaining the element defined in the map by using the (static) defaultOutputLayoutType
    std::map<int, std::pair<int, Transformer::LayoutFormatAndString>>::iterator it = layoutStringFormat.find((int)Transformer::defaultOutputLayoutType);
    if (it == layoutStringFormat.end()) {  // No element exists.
        returnedValue = {Commons::POSIXErrors::E_AGAIN, -1};
    } else {  // Hitting
        // Verifying if the number of the arguments is equal to the one from the parameter, argumentsNumber
        if ((it->second).first != (int)argumentsNumber) {
            returnedValue = {Commons::POSIXErrors::E_AGAIN, -1};
        } else {  // The two values are equal and the IO process can be executed.
            returnedValue = Commons::IOExporters::printFromRegisteredDescriptor(Transformer::destinationDescriptor,
                                                                                ((layoutFormatAndStringType == Transformer::LayoutFormatAndStringType::TITLE) ? (const unsigned char*)(((it->second).second.title).c_str()) : (const unsigned char*)(((it->second).second.format).c_str())),
                                                                                arguments);
        }
    }
    return returnedValue;
}

/**
 * Releasing/Recovery the descriptor
 *
 * @return [OUT, Commons::POSIXErrors]  Please refer to the enumeration
 */
Commons::POSIXErrors Transformer::releaseDescriptors() {
    Commons::POSIXErrors isSuccess = Commons::POSIXErrors::OK;
    Commons::IOExporters::releaseDescriptor(Transformer::newDescriptor);
    Commons::IOExporters::releaseDescriptor(Transformer::destinationDescriptor);

    return isSuccess;
}

}  // namespace SizingServices