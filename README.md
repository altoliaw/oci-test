# C++ Applications - Analysis System

**Project**: Native Sizing (native-sizing)
**Author**: Nick, Liao
**Started**: May 15th, 2024
**Language**: C++17
**Build System**: CMake 3.14+
**Platform**: Linux (RHEL 8.7+), Windows

---

## Table of Contents

- [Quick Start](#quick-start)
- [Overview](#overview)
- [Architecture](#architecture)
- [Directory Structure](#directory-structure)
- [Development Environment](#development-environment)
- [Building the Project](#building-the-project)
- [Testing](#testing)
- [Adding New Features](#adding-new-features)
- [Coding Standards](#coding-standards)
- [Dependencies](#dependencies)

---

## Quick Start

### For Production Build

```bash
# Clone the repository
git clone <repository-url>
cd <repository>

# Build production application
./processMake.sh --r

# Executables will be generated in build/Apps/
```

### For Running Tests

```bash
# Run all unit tests
./unitTest.sh

# Run tests with clean build
./unitTest.sh --remove
```

### Important Build Scripts

The project provides two main shell scripts for building:

1. **processMake.sh** - Use this for building production applications
2. **unitTest.sh** - Use this for running unit tests

Do NOT use `make` or `cmake` commands directly unless you are familiar with the build system configuration. Always use the provided shell scripts.

---

## Overview

The Native Sizing project is a high-performance C++ application designed for traffic monitoring, packet capture analysis, and network sizing calculations. The system integrates with Oracle databases for data persistence and provides comprehensive network traffic analysis capabilities.

### Key Features

- **Packet Capture Analysis**: Processes PCAP files for network traffic analysis
- **Cross-Platform Support**: Runs on Linux (libpcap) and Windows (nPcap/WinDivert)
- **Database Integration**: Oracle database connectivity via OCI and ocilib
- **Modular Architecture**: Clean separation between models, controllers, and applications
- **Comprehensive Testing**: GoogleTest-based unit testing framework

---

## Architecture

The project follows a **layered MVC (Model-View-Controller) architecture** with clear separation of concerns:

```
┌─────────────────────────────────────────────┐
│         Applications (Apps/)                 │
│  Entry points for executables               │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│       Controllers (Sources/)                 │
│  Business logic and orchestration           │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│         Models (Models/)                     │
│  Reusable libraries and utilities           │
└─────────────────────────────────────────────┘
```

### Platform Abstraction

The system uses **conditional compilation** and **abstract base classes** for platform-specific implementations:

- **Linux**: libpcap for packet capture
- **Windows**: nPcap or WinDivert for packet capture

Platform-specific code is isolated in separate implementation files (e.g., `LinuxPCAP.cpp`, `WindowsNpcapPCAP.cpp`) and selected at compile time.

---

## Directory Structure

### Top-Level Directories

| Directory | Purpose | Description |
|-----------|---------|-------------|
| **Apps/** | Application Entry Points | Contains main() functions for executable programs |
| **Sources/** | Controllers | Business logic and application controllers |
| **Models/** | Reusable Libraries | Core utilities, data models, and platform abstractions |
| **Headers/** | Public Interfaces | Exported header files organized by module |
| **Tests/** | Unit Tests | GoogleTest-based test suites mirroring the Models/ structure |
| **Settings/** | Configuration | INI, JSON, and CMake configuration files |
| **Vendors/** | Third-Party Libraries | External dependencies (installed via dependency manager) |
| **Services/** | Service Layer | Service implementations for business operations |
| **build/** | Build Artifacts | CMake-generated files and compiled binaries (auto-generated) |
| **Bin/** | Final Executables | Linked executables ready for deployment (auto-generated) |
| **Outputs/** | Runtime Output | Application-generated output files |
| **Logs/** | Log Files | Application logs and debugging information |

---

### Detailed Directory Explanations

#### **Apps/** - Application Entry Points
Contains the main entry points for different executable applications. Each subdirectory represents a separate application:

- **`Apps/Sizing/`**: Main sizing application entry point
- **`Apps/Oci/`**: Oracle database testing and verification application
- **`Apps/Executions/`**: (Legacy) Final executable linking directory

**Usage**: When creating a new application, add a subdirectory under `Apps/` with a main entry point file.

---

#### **Sources/** - Controllers
Contains application controllers which implement the business logic and orchestrate the interaction between models:

- **`Sources/SizingControllers/`**: Controllers for the sizing application
  - Platform-specific implementations (Linux, Windows)
  - Abstract base classes (Prototype pattern)
- **`Sources/OciController/`**: Oracle database operation controllers

**Usage**: Controllers shall be organized by feature or subsystem. Use abstract base classes for platform-specific implementations.

---

#### **Models/** - Reusable Libraries
Core libraries and utilities that can be reused across different applications:

##### **`Models/Commons/`** - Common Utilities
Core utility classes used throughout the project:
- `HashTable`: Custom hash table implementation
- `StringImplement`: String manipulation utilities (trim, merge spaces, etc.)
- `Time`: Time conversion and formatting utilities
- `IOExporters`: Input/output export operations
- `POSIXErrors`: Standard error code definitions

##### **`Models/FileParsers/`** - Configuration File Parsers
Parsers for initialization and configuration files:
- `InitializedFileParser`: INI file parser
- `InitializedJsonFileParser`: JSON file parser using cJSON

##### **`Models/PCAP/`** - Packet Capture
Platform-specific packet capture implementations:
- `PCAPPrototype`: Abstract base class
- `LinuxPCAP`: Linux libpcap implementation
- `WindowsNpcapPCAP`: Windows nPcap implementation
- `WindowsWinDivertPCAP`: Windows WinDivert implementation

##### **`Models/DataModels/`** - Data Models
Data structures and models for database operations:
- `TestDataModel`: Test data structures

##### **`Models/DBUtils/`** - Database Utilities
Database connectivity and SQL operations:
- `DBConnector`: Oracle database connection manager (Singleton pattern)
- `DBSql`: SQL utility functions for batch operations and data fetching

**Usage**: When adding new functionality, first check if it belongs in an existing model. If creating a new model, follow the existing structure with Headers/ and Sources/ subdirectories.

---

#### **Headers/** - Public Interfaces
Exported header files organized by module. This separation keeps the public API clean:

- **`Headers/SizingControllers/`**: Public interfaces for sizing controllers
- **`Headers/OciController/`**: Public interfaces for OCI controllers

**Important**: Headers are separated from their implementation sources. When adding a new module, create corresponding headers in this directory.

---

#### **Tests/** - Unit Tests
GoogleTest-based unit tests mirroring the Models/ structure:

- **`Tests/Models/Commons/`**: Tests for common utilities
- **`Tests/Models/FileParsers/`**: Tests for file parsers
- **`Tests/Shells/`**: Tests for shell script utilities

**Testing Philosophy**: Each model should have corresponding unit tests. Tests are only built when `BUILD_TEST=ON` (default).

---

#### **Settings/** - Configuration Files

##### **`Settings/.Cmake/`** - CMake Modules
CMake configuration files:
- `.globalCmakeVariable.cmake`: Global CMake variables
- `.systemSupport.cmake`: Platform detection
- `.testingInstruction.cmake`: Test configuration
- `.compiledRouter.cmake`: Build routing (test vs. release)

##### **`Settings/.Json/`** - JSON Configuration
Application configuration files:
- `SizingMain.json`: Main sizing application settings

**Usage**: Configuration files shall be stored here and loaded at runtime using the FileParsers models.

---

#### **Vendors/** - Third-Party Libraries
External dependencies installed via the dependency management system:

| Library | Platform | Purpose |
|---------|----------|---------|
| **libpcap** | Linux | Packet capture library |
| **nPcap** | Windows | Windows packet capture SDK |
| **cJSON** | Both | JSON parsing library |
| **OCI** | Linux | Oracle Instant Client |
| **ocilib** | Linux | OCI wrapper library |

Dependencies are defined in `Settings/.Json/globalDependencies.json` (if available) and installed automatically.

**Usage**: Do not manually add files to this directory. Use the dependency management system.

---

#### **Services/** - Service Layer
Service implementations for business operations:

- **`Services/SizingServices/`**: Business services for sizing operations

**Usage**: Services provide higher-level business operations that coordinate multiple controllers and models.

---

## Development Environment

### Required Tools (RHEL 8.7)

```bash
g++ (GCC) 8.5.0 20210514 (Red Hat 8.5.0-15)
gcc (GCC) 8.5.0 20210514 (Red Hat 8.5.0-15)
cmake version 3.20.2 or higher
git 2.x
```

### Optional Tools

```bash
jq-1.6          # JSON processing (for dependency management)
GNU Awk 4.2.1   # Text processing (for build scripts)
```

### Platform-Specific Requirements

#### Linux (RHEL/CentOS)
- Development tools: `sudo yum groupinstall "Development Tools"`
- CMake: `sudo yum install cmake`
- libpcap development headers: Will be installed via dependency manager

#### Windows
- Visual Studio 2019 or later with C++ support
- CMake 3.14+
- nPcap SDK: Will be installed via dependency manager

---

## Building the Project

The project uses **CMake** as the primary build system. Use the provided shell scripts for building:

### Build Scripts

The project provides two main build scripts:

#### 1. processMake.sh - Production Build
Builds the production application for release:

```bash
# Build production application
./processMake.sh

# Clean build (remove build directory first)
./processMake.sh --remove

# Clean build and reinstall vendors
./processMake.sh --remove --vendor
```

**Supported Options:**
- `--remove`, `--r`, `-r`: Remove build directory before building
- `--vendor`, `--v`, `-v`: Remove and reinstall all vendor dependencies

This script will:
- Install dependencies from `Vendors/` if needed (via `Shells/installVendor.sh`)
- Create required directories (`Outputs/`, `Logs/`, `Bin/`)
- Configure CMake for release mode (BUILD_TEST=OFF, CMAKE_BUILD_TYPE=Debug)
- Detect operating system (Linux or Windows/MinGW)
- Compile all production code
- Generate executables in `build/Apps/`
- On Windows: Perform post-processing (copy WinDivert DLLs to Bin/)

#### 2. unitTest.sh - Testing Build
Builds and runs unit tests using GoogleTest:

```bash
# Run all unit tests
./unitTest.sh

# Clean build and run tests
./unitTest.sh --remove
```

This script will:
- Install dependencies from `Vendors/` if needed
- Remove existing build directory (if --remove flag is used)
- Configure CMake for testing mode (BUILD_TEST=ON)
- Compile test code and GoogleTest framework
- Run all unit tests with verbose output

### Manual CMake Build (Advanced)

For advanced users who need direct CMake control:

#### Testing Mode (Default)
```bash
cmake -S . -B build
cmake --build build
cd build && ctest -V
```

#### Release Mode
```bash
cmake -S . -B build -DBUILD_TEST=OFF
cmake --build build
```

### Build Output Locations

- **Testing Mode**: Test executables in `build/Tests/`
- **Release Mode**: Application executables in `build/Apps/`

### Cleaning Build Artifacts

```bash
# Remove all build artifacts
rm -rf build/

# Or use build scripts with --remove flag
./processMake.sh --remove     # For production build
./unitTest.sh --remove         # For test build

# Clean build and remove vendor dependencies
./processMake.sh --remove --vendor
```

**Note**: The `--vendor` flag will force reinstallation of all third-party libraries in the `Vendors/` directory.

---

## Testing

### Running All Tests

Use the provided test script:

```bash
# Run all unit tests
./unitTest.sh

# Clean build and run all tests
./unitTest.sh --remove
```

The script will:
- Install dependencies automatically
- Build all test code with GoogleTest
- Run all tests with verbose output
- Display failed tests and rerun them with detailed output

### Running Specific Tests (Manual)

For advanced testing scenarios:

```bash
# Build tests first
cmake -S . -B build
cmake --build build

# Run specific test by name
cd build && ctest -R Test_HashTable -V

# Run all tests with verbose output
cd build && ctest -V

# Rerun only failed tests
cd build && ctest --rerun-failed --output-on-failure
```

### Test Structure

Tests are organized to mirror the Models/ structure:
- `Tests/Models/Commons/` → tests for `Models/Commons/`
- `Tests/Models/FileParsers/` → tests for `Models/FileParsers/`

### Adding New Tests

1. Create test file in appropriate `Tests/` subdirectory
2. Follow GoogleTest naming convention: `Test_<ModuleName>.cpp`
3. Update `Tests/CMakeLists.txt` to include the new test
4. Run tests to verify

---

## Adding New Features

When developing new features, follow this decision tree:

### Feature Development Decision Tree

```
┌─ New Feature Request
│
├─ Q1: Is this a standalone application?
│  ├─ YES → Create new subdirectory in Apps/
│  │        Add main() entry point
│  │        Update CMakeLists.txt
│  │
│  └─ NO → Continue to Q2
│
├─ Q2: Is this business logic or orchestration?
│  ├─ YES → Add to appropriate controller in Sources/
│  │        Consider if platform-specific implementation is needed
│  │        Use abstract base class pattern if multi-platform
│  │
│  └─ NO → Continue to Q3
│
├─ Q3: Is this a reusable utility or library?
│  ├─ YES → Add to appropriate model in Models/
│  │        Categories:
│  │        - Commons: Generic utilities
│  │        - FileParsers: File parsing
│  │        - PCAP: Network capture
│  │        - DBUtils: Database operations
│  │        - DataModels: Data structures
│  │
│  └─ NO → Continue to Q4
│
└─ Q4: Is this a high-level service operation?
   ├─ YES → Add to Services/
   │        Services coordinate multiple controllers/models
   │
   └─ NO → Reconsider feature design
```

### Design Considerations

When implementing new features, consider:

#### 1. **Platform Compatibility**
- Will this feature work on both Linux and Windows?
- If platform-specific:
  - Create abstract base class (e.g., `FeaturePrototype`)
  - Implement platform-specific versions (`LinuxFeature`, `WindowsFeature`)
  - Use `#ifdef __linux__` / `#elif defined(_WIN32)` for conditional compilation

#### 2. **Code Reusability**
- Can this be used by multiple applications?
  - YES → Place in `Models/`
  - NO → Place in `Sources/` or `Apps/`

#### 3. **External Dependencies**
- Does this require third-party libraries?
  - Add to dependency configuration
  - Document in README
  - Ensure cross-platform availability

#### 4. **Testing Strategy**
- All Models/ code **must** have unit tests
- Controllers should have integration tests
- Use GoogleTest framework
- Aim for >80% code coverage

#### 5. **Error Handling**
- Use `Commons::POSIXErrors` enum for return values
- Validate input parameters before use
- Return `POSIXErrors::OK` on success
- Document error conditions in function comments

#### 6. **Memory Management**
- Use RAII principles
- Set pointers to `nullptr` after deletion
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) for ownership
- Use raw pointers only for non-owning references

#### 7. **Documentation**
- Follow IEEE technical writing style (see CODING_STANDARDS.md)
- Use Doxygen-style comments with @file, @param, @return tags
- Document complex algorithms and assumptions
- Use gerund forms for function descriptions

---

## Coding Standards

**All code must follow the conventions defined in `CODING_STANDARDS.md`.**

### Quick Reference

#### File Structure
```cpp
#pragma once
/** @file FileName.hpp
 * Brief description
 *
 * @author Nick, Liao
 * @date YYYY/MM/DD
 */
```

#### Function Documentation (IEEE Style)
```cpp
/**
 * Obtaining the value from the hash table by using the column name; if the element
 * does not exist, the returned value will be 0x0
 *
 * @param columnName [char*] The column name
 * @param value [void**] The address of the value of the column name
 * @return [char] The number of hit elements; if the value is 0x0, no element is found
 */
```

#### Naming Conventions
- **Classes**: PascalCase (`HashTable`, `StringImplement`)
- **Functions**: camelCase (`getValueByName`, `addElement`)
- **Variables**: camelCase (`columnName`, `hashTableSize`)
- **Namespaces**: PascalCase (`Commons`, `FileParsers`)

#### Git Commit Messages
```
type(scope): Brief description in lowercase

Examples:
feat(OCI): Add fetch data function
fix(makefile): The multi-specific targets have been added
docs(readme): Update architecture documentation
refactor(models): Migrate to MVC architecture
```

---

## Dependencies

### Runtime Dependencies

| Library | Version | Platform | Purpose |
|---------|---------|----------|---------|
| libpcap | 1.9.1+ | Linux | Packet capture |
| nPcap | 1.13+ | Windows | Windows packet capture |
| cJSON | latest | Both | JSON parsing |
| Oracle Instant Client | 19.10 | Linux | Database connectivity |
| ocilib | latest | Linux | OCI wrapper |

### Build Dependencies

- CMake 3.14+
- C++17 compatible compiler
- GoogleTest (auto-fetched by CMake)

### Installing Dependencies

Dependencies are managed automatically. The build system will:
1. Check for system-installed libraries
2. Download and build missing dependencies
3. Install to `Vendors/` directory

Manual installation (if needed):
```bash
# Install vendor dependencies
cmake -S . -B build
# Dependencies will be fetched during configuration
```

---

## Project Status

### Current Version
- **Build System**: CMake only (Makefile deprecated as of commit 1ad2a98)
- **C++ Standard**: C++17
- **Testing**: GoogleTest framework integrated
- **Platforms**: Linux (primary), Windows (in development)

### Recent Changes
- Migrated from Make to CMake build system
- Refactored `SizingController` → `SizingControllers` (multi-platform support)
- Added Oracle database integration (OCI/ocilib)
- Implemented comprehensive unit testing framework

### Known Issues
- Application build (BUILD_TEST=OFF) requires path fixes for Vendors/ libraries
- Windows platform support is under active development

---

## Additional Resources

- **CLAUDE.md**: Guidance for AI-assisted development in this repository
- **CODING_STANDARDS.md**: Detailed coding conventions and IEEE writing style
- **Settings/.Cmake/**: CMake module documentation

---

## Support and Contact

For questions or issues:
1. Check existing documentation (CLAUDE.md, CODING_STANDARDS.md)
2. Review code comments and unit tests
3. Contact: Nick, Liao

---

**Last Updated**: January 2026
**Project Start**: May 15th, 2024
