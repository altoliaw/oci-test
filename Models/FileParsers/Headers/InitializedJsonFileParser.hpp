#pragma once
/** @file InitializedJsonFileParser.hpp
 * A class for parsing the key and value pairs from the .json file; this class will be implemented
 * in a singleton pattern; for unit testing the singleton pattern in various unit tests, the unique
 * pointer shall be used; this is because the static pointer shall refer to the dynamic memory manually and
 * the memory shall be released manually; if the pointer is a normal pointer and refers to a static instance
 * in the initialization function, the dynamic memory released phase can not be implemented; in addition, unlike
 * "initializedFileParser" class, the class uses cJson's reserved mechanism for reserving the K-V pairs; as a 
 * result, no hash tables which users defined are used
 *
 * @author Nick, Liao
 * @date 2024/08/05
 * 
 * @note The file is dependent to the Models.Commons
 * @note The file is dependent to the third party software, cJson where installed in Vendors folder
 */
#include <string.h>
#include <iostream>

#include <memory>  // For the unique pointer
#include <vector>  // To simulate the stack container
#include <string>

#include "../../../Vendors/cJson/Includes/cJSON.h"
#include "../../Commons/Headers/HashTable.hpp"
#include "../../Commons/Headers/StringImplement.hpp"

namespace FileParsers {
/**
 * Several InitializedFileParser implementation (singleton)
 */
class InitializedJsonFileParser {
   public:
    // For reserving the parsed content from the cJson parser; the pointer refers to
    // the cJson object
    cJSON* jsonParsedContent;

    // A static variable for the object from the class (singleton); a unique pointer
    // shall be declared
    static std::unique_ptr<InitializedJsonFileParser> initializedFileParserPointer;

    static std::unique_ptr<InitializedJsonFileParser>& getInitializedFileParserInitialization();
    static Commons::POSIXErrors releaseInitializedFileParserInitialization();
    static Commons::POSIXErrors parseInitializedFile(const unsigned char*);
    static Commons::POSIXErrors getValueFromFileParser(const unsigned char*, unsigned char*, cJSON** = nullptr);

    ~InitializedJsonFileParser();

   private:
    InitializedJsonFileParser();
};
}  // namespace FileParsers