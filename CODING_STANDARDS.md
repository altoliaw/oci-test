# Coding Standards

This document defines the coding conventions and style guidelines for this project, based on analysis of commit 3c1db0a.

## File Structure & Documentation

### Header Files
```cpp
#pragma once
/** @file FileName.hpp
 * Brief description of the file's purpose
 *
 * @author Nick, Liao
 * @date YYYY/MM/DD
 *
 * @note Dependencies or important information (if applicable)
 */
```

### Source Files
```cpp
/**
 * @see FileName.hpp
 */
#include "../Headers/FileName.hpp"
```

### Function Documentation
```cpp
/**
 * Brief description of the function's purpose and behavior
 *
 * @param parameterName [type] Description of parameter
 * @param anotherParam [type] Description; include default values and constraints
 * @return [type] Description of return value; include special values (e.g., -1 for error)
 */
```

## IEEE Technical Writing Style

**All documentation and comments MUST follow IEEE technical specification writing conventions.**

### Modal Verbs (RFC 2119 / IEEE Standards)

- **"shall"**: Use for mandatory requirements or specifications
  - ✅ "the value of the size shall be set as a prime"
  - ✅ "the pointer shall be modified as nullptr"
  - ✅ "the variable in the Makefile shall be added"
  - ❌ NOT "should be" or "must be" for specifications

- **"will"**: Use for declarative statements about behavior
  - ✅ "all variables in the class will be initialized"
  - ✅ "the function will be executed"
  - ✅ "\0 will be added at the last character automatically"

- **"may"**: Use for optional or conditional behavior
  - ✅ "the function may cause an error when the string is null"
  - ✅ "this function may be the bottleneck"

### Writing Conventions

#### 1. Use Gerund (-ing) Forms for Function/Feature Descriptions
```cpp
// ✅ CORRECT
/**
 * Obtaining the result from the hash table
 * Transforming the POSIX time string into the epoch
 * Removing the element by using the specific name
 * Adding an element into the hash table
 */

// ❌ INCORRECT
/**
 * Gets the result from the hash table
 * Transform the POSIX time string
 * Remove the element
 */
```

#### 2. Use Passive Voice for Technical Descriptions
```cpp
// ✅ CORRECT
/**
 * The pointer is referred to a static object
 * The memory is released by using the cJson function
 * The value is equal to -1
 * The function has been called many times
 */

// ❌ INCORRECT
/**
 * We refer the pointer to a static object
 * You release the memory
 * It equals -1
 */
```

#### 3. Use Formal Conditional Expressions
```cpp
// ✅ CORRECT
/**
 * if the value is equal to -1, the error occurs
 * if the pointer is equal to nullptr, initialization will be performed
 * when the returned value is 0, the string contains the pattern
 */

// ❌ INCORRECT
/**
 * returns -1 on error
 * nullptr means not initialized
 * 0 = string contains pattern
 */
```

#### 4. Reference Users in Third Person
```cpp
// ✅ CORRECT
/**
 * users do not call the function in the codes
 * which users define for transforming to the time string
 * the pointer can be reused from the caller
 */

// ❌ INCORRECT
/**
 * you should not call this function
 * you can define this for transforming
 */
```

#### 5. Use Precise Technical Language
```cpp
// ✅ CORRECT
/**
 * The copied column name from the element in the hash
 * The address of the value of the column name
 * The starting address of the name string
 * The size of memory of the value above
 */

// ❌ INCORRECT
/**
 * Column name copy from hash element
 * Value address
 * String address
 * Memory size
 */
```

#### 6. Specify Exact Values and Conditions
```cpp
// ✅ CORRECT
/**
 * @return [char] The number of the hit element; if the element does not exist,
 * the value will be 0x0; otherwise 0x1
 *
 * @return [long] The epoch; if the value is -1, the error occurs
 *
 * @param direction [const char] 0x0 is from the left-hand side and
 * the 0x1 is the right-hand side (the default is 0)
 */

// ❌ INCORRECT
/**
 * @return true if found, false otherwise
 * @return epoch or -1 on error
 * @param direction left (0) or right (1), default 0
 */
```

