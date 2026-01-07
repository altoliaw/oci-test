/**
 * @see IOExporters.hpp
 */
#include "../Headers/IOExporters.hpp"

namespace Commons {
// The initialization of the static member defined in the class IOExporters, and
// creating objects of the descriptors, namely STDIN, STDOUT, and STDERR, there are the default descriptors
std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>> Commons::IOExporters::descriptorSet = [] {
    std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>> initialMap;
    initialMap.emplace(IOExporters::DescriptorType::STDIN, std::make_unique<IOExporters::DescriptorInformation>(IOExporters::DescriptorType::STDIN));
    initialMap.emplace(IOExporters::DescriptorType::STDOUT, std::make_unique<IOExporters::DescriptorInformation>(IOExporters::DescriptorType::STDOUT));
    initialMap.emplace(IOExporters::DescriptorType::STDERR, std::make_unique<IOExporters::DescriptorInformation>(IOExporters::DescriptorType::STDERR));
    return initialMap;
}();

/**
 * Constructor, DescriptorInformation
 *
 * @param currentDescriptor [IN, int] The descriptor itself
 */
IOExporters::DescriptorInformation::DescriptorInformation(int currentDescriptor) {
    this->currentDescriptor = currentDescriptor;
    reservedDescriptorInformation.first = -1;
    reservedDescriptorInformation.second = -1;
}

/**
 * Destructor, DescriptorInformation; this destructor is trigger by the life cycle of the static map, descriptorSet which contains unique_ptr elements;
 * that implies, when the execution ends, the elements of the map will be destructed each element which referred by an auto pointer automatically;
 * as a result, designers shall not release each element manually
 */
IOExporters::DescriptorInformation::~DescriptorInformation() {
    // Using the currentDescriptor as id to removing the DescriptorInformation instance
    std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>>::iterator it = descriptorSet.find(currentDescriptor);
    if (it != descriptorSet.end()) {  // Hitting the element in the map
        // Recovering the descriptor
        recoverDescriptor(currentDescriptor);
    }

    // Re-valuing the variables
    currentDescriptor = -1;
    reservedDescriptorInformation.first = -1;
    reservedDescriptorInformation.second = -1;
}

/**
 * Destructor, IOExporters
 */
IOExporters::~IOExporters() {
}

/**
 * Changing the new descriptor to the destination descriptor
 *
 * @param destinationDescriptor [IN, const int] The descriptor that users shall operate
 * @param newDescriptor [IN, const int] The descriptor that user wants to redirect to
 * @return Commons::POSIXErrors Please refer to the enumeration
 */
Commons::POSIXErrors IOExporters::changeDescriptor(const int destinationDescriptor, const int newDescriptor) {
    Commons::POSIXErrors isSuccess = Commons::POSIXErrors::E_IO;
    std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>>::iterator it = descriptorSet.end();   // The temporary for the .find(.) result or emplace result
    std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>>::iterator it2 = descriptorSet.end();  // The temporary for the .find(.) result or emplace result

    // Verifying if the destinationDescriptor and newDescriptor have exist in the map, if no, inserting these two into the map, respectively
    {
        std::pair<std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>>::iterator, bool> emplaceResult;
        if ((it = descriptorSet.find(destinationDescriptor)) == descriptorSet.end()) {  // No hitting
            // Inserting the object
            emplaceResult = descriptorSet.emplace(destinationDescriptor, std::unique_ptr<DescriptorInformation>(new DescriptorInformation(destinationDescriptor)));
            it = emplaceResult.first;
        }
        if ((it2 = descriptorSet.find(newDescriptor)) == descriptorSet.end()) {  // No hitting
            // Inserting the object
            emplaceResult = descriptorSet.emplace(newDescriptor, std::unique_ptr<DescriptorInformation>(new DescriptorInformation(newDescriptor)));
            it2 = emplaceResult.first;
        }
    }

    // Changing the two descriptors, ...
    {
        // First, if the destinationDescriptor has been swapped, the destinationDescriptor and newDescriptor shall be recovered.
        std::pair<Commons::POSIXErrors, Commons::IOExporters::DescriptorBehavior> returnedValue = IOExporters::recoverDescriptor(it->second->currentDescriptor);
        isSuccess = returnedValue.first;  // Passing the returned value (success flag) to the variable, isSuccess
        returnedValue = IOExporters::recoverDescriptor(it2->second->currentDescriptor);
        isSuccess = (returnedValue.first != Commons::POSIXErrors::OK) ? returnedValue.first : isSuccess;  // Combining the result above
        if (isSuccess == Commons::POSIXErrors::OK) {                                                      // Changing the two descriptors
            (it->second->reservedDescriptorInformation).first = dup(it->second->currentDescriptor);       // Copying to the reserved descriptor
            dup2(it2->second->currentDescriptor, it->second->currentDescriptor);                          // Copying the device from the newDescriptor to destinationDescriptor
            // Commons::IOExporters::STDOUT
            (it->second->reservedDescriptorInformation).second = it2->second->currentDescriptor;  // Passing referred descriptor id for reservation
        }
    }

    return isSuccess;
}

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor
 *
 * @param descriptor [IN, const int, DescriptorType::STDOUT] The descriptor where users specify
 * @param stringFormat [IN, const unsigned char*] The format of the string modelled in C-style, such as "This is %d %s\n"
 * @param ... [IN] The arguments for the parameter, stringFormat
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> IOExporters::printFromRegisteredDescriptor(const int descriptor, const unsigned char* stringFormat, ...) {
    std::pair<Commons::POSIXErrors, long> returnedValue;

    va_list arguments;
    va_start(arguments, stringFormat);  // Stating from the location from the variable, descriptor defined in the parameter list
    returnedValue = IOExporters::printFromRegisteredDescriptorExecution(descriptor, stringFormat, arguments);
    va_end(arguments);  // The obtained arguments' process end
    return returnedValue;
}

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor (overloading)
 *
 * @param stringFormat [IN, const unsigned char*] The format of the string modelled in C-style, such as "This is %d %s\n"
 * @param ... [IN] The arguments for the parameter, stringFormat
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> IOExporters::printFromRegisteredDescriptor(const unsigned char* stringFormat, ...) {
    std::pair<Commons::POSIXErrors, long> returnedValue;

    va_list arguments;
    va_start(arguments, stringFormat);  // Stating from the location from the variable, descriptor defined in the parameter list
    returnedValue = IOExporters::printFromRegisteredDescriptorExecution(IOExporters::DescriptorType::STDOUT, stringFormat, arguments);
    va_end(arguments);  // The obtained arguments' process end
    return returnedValue;
}

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor
 *
 * @param descriptor [IN, const int, DescriptorType::STDOUT] The descriptor where users specify
 * @param stringFormat [IN, const unsigned char*] The format of the string modelled in C-style, such as "This is %d %s\n"
 * @param arguments [IN, va_list] The arguments for the parameter, stringFormat
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> IOExporters::printFromRegisteredDescriptor(const int descriptor, const unsigned char* stringFormat, va_list arguments) {
    std::pair<Commons::POSIXErrors, long> returnedValue;

    returnedValue = IOExporters::printFromRegisteredDescriptorExecution(descriptor, stringFormat, arguments);
    return returnedValue;
}

/**
 * Displaying the string with the specified string format and the arguments by using the specified descriptor (overloading)
 *
 * @param stringFormat [IN, const unsigned char*] The format of the string modelled in C-style, such as "This is %d %s\n"
 * @param arguments [IN, va_list] The arguments for the parameter, stringFormat
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> IOExporters::printFromRegisteredDescriptor(const unsigned char* stringFormat, va_list arguments) {
    std::pair<Commons::POSIXErrors, long> returnedValue;
    returnedValue = IOExporters::printFromRegisteredDescriptorExecution(IOExporters::DescriptorType::STDOUT, stringFormat, arguments);
    return returnedValue;
}

/**
 * Recovering the specified descriptor
 *
 * @param descriptorNumber [IN, const int] The specfied descriptor
 * @return std::pair<Commons::POSIXErrors, IOExporters::DescriptorBehavior> The pair contains successful information and the swapped information
 * - The first one is the flag of success; 0: failed; 1: success
 * - The second one is the swapped flag: "NONE" shows no swap occur; "SWAP" shows the swap occurs
 */
std::pair<Commons::POSIXErrors, IOExporters::DescriptorBehavior> IOExporters::recoverDescriptor(const int descriptorNumber) {
    Commons::POSIXErrors isSuccess = Commons::POSIXErrors::OK;
    IOExporters::DescriptorBehavior behavior = IOExporters::DescriptorBehavior::NONE;

    // Verifying if the descriptor exists
    std::map<int, std::unique_ptr<DescriptorInformation>>::iterator it = descriptorSet.find(descriptorNumber);
    if (it == descriptorSet.end()) {  // No hitting
        isSuccess = Commons::POSIXErrors::E_AGAIN;
    } else {
        // When the reserved device does not belong to -1 (a device has been linked. i.e., the swapped has been occurs)
        if ((descriptorSet[descriptorNumber]->reservedDescriptorInformation).first != -1) {
            // Swapping the past device which is referred by the reserved descriptor to the current descriptor
            dup2((descriptorSet[descriptorNumber]->reservedDescriptorInformation).first, descriptorSet[descriptorNumber]->currentDescriptor);

            // Releasing the reserved descriptor except STDIN, STDOUT, and STDERR (descriptor number always > 0 when success occurs)
            if ((descriptorSet[descriptorNumber]->reservedDescriptorInformation).first >= DescriptorType::OTHERS) {
                close((descriptorSet[descriptorNumber]->reservedDescriptorInformation).first);  // Removing the descriptor duplicating by users
            }
            (descriptorSet[descriptorNumber]->reservedDescriptorInformation).first = -1;   // There is no descriptor for reservation.
            (descriptorSet[descriptorNumber]->reservedDescriptorInformation).second = -1;  // No descriptor number is necessary
            behavior = IOExporters::DescriptorBehavior::SWAP;
        }
    }
    return {isSuccess, behavior};
}

/**
 * Removing the descriptor and destructing the descriptor from the map
 *
 * @param descriptorNumber [IN, const int] The descriptor that user requested for removal
 * @return Commons::POSIXErrors [OUT, Commons::POSIXErrors] The flag to determine if the function has been success elegantly;
 * when success, the returned value is equal to "Commons::POSIXErrors::OK";
 * otherwise, the values in the "Commons::POSIXErrors" except "Commons::POSIXErrors::OK" will be occurred
 */
Commons::POSIXErrors IOExporters::releaseDescriptor(const int descriptorNumber) {
    Commons::POSIXErrors isSuccess = Commons::POSIXErrors::OK;

    // Using the currentDescriptor as id to removing the DescriptorInformation instance
    std::map<int, std::unique_ptr<IOExporters::DescriptorInformation>>::iterator it = descriptorSet.find(descriptorNumber);
    if (it == descriptorSet.end()) {  // No hitting the element in the map
        // Do Nothing (i.e., the element does not exist.)
        isSuccess = Commons::POSIXErrors::E_BADF;
    } else {  // Hitting, executing the reset() to destruct the object
        // Releasing the descriptor except  STDIN, STDOUT, and STDERR
        if (it->first < DescriptorType::STDIN || it->first >= DescriptorType::OTHERS) {
            // Recovering the descriptor
            std::pair<Commons::POSIXErrors, Commons::IOExporters::DescriptorBehavior> isSuccessResult;
            isSuccessResult = IOExporters::recoverDescriptor(descriptorNumber);
            if (isSuccessResult.first == Commons::POSIXErrors::OK) {
                // Removing the element in the map; this will call the destruct of the object (DescriptorInformation)
                descriptorSet.erase(descriptorNumber);
            } else {  // Recovering error
                isSuccess = Commons::POSIXErrors::E_BADF;
            }
        } else {  // When the descriptor belongs to any one of STDIN, STDOUT, and STDERR; this will not remove by users manually
            isSuccess = Commons::POSIXErrors::E_BADF;
        }
    }
    return isSuccess;
}

/**
 * This function is for unit test for recovering the original static map
 */
void IOExporters::resetDescriptorSet() {
    descriptorSet.clear();
    descriptorSet.emplace(DescriptorType::STDIN, std::make_unique<DescriptorInformation>(DescriptorType::STDIN));
    descriptorSet.emplace(DescriptorType::STDOUT, std::make_unique<DescriptorInformation>(DescriptorType::STDOUT));
    descriptorSet.emplace(DescriptorType::STDERR, std::make_unique<DescriptorInformation>(DescriptorType::STDERR));
}

/**
 * The implement of "printFromRegisteredDescriptor" function series overloading
 *
 * @param descriptor [IN, const int, DescriptorType::STDOUT] The descriptor where users specify
 * @param stringFormat [IN, const unsigned char*] The format of the string modelled in C-style, such as "This is %d %s\n"
 * @param arguments [IN, va_list] The arguments for the parameter, stringFormat
 * @return [OUT std::pair<Commons::POSIXErrors, long> ] Two types of result will be returned; the first one is the flag (defined in Commons::POSIXErrors) of the function;
 * the second one is the length of the processed string
 */
std::pair<Commons::POSIXErrors, long> IOExporters::printFromRegisteredDescriptorExecution(const int descriptor, const unsigned char* stringFormat, va_list arguments) {
    Commons::POSIXErrors isSuccess = Commons::POSIXErrors::OK;
    long templength = 0;  // A temporary for reserving the processed length
    long Length = 0;      // The final length for returning

    // Preparing a copy for measuring the length
    {
        va_list argumentsCopy;              // For calculating the total length of the final string
        va_copy(argumentsCopy, arguments);  // Copying arguments to argumentsCopy
        templength = vsnprintf(nullptr, 0, (const char*)stringFormat, argumentsCopy);
        va_end(argumentsCopy);
    }
    // Generating the string with the specified variable, stringFormat
    {
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(templength + 1);
        vsnprintf(buffer.get(), templength + 1, (const char*)stringFormat, arguments);
        // When the descriptor is illegal, the IOExporters::DescriptorType::STDOUT will be the default
        Length = write((descriptor < 0) ? IOExporters::DescriptorType::STDOUT : descriptor,
                       buffer.get(),
                       templength);
        if (Length == -1) {  // vsnprintf error
            isSuccess = Commons::POSIXErrors::E_IO;
        }
        buffer.reset();  // Releasing the storage
    }
    return {isSuccess, Length};
}

}  // namespace Commons
