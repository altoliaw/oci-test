/**
 * @see InitializedJsonFileParser.hpp
 */
#include "../Headers/InitializedJsonFileParser.hpp"

namespace FileParsers {
// Initialization as nullptr to the static variable defined as the unique pointer in the class
std::unique_ptr<InitializedJsonFileParser> InitializedJsonFileParser::initializedFileParserPointer = nullptr;

/**
 * Constructor
 */
InitializedJsonFileParser::InitializedJsonFileParser() {
    jsonParsedContent = nullptr;
}

/**
 * Destructor
 */
InitializedJsonFileParser::~InitializedJsonFileParser() {
    if (jsonParsedContent != nullptr) {
        // Releasing the memory by using the cJson function
        cJSON_Delete(jsonParsedContent);
        jsonParsedContent = nullptr;
    }
}

/**
 * The static method for initializing the singleton
 *
 * @return [std::unique_ptr<InitializedJsonFileParser>&] The reference to the "InitializedFileParser" instance
 */
std::unique_ptr<InitializedJsonFileParser>& InitializedJsonFileParser::getInitializedFileParserInitialization() {
    // If the static pointer is nullptr, the pointer shall be referred to a static object defined in the function
    if (InitializedJsonFileParser::initializedFileParserPointer == nullptr) {
        InitializedJsonFileParser::initializedFileParserPointer.reset(new InitializedJsonFileParser());
    }

    return InitializedJsonFileParser::initializedFileParserPointer;
}

/**
 * The caller for releasing the memory of the singleton because when the singleton will be recreated in
 * the unit tests
 *
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors" The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors InitializedJsonFileParser::releaseInitializedFileParserInitialization() {
    // If the static pointer is not nullptr, the pointer shall be referred to a static object defined in the function
    if (InitializedJsonFileParser::initializedFileParserPointer != nullptr) {
        InitializedJsonFileParser::initializedFileParserPointer.reset();  // Destroying the instance/object; the destroyed process will executed the
                                                                          // destructor of the instance/object
        InitializedJsonFileParser::initializedFileParserPointer = nullptr;
    }
    return Commons::POSIXErrors::OK;
}

/**
 * Obtaining the setting contents from the source path; this function may be the bottleneck depending on
 * the complexity of the .json structure because the recursions are used in the method
 *
 * @param sourcePath [const unsigned char*] The absolute path of the source path
 * @return [Commons::POSIXErrors] The status defined in the class "POSIXErrors"
 */
Commons::POSIXErrors InitializedJsonFileParser::parseInitializedFile(const unsigned char* sourcePath) {
    // Creating the singleton by reference automatically, the function, getInitializedFileParserInitialization, will be
    // done once, even though the function, getInitializedFileParserInitialization(.) has been called many times
    std::unique_ptr<InitializedJsonFileParser>& initialedFileParserInstance = InitializedJsonFileParser::getInitializedFileParserInitialization();

    // Opening the specified file
    FILE* descriptor = fopen((const char*)sourcePath, "r+");
    if (descriptor == nullptr) {
        std::cerr << "No file exists\n";
        return Commons::POSIXErrors::E_EXIST;
    }

    // Calculating the size of the contents of the .json file
    fseek(descriptor, 0, SEEK_END);
    unsigned long length = ftell(descriptor);
    fseek(descriptor, 0, SEEK_SET);  // Resetting the file descriptor to the starting position

    // Dynamic memory allocation (array) by using the unique pointer and reading the .json content into the array
    std::unique_ptr<unsigned char[]> jsonContent = nullptr;
    jsonContent.reset(new unsigned char[length + 1]);
    unsigned int readLength = fread(jsonContent.get(), 1, length, descriptor);
    
    // If the length from the fread function is not equal to the one from ftell function in the linux or
    // the length from the fread function is not equal to and less than the one from ftell function in the windows, ...
    // The length from the fread function and the length from the ftell function are different in windows because
    // in this case, the author used fopen(.,"r"). If the author used only mode "r", the file will open in Unix format.
    // To see more information, please refer to the URL: https://stackoverflow.com/questions/23690436/fread-dropping-carriage-returns-in-c 
    if (readLength > (unsigned int)length) {
        if (initialedFileParserInstance->jsonParsedContent != nullptr) {
            std::cerr << "Releasing the memory of the cJson object\n";
            cJSON_Delete(initialedFileParserInstance->jsonParsedContent);
            initialedFileParserInstance->jsonParsedContent = nullptr;
        }
        jsonContent.reset(nullptr);
        // Closing the descriptor
        if (descriptor != nullptr) {
            fclose(descriptor);
        }
        return Commons::POSIXErrors::E_EXIST;
    }
    jsonContent[readLength] = '\0';

    // Closing the descriptor
    if (descriptor != nullptr) {
        fclose(descriptor);
    }

    // Parsing .json file recursively
    // When the object is not null, the object shall be destructed by cJson and the pointer shall refer to the nullptr.
    if (initialedFileParserInstance->jsonParsedContent != nullptr) {
        std::cerr << "Releasing the memory of the cJson object\n";
        cJSON_Delete(initialedFileParserInstance->jsonParsedContent);
        initialedFileParserInstance->jsonParsedContent = nullptr;
    }

    // Json content parsing
    initialedFileParserInstance->jsonParsedContent = cJSON_Parse((char*)(jsonContent.get()));
    if (initialedFileParserInstance->jsonParsedContent == nullptr) {  // JSON syntax is error.
        jsonContent.reset(nullptr);
        std::cerr << "JSON syntax is error\n";
        return Commons::POSIXErrors::E_EXIST;
    }

    return Commons::POSIXErrors::OK;
}

/**
 * Obtaining the value from the cJson parsed object
 *
 * @param columnName [const unsigned char*] The key of the element in the .json file; generally, the value which users search can
 * be denoted as the combination of "object keys" with delimiters, "." such as "key1.key2.key3[i]" where key3 is an array;
 * however, when the content of the key contains dot, the double quotes shall be used such as "key1.\".key2\"".key3[i]" where
 * .key2 belongs to an object's key
 * @param value [unsigned char*] The pointer to the value in the hash table; the value is searched from the columnName;
 * In addition, the value shall be assigned a static memory space
 * @param item [cJSON**] The cJson node of the value; that implies that in the function, users can obtain the value in a string format;
 * otherwise, users can obtain the node which generates the value; the default value is nullptr; that implies users can use an argument or not
 * to call the function
 * @return [Commons::POSIXErrors] The successful flag
 */
Commons::POSIXErrors InitializedJsonFileParser::getValueFromFileParser(const unsigned char* columnName, unsigned char* value, cJSON** item) {
    // Creating the singleton by reference automatically, the function, getInitializedFileParserInitialization, will be
    // done once, even though the function, getInitializedFileParserInitialization(.) has been called many times
    std::unique_ptr<InitializedJsonFileParser>& initialedFileParserInstance = InitializedJsonFileParser::getInitializedFileParserInitialization();

    // Declaring the token set
    unsigned int length = strlen((const char*)columnName);
    std::vector<std::string> tokenSet;
    if (tokenSet.empty() == false) {
        tokenSet.clear();
        tokenSet.shrink_to_fit();
    }
    bool inQuotes = false;  // For verifying if a quote exists in the key string
    std::string token = "";
    {  // Parsing the instruction from the columnName
        for (unsigned int i = 0; i < length; i++) {
            if (columnName[i] == '\\') {
                // When encountering the '\', the character shall not be reserved
                // because the key which has the character in the key string in the .json file implies only the next character.
                if (i + 1 < length) {
                    token += columnName[++i];  // Reserving the next character
                }

            } else if (columnName[i] == '"') {
                // When encountering the '"', the character shall be reserved because the key has the character in the key string.
                inQuotes = !inQuotes;
                token += columnName[i];
            } else if (columnName[i] == '.' && !inQuotes) {
                // When meeting the character '.' and do not in quotes, pushing the token into the vector
                if (token.length() > 0) {
                    tokenSet.push_back(token);
                    token = "";  // Clearing the token buffer`
                }
            } else if (columnName[i] == '[' && !inQuotes) {
                // When meeting the character '[' and do not in quotes, pushing the token into the vector
                if (token.length() > 0) {
                    tokenSet.push_back(token);
                    token = "";  // Clearing the token buffer`
                }
                // Then, creating a new token and adding the character into the token string
                token += columnName[i];
            } else if (columnName[i] == ']' && !inQuotes) {
                // When meeting the character ']' and do not in quotes, adding the character and pushing the token into the vector
                token += columnName[i];
                tokenSet.push_back(token);
                token = "";  // Clearing the token buffer
            } else {
                token += columnName[i];
            }
        }

        // Adding the last token into the vector
        if (token.length() > 0) {
            tokenSet.push_back(token);
        }
    }

    cJSON* current = nullptr;
    {  // Tracing the parsed json by using the tokens in the tokenSet sequentially
        current = initialedFileParserInstance->jsonParsedContent;
        for (std::vector<std::string>::iterator it = tokenSet.begin();
             it != tokenSet.end();
             ++it) {

            // When the instruction implies the .json's array, the '[' and ']' shall be removed for cJSON to search item.
            if ((*it).front() == '[' && (*it).back() == ']') {
                std::string indexStr = (*it).substr(1, (*it).size() - 2);
                int index = std::stoi(indexStr);  // Converting the string to integer
                current = cJSON_GetArrayItem(current, index);
            } else {
                current = cJSON_GetObjectItem(current, (*it).c_str());
            }

            // A token has not been searched in the .json structure.
            if (current == nullptr) {
                break;
            }
        }
    }

    // Clearing the vector buffer
    tokenSet.clear();
    tokenSet.shrink_to_fit();

    // If the current is not null, ....
    if (current != nullptr) {
        if (item != nullptr) {  // If the item does not come from the initialization, copying the value in the "current"
                                // to the value of the *item
            *item = current;
        }

        std::string output = "";
        // Determining if the cJson type to return a suitable value
        switch (current->type) {
            case cJSON_False:
            case cJSON_True:
                output = (current->type == cJSON_True) ? "true" : "false";
                break;
            case cJSON_NULL:
                break;
            case cJSON_Number:
                output = std::to_string(current->valuedouble);
                break;
            case cJSON_String:
                output = current->valuestring;
                break;
            case cJSON_Array:
            case cJSON_Object:
            case cJSON_Raw:
                std::string tmp(cJSON_Print(current));
                output = tmp;
                break;
        }
        length = output.length();
        memcpy(value, (unsigned char*)(output.c_str()), length);
        value[length] = '\0';
    } else {
        // Clearing the vector buffer
        return Commons::POSIXErrors::E_EXIST;
    }
    return Commons::POSIXErrors::OK;
}

}  // namespace FileParsers