### Documentation Examples

#### Complete Function Documentation
```cpp
/**
 * Obtaining the value from the hash table by using the column name; if the element does not exist,
 * the returned value will be 0x0
 *
 * @param columnName [char*] The column name
 * @param columnNameInHashAddress [char**] The copied column name from the element in the hash
 * (the pointer can be reused from the caller)
 * @param value [void**] The address of the value of the column name
 * @param sizeOfMemory [size_t*] The address of the size of memory of the value; using the
 * memory to obtain the memory from the caller
 * @param type [ElementType*] The pointer to the type from the caller
 * @return [char] The number of the hit element; if the element does not exist,
 * the value will be 0x0; otherwise 0x1
 */
```

#### Constructor Documentation
```cpp
/**
 * Constructor
 *
 * @param size [int] The size of the hashtable; the default value
 * is 97 (a prime); if possible, the value of the size shall be set as a prime
 */
```

#### Static Method Documentation
```cpp
/**
 * The static method for initializing the singleton
 *
 * @return [std::unique_ptr<InitializedJsonFileParser>&] The reference to the
 * "InitializedFileParser" instance
 */
```

### IEEE Style Quick Reference

| Context | Use | Example |
|---------|-----|---------|
| Mandatory requirement | shall | "the pointer shall be nullptr" |
| Future behavior | will | "the function will be called" |
| Optional/conditional | may | "may cause an error" |
| Possibility | can | "the pointer can be reused" |
| Function purpose | Gerund (-ing) | "Obtaining the value" |
| Technical description | Passive voice | "is referred to" |
| Conditions | "if ... , ..." | "if the value is -1, the error occurs" |
| User reference | third person | "users define" |
| Return values | Exact values | "0x0" or "0x1", not "true/false" |

## Naming Conventions

### C++ Code
- **Classes**: PascalCase
  - `HashTable`, `StringImplement`, `InitializedJsonFileParser`
- **Functions/Methods**: camelCase
  - `getValueByName()`, `addElementIntoHashTable()`, `trimLeftSpace()`
- **Variables**: camelCase
  - `columnName`, `hashTableSize`, `operatedElement`, `sizeOfMemory`
- **Member Variables**: camelCase (no prefix)
  - `hashTableSize`, `queue`, `lastElement`
- **Namespaces**: PascalCase
  - `Commons`, `FileParsers`, `SizingMainCaller`
- **Enums**: PascalCase
  - `ElementType`, `POSIXErrors`, `POSIXSysLog`
- **Enum Values**: camelCase for types, PascalCase for severity levels
  - `charType`, `integerType`, `stringType`
  - `Emergency`, `Alert`, `Critical`

### Makefiles
- **Variables**: UPPERCASE or dot.CamelCase
  - `CC`, `CFLAGS`, `LDFLAGS`, `projectDir`
  - `Commons.Sources`, `Commons.Headers`, `PCAP.Sources`
- **Phony Targets**: lowercase
  - `all`, `clean`, `build`, `vendor`, `information`

### Shell Scripts
- **Functions**: camelCase or snake_case
  - `getIniValue()`, `iniParser_getIni()`, `dependenciesTraversal()`
- **Variables**: camelCase with "local" keyword
  - `local fileName`, `local currentSection`, `local key`

## Code Organization

### Directory Structure
- **Headers**: Place all header files in `Headers/` directory organized by module
  - `Headers/SizingController/`, `Headers/OciController/`
- **Sources**: Implementation files stay with their modules
  - `Models/Commons/Sources/`, `Sources/SizingController/`
- **Separation**: Headers and sources are in separate directories, NOT side-by-side

