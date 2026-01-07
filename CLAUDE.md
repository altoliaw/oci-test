# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Coding Standards

**All code contributions must follow the conventions defined in `CODING_STANDARDS.md`.**

### IEEE Technical Writing Style (CRITICAL)
**All documentation MUST use IEEE technical specification writing conventions:**
- Use **"shall"** for requirements (NOT "should" or "must")
  - ✅ "the pointer shall be set as nullptr"
  - ❌ "the pointer should be nullptr"
- Use **gerund (-ing) forms** for function descriptions
  - ✅ "Obtaining the result from the hash table"
  - ❌ "Gets the result from the hash table"
- Use **passive voice** for technical descriptions
  - ✅ "The memory is released by using the cJson function"
  - ❌ "We release the memory"
- Use **formal conditional expressions**
  - ✅ "if the value is equal to -1, the error occurs"
  - ❌ "returns -1 on error"
- Reference **users in third person**
  - ✅ "users do not call the function"
  - ❌ "you should not call this function"
- Specify **exact hexadecimal values**
  - ✅ "the value will be 0x0; otherwise 0x1"
  - ❌ "returns true or false"

### Code Style Requirements
- Use `#pragma once` for all header files
- Follow Doxygen documentation style with @file, @author, @date, @param, @return tags
- Naming: PascalCase for classes/namespaces, camelCase for functions/variables
- Headers go in `Headers/` directory, NOT alongside source files
- Use `POSIXErrors` enum for return values
- Platform-specific code uses `#ifdef __linux__` / `#elif defined(_WIN32) || defined(_WIN64)`
- Git commits follow conventional commits format: `type(scope): description`
- Always set pointers to nullptr after deletion
- Validate input parameters before use

## Project Overview

This is a C++ traffic monitoring and sizing application that processes PCAP files and integrates with Oracle databases. The project supports both Linux and Windows, with separate build systems for each platform.

## Build Systems

The project uses **two separate build systems**:

1. **Make (Production)**: For building production applications on Linux
2. **CMake (Testing)**: For running unit tests with GoogleTest

### Make Build (Linux Production)

The Makefile uses a complex configuration system with INI files and shell scripts:

- Compiler settings: `Settings/.Ini/globalCompiling.ini`
- Dependencies: `Settings/.Json/globalDependencies.json`
- Shell utilities: `Shells/` directory (iniParser.sh, codecUtilities.sh, etc.)

**Build Commands:**
```bash
# Build all applications (compiles but doesn't link final executables)
make build

# Build and run (full compilation + linking + execution)
make

# Clean all object files and executables
make clean

# Install third-party dependencies to Vendors/
make vendor

# Show project information
make information
```

The build process compiles in this order:
1. Models/Commons
2. Models/FileParsers
3. Models/PCAP
4. Sources/SizingController
5. Apps/Sizing
6. Apps/Executions (links final executable to Bin/)

### CMake Build (Testing)

**Test Commands:**
```bash
# Run all unit tests
./unitTest.sh

# Run tests with clean build
./unitTest.sh --remove

# Manual CMake workflow
make cmake          # Build and run tests via Makefile
cmake -S . -B build && cmake --build build && cd build && ctest -V

# Build without tests (for Windows compilation)
cmake -S . -B build -DBUILD_TEST=OFF && cmake --build build

# Clean CMake artifacts
make cmakeClean
```

The CMake build is controlled by `BUILD_TEST` option (ON by default). When enabled, it:
- Fetches GoogleTest (or uses system-installed version)
- Builds test executables from Tests/ directory
- Runs ctest

## Architecture

### Directory Structure

- **Models/**: Reusable libraries and data models
  - `Commons/`: Core utilities (HashTable, StringImplement, Time, Log, IOSpecification)
  - `FileParsers/`: INI and JSON file parsers
  - `PCAP/`: Packet capture processing (platform-specific: LinuxPCAP/WindowsPCAP)
  - `DataModels/`: Database data structures
  - `DBUtils/`: Database utility functions

- **Sources/**: Application controllers
  - `SizingController/`: Main business logic for sizing application (platform-specific implementations)
  - `OciController/`: Oracle database integration controller

- **Apps/**: Application entry points
  - `Sizing/`: Main sizing application entry
  - `Executions/`: Final executable linking (creates Bin/SizingMain.exe)
  - `Oci/`: Oracle database test application

- **Headers/**: Public interface headers organized by controller/module
- **Tests/**: GoogleTest unit tests mirroring Models/ structure
- **Shells/**: Build system shell utilities
- **Settings/**: Configuration files
  - `.Ini/`: Compiler settings
  - `.Json/`: Dependency definitions
  - `.Cmake/`: CMake module files
- **Vendors/**: Third-party dependencies (installed via `make vendor`)
- **Bin/**: Final executables
- **Outputs/**: Runtime output files
- **Logs/**: Application logs

### Platform Abstraction

The codebase uses conditional compilation for cross-platform support:
- Platform detection via `#ifdef __linux__` and `#if defined(_WIN32) || defined(_WIN64)`
- Platform-specific implementations in separate files (e.g., LinuxPCAP.cpp, WindowsPCAP.cpp)
- Platform-specific controllers (LinuxSizingMainCaller, WindowsSizingMainCaller)

### Dependency Management

Third-party libraries are managed through `Settings/.Json/globalDependencies.json`:
- **libpcap** (Linux only): Packet capture library
- **nPcap** (Windows only): Windows packet capture
- **cJson**: JSON parsing
- **OCI + ocilib**: Oracle database connectivity (Linux only)

Dependencies are installed to `Vendors/` with organized `Includes/` and `Libs/` subdirectories.

### Build System Details

The Makefile system uses a unique approach:
1. Reads compiler settings from INI files via `Shells/iniParser.sh`
2. Generates temporary Makefiles (`tmp.mk`, `commonTmp.mk`) with variables
3. Recursively invokes sub-Makefiles with hex-encoded arguments
4. Each module compiles object files (.o) in place
5. Final linking happens in Apps/Executions/

**Key variables passed between Makefiles:**
- Compiler flags: CC, STD, CMPOPT, DETAILINFO, WALL, FMSG
- Paths: CFLAGS (includes), LDFLAGS (library paths), LDLIBS (libraries)
- Project metadata: projectDir, ARGUMENTS (hex-encoded), TempMakefile

## Development Workflow

### Running a Single Test

```bash
# Build and run specific test
cmake -S . -B build && cmake --build build
cd build && ctest -R Test_HashTable -V
```

### Adding New Models

1. Create Headers and Sources subdirectories under Models/
2. Add Makefile (follow Models/Commons/Makefile pattern)
3. Update root Makefile build target to include new model
4. Add corresponding test in Tests/Models/
5. Update CMakeLists.txt in Models/ to include subdirectory

### Configuration Files

- Application settings: `Settings/.Ini/SizingMain.ini`, `Settings/.Json/SizingMain.json`
- Modify compiler flags: Edit `Settings/.Ini/globalCompiling.ini`
- Add dependencies: Update `Settings/.Json/globalDependencies.json` and run `make vendor`

## Important Notes

- **Two separate build systems**: Use Make for production builds, CMake only for testing
- **Platform-specific code**: Always provide both Linux and Windows implementations
- **Header organization**: Headers go in Headers/ directory, not alongside source files
- **Object files**: Compiled in-place within each module directory
- **Final executables**: Linked in Apps/Executions/ and output to Bin/
- **Temporary files**: tmp.mk and commonTmp.mk are auto-generated, don't edit manually
- The build system encodes arguments as hex strings when passing between Makefiles
