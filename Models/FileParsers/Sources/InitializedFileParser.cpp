/**
 * @see InitializedFileParser.hpp
 */
#include "../Headers/InitializedFileParser.hpp"

namespace FileParsers {
// Initialization as nullptr to the static variable defined as the unique pointer in the class
std::unique_ptr<InitializedFileParser> InitializedFileParser::initializedFileParserPointer = nullptr;

/**
 * Constructor
 */
InitializedFileParser::InitializedFileParser() {
    initializedTable = nullptr;
    initializedTable = new Commons::HashTable();
}

/**
 * Destructor
 */
InitializedFileParser::~InitializedFileParser() {
    if (initializedTable != nullptr) {
        delete initializedTable;
        initializedTable = nullptr;
    }
}

/**
 * The static method for initializing the singleton
 *
 * @return [std::unique_ptr<InitializedFileParser>&] The reference to the "InitializedFileParser" instance
 */
std::unique_ptr<InitializedFileParser>& InitializedFileParser::getInitializedFileParserInitialization() {
    // If the static pointer is nullptr, the pointer shall be referred to a static object defined in the function
    if (InitializedFileParser::initializedFileParserPointer == nullptr) {
        InitializedFileParser::initializedFileParserPointer.reset(new InitializedFileParser());
    }

    return InitializedFileParser::initializedFileParserPointer;
}

/**
 * The caller for releasing the memory of the singleton because when the singleton will be recreated in 
 * the unit tests
 * 
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors InitializedFileParser::releaseInitializedFileParserInitialization() {
    // If the static pointer is not nullptr, the pointer shall be referred to a static object defined in the function
    if (InitializedFileParser::initializedFileParserPointer != nullptr) {
        InitializedFileParser::initializedFileParserPointer.reset();  // Destroying the instance/object; the destroyed process will executed the
                                                                      // destructor of the instance/object
        InitializedFileParser::initializedFileParserPointer = nullptr;
    }
    return Commons::POSIXErrors::OK;
}

/**
 * Obtaining the setting contents from the source path
 *
 * @param sourcePath [const unsigned char*] The absolute path of the source path
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors InitializedFileParser::parseInitializedFile(const unsigned char* sourcePath) {
    // Creating the singleton by reference automatically, the function, getInitializedFileParserInitialization, will be
    // done once, even though the function, getInitializedFileParserInitialization(.) has been called many times
    std::unique_ptr<InitializedFileParser>& initialedFileParserInstance = InitializedFileParser::getInitializedFileParserInitialization();

    // Opening the specified file
    FILE* descriptor = fopen((const char*)sourcePath, "r+");
    if (descriptor == nullptr) {
        std::cerr << "No file exists\n";
        return Commons::POSIXErrors::E_EXIST;
    }

    // Section & Key, and value parts
    unsigned char section[512] = {'\0'};
    unsigned char key[1024] = {'\0'};
    unsigned char value[2048] = {'\0'};
    char* linePointer = (char*)value;
    size_t lineBufferLength = sizeof(value) / sizeof(unsigned char);

    while (fgets(linePointer, lineBufferLength, descriptor) != nullptr) {
        unsigned int length = strlen(linePointer);
        // Trimming the spaces
        length = (length > 0) ? Commons::StringImplement::trimLeftSpace((unsigned char**)(&linePointer)) : 0;
        length = (length > 0) ? Commons::StringImplement::trimRightSpace((unsigned char**)(&linePointer)) : 0;
        length = (length > 0) ? Commons::StringImplement::obtainNoNewLineSignAtTheEnd((unsigned char**)(&linePointer)) : 0;

        if (length <= 0) {  // Empty line
            continue;
        } else if (length > 0 && linePointer[0] == '#') {  // Commented line
            continue;
        } else if (length > 0 && linePointer[0] == '[') {  // Section part
            memcpy(section, (unsigned char*)(linePointer + 1), length - 1);
            // Verifying if the last character is ']'
            if (section[length - 1 - 1] == ']') {
                section[length - 1 - 1] = '\0';
            } else {
                section[length - 1] = '\0';
            }
        } else {  // Key-Value part
            int sectionLength = (int)strlen((char*)section);
            if (sectionLength <= 0) {
                std::cerr << "The key-value pair has not been denoted in the section phase\n";
                return Commons::POSIXErrors::E_NOITEM;
            }

            // Accessing the key value pairs; spliting the key and value by using the character '='
            int delimiterIndex = -1;
            for (size_t i = 0; i < length; i++) {
                if (linePointer[i] == '=') {
                    delimiterIndex = i;
                    break;
                }
            }

            // The delimiter does not exist in the sentence.
            if (delimiterIndex == -1) {
                std::cerr << "The delimiter does not exist in the sentence\n";
                return Commons::POSIXErrors::E_NOITEM;
            }

            // Key preparing; the key will be denoted as the syntax- "$sectionName.$key";
            // for example, given the section name is "compile" and the key name is "base", the final key will be
            // equal to "compile.base"
            memcpy(key, section, sectionLength);
            key[sectionLength] = '.';
            memcpy(key + sectionLength + 1, (unsigned char*)linePointer, delimiterIndex);  // The last argument is from "delimiterIndex - 1 - 0 + 1"
            key[sectionLength + 1 + delimiterIndex] = '\0';

            // Value preparing (adjusting the value array)
            memmove(value, value + delimiterIndex + 1, length - (delimiterIndex + 1));
            value[length - (delimiterIndex + 1)] = '\0';

            // Verifying the instance
            if (initialedFileParserInstance == nullptr) {
                std::cerr << "No InitialedFileParser instance\n";
            }
            // Putting the K-V pair into the hash table; because the value belongs to the string,
            // the size shall be included the size of '\0'
            initialedFileParserInstance->initializedTable->addElementIntoHashTable(
                (char*)key, (void*)value, (size_t)(length - (delimiterIndex + 1) + 1), Commons::HashTable::ElementType::unsignedCharStarType);
        }
    }

    // Closing the descriptor
    if (descriptor != nullptr) {
        fclose(descriptor);
    }
    return Commons::POSIXErrors::OK;
}

/**
 * Obtaining the value from the hash table
 *
 * @param columnName [const unsigned char*] The key of the element in the hash table
 * @param value [unsigned char*] The pointer to the value in the hash table; the value is searched from the columnName;
 * In addition, the value shall be assigned a static memory space
 * @return [Commons::POSIXErrors] The successful flag
 */
Commons::POSIXErrors InitializedFileParser::getValueFromFileParser(const unsigned char* columnName, unsigned char* value) {
    // Creating the singleton by reference automatically, the function, getInitializedFileParserInitialization, will be
    // done once, even though the function, getInitializedFileParserInitialization(.) has been called many times
    std::unique_ptr<InitializedFileParser>& initialedFileParserInstance = InitializedFileParser::getInitializedFileParserInitialization();

    unsigned char* copiedColumnNameAddress = nullptr;                            // The pointer for referring to the columnName defined in the hash table
    void* valuePointer = nullptr;                                                // The pointer for referring to the value which is searching by using the columnName
    size_t valueSize = 0;                                                        // The memory size of the value
    Commons::HashTable::ElementType type = Commons::HashTable::ElementType::unsignedCharStarType;  // The data type of the value

    // Obtaining the value
    char isExisted = initialedFileParserInstance->initializedTable->getValueByName(
        (char*)columnName, (char**)(&copiedColumnNameAddress), &valuePointer, &valueSize, &type);

    // When the key does not exist in the hash table, ...
    if (isExisted == 0x0) {
        // std::cerr << "There is no item in the hash table.\n";
        return Commons::POSIXErrors::E_NOITEM;
    }

    // When the type is equal to the "unsigned char*"
    if (type == Commons::HashTable::ElementType::unsignedCharStarType) {
        memcpy((void*)value, valuePointer, valueSize);
        value[valueSize] = '\0';  // For ensuring that the '\0' will be appeared
    } else {
        // Do nothing
        return Commons::POSIXErrors::E_NOITEM;
    }

    return Commons::POSIXErrors::OK;
}

}  // namespace Commons