### Namespace Usage
- Every module should have its own namespace
- Close namespaces with comment:
```cpp
}  // namespace Commons
```
- Single space before comment

### Platform-Specific Code
```cpp
#ifdef __linux__  // Linux
    // Linux-specific implementation
#elif defined(_WIN32) || defined(_WIN64)
    // Windows-specific implementation
#endif
```

- Use separate files for platform implementations: `LinuxPCAP.cpp`, `WindowsPCAP.cpp`
- Use prototype/abstract base class pattern for platform abstraction

## Formatting

### Indentation & Spacing
- **Indentation**: 4 spaces (NO TABS in C++ code)
- **Line Length**: Keep reasonable (observed ~100-120 characters max)
- **Braces**: Opening brace on same line
```cpp
void function() {
    // code
}

if (condition) {
    // code
} else {
    // code
}
```

### Operators & Expressions
- Spaces around binary operators: `a + b`, `x == y`, `i = 0`
- No space after unary operators: `!flag`, `*pointer`, `&variable`
- Spaces after commas: `function(a, b, c)`
- Spaces around assignment in for loops: `for (int i = 0; i < n; i++)`

### Alignment
- Align related variable declarations:
```cpp
char*           columnName;
void*           value;
Element*        nextInHashTable;
size_t          sizeOfMemory;
ElementType     type;
```

### Comments
- Use `//` for single-line comments
- Use `/* */` for inline explanations
- Trailing comments aligned when explaining related lines
- Multi-line comments use `/** */` or `/* */`
- Comments explain "why", not "what" (except for complex algorithms)

## Memory Management

### Allocation & Deallocation
- **Dynamic Arrays**: Use `new[]` and `delete[]`
```cpp
char* tmpColumnName = new char[strlen(columnName) + 1];
delete[] tmpColumnName;
```

- **Objects**: Use `new` and `delete`
```cpp
Element* instance = new Element(...);
delete instance;
instance = nullptr;
```

- **C-style Memory**: Use `calloc`/`malloc` and `free` for void* buffers
```cpp
void* tmpValue = calloc(1, sizeOfMemory);
free(tmpValue);
```

### Smart Pointers
- Use `std::unique_ptr` for singletons and ownership semantics
```cpp
static std::unique_ptr<InitializedJsonFileParser> initializedFileParserPointer;
```

### Cleanup
- **Always** set pointers to nullptr after deletion
- Implement proper destructors for resource cleanup
- Use RAII principles where applicable

## Error Handling

### Return Values
- Use custom `POSIXErrors` enum for error codes
- Return `POSIXErrors::OK` for success
- Check for nullptr before operations:
```cpp
if (columnName == nullptr) {
    return POSIXErrors::E_NOITEM;
}
```

### Error Checking
- Validate input parameters at function entry
- Return appropriate error codes:
  - `E_NOITEM`: Item not found
  - `E_NOMEM`: Memory allocation failed
  - `E_INVAL`: Invalid parameter
  - `OK`: Success

## Class Design

### Constructors & Destructors
- Provide default parameter values where appropriate
```cpp
HashTable(int = 97);
```

- Always declare virtual destructors for base classes
```cpp
virtual ~HashTable();
```

- Initialize member variables in constructor initialization lists or constructor body
```cpp
HashTable::HashTable(int size) {
    hashTableSize = size;
    queue = lastElement = operatedElement = nullptr;
    hashTable = new Element*[hashTableSize]{}; // Zero-initialize
}
```

### Access Modifiers
- Order: `public`, `protected`, `private`
- Use `virtual` for interface methods in base classes
```cpp
class SizingMainCallerPrototype {
   public:
    virtual Commons::POSIXErrors start(int, char**) = 0;
};
```

### Struct vs Class
- Use `struct` for data containers with public members
- Use `class` for encapsulated objects with behavior

## Makefiles

### Structure
```makefile
# Brief description of the Makefile
#
# @author Nick
# @date YYYY/MM/DD

# ######## [Section Name]
# Comments explaining the section
VARIABLE := value

# ######## [Phonies]
.Phony: target
target: dependencies
	@command
```

### Conventions
- Use `######## [Section Name]` for major sections
- Use `:=` for immediate assignment
- Use `=` for recursive assignment
- Prefix recipes with `@` to suppress echo (for clean output)
- Use `${variable}` for variable expansion
- Document each phony target
- Include author and date in header

## Shell Scripts

### Structure
```bash
#!/bin/bash
# /**
#  * Brief description of the script
#  *
#  * @author Nick
#  * @Date YYYY/MM/DD
#  */

# /**
#  * Function description
#  *
#  * @param $0 The function called by users
#  * @param $1 Description of first parameter
#  * @param $2 Description of second parameter
#  * @return Description of return value
#  */
function functionName() {
    local variableName=$1
    # Implementation
}
```

### Best Practices
- Always use `local` for function-local variables
- Quote variable expansions: `"$variable"`
- Use `[[ ]]` for conditionals instead of `[ ]`
- Provide clear parameter documentation

## CMake

### Structure
```cmake
# [Section Description]
## Additional comments about the section
## Multi-line explanation if needed
command(parameters)
```

### Conventions
- Use `if (CONDITION)` ... `endif()` (include condition in endif comment when complex)
- Document each section with `# [Section Name]`
- Use `##` for detailed explanations
- Uppercase for CMake commands: `add_subdirectory()`, `enable_testing()`

## Testing

### GoogleTest Conventions
```cpp
#ifdef __linux__
TEST(ModuleName, FeatureName_specificCase) {
    // Arrange
    Commons::HashTable table(1);

    // Act
    table.addElementIntoHashTable(...);

    // Assert
    ASSERT_EQ(result, expectedValue);
}
#endif
```

### Test Naming
- Format: `TEST(Module, Feature_specificCase)`
- Use descriptive names: `HashTable_addNRemoveFirstElements`
- Wrap platform-specific tests with `#ifdef`

## Git Commit Messages

### Format
```
type(scope): Brief description in lowercase

Optional body with detailed explanation of changes
```

### Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `refactor`: Code refactoring
- `test`: Adding/modifying tests
- `chore`: Build process, dependencies

### Examples
```
feat(OCI): Add fetch data function (without post processing)
fix(makefile): The multi-specific targets have been added in the Makefile
docs(makefile): Spelling corrections in Makefile comments
refactor(tmp.c): migrate tmp.c to C++ with MVC architecture
```

## Best Practices

### General Principles
1. **Clarity over cleverness**: Write clear, readable code
2. **Document complex logic**: Explain non-obvious implementations
3. **Validate inputs**: Check parameters before use
4. **Clean up resources**: Always free allocated memory
5. **Handle errors gracefully**: Return meaningful error codes
6. **Platform compatibility**: Support both Linux and Windows
7. **Maintain consistency**: Follow existing patterns in the codebase

### Code Comments
- Use comments to explain "why", not "what"
- Document assumptions and constraints
- Explain complex algorithms with detailed comments
- Update comments when code changes

### Variable Initialization
- Initialize pointers to nullptr
- Zero-initialize arrays: `new Type[size]{}`
- Validate before use

### String Handling
- Always null-terminate C strings
- Use `strlen()` + 1 for allocation
- Prefer `strcpy`, `strcmp`, `strncpy` for C-style strings
- Include `<string.h>` for C string functions

### Include Guards
- Always use `#pragma once` in header files
- Place at the very beginning of the file

### Dependencies
- Document dependencies with `@note` in file headers
- List third-party dependencies explicitly
```cpp
/** @file FileName.hpp
 * Description
 *
 * @note The file is dependent to the Models.Commons
 * @note The file is dependent to the third party software, cJson
 */
```
