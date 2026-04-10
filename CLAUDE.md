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

This is a C++ traffic monitoring and sizing application (native-sizing) that processes PCAP files and integrates with Oracle databases. The project supports both Linux (RHEL 8.7+) and Windows, using CMake as the primary build system.

## Build System

CMake is the **only** build system. Do NOT use `make` or `cmake` commands directly — use the provided shell scripts instead.

### Production Build

```bash
# Build production application (executables output to build/Apps/)
./processMake.sh

# Clean build (removes build/ first)
./processMake.sh --remove

# Clean build and reinstall vendor dependencies
./processMake.sh --remove --vendor
```

On Linux: builds a single target. On Windows (MinGW): builds both nPcap and WinDivert targets sequentially.

### Testing Build

```bash
# Run all unit tests
./unitTest.sh

# Clean build and run tests
./unitTest.sh --remove
```

### Running a Single Test

```bash
cmake -S . -B build && cmake --build build
cd build && ctest -R Test_HashTable -V
```

### CMake Options

- `BUILD_TEST=ON` (default): builds GoogleTest unit tests from `Tests/`
- `BUILD_TEST=OFF`: builds production application executables
- `OPERATING_SYSTEM`: override platform detection (used by processMake.sh on Windows)

## Architecture

The project follows a layered MVC architecture: **Apps → Sources (Controllers) → Models**.

### Directory Structure

- **Apps/**: Executable entry points
  - `Sizing/`: Main sizing application
  - `Oci/`: Oracle database test app using ocilib wrapper
  - `OciRaw/`: Oracle database test app using raw OCI API (no ocilib)
  - `Executions/`: Legacy; superseded by CMake build output in `build/Apps/`

- **Sources/**: Business logic controllers
  - `SizingControllers/`: Sizing app logic with platform-specific implementations
  - `OciControllers/`: Oracle operations via ocilib
  - `OciRawControllers/`: Oracle operations via raw OCI API directly

- **Models/**: Reusable libraries
  - `Commons/`: Core utilities — `HashTable`, `StringImplement`, `Time`, `IOExporters`, `POSIXErrors`
  - `FileParsers/`: INI (`InitializedFileParser`) and JSON (`InitializedJsonFileParser`) parsers
  - `PCAP/`: Packet capture — abstract `PCAPPrototype`, `LinuxPCAP`, `WindowsNpcapPCAP`, `WindowsWinDivertPCAP`
  - `DataModels/`: Data structures for DB operations
  - `DBUtils/`: `DBConnector` (Singleton), `DBSql` for batch SQL operations

- **Services/**: High-level service layer coordinating controllers and models
  - `SizingServices/`: Business services for sizing operations

- **Headers/**: Public interface headers, organized by module (`SizingControllers/`, `OciControllers/`, `OciRawControllers/`)
- **Tests/**: GoogleTest unit tests mirroring `Models/` structure
- **Settings/**: Configuration
  - `.Cmake/`: CMake modules (platform detection, test config, build routing)
  - `.Json/`: Runtime config (`SizingMain.json`, `globalDependencies.json`)
- **Vendors/**: Third-party dependencies installed via `./processMake.sh` or `Shells/installVendor.sh`

### Platform Abstraction

Platform-specific code uses abstract base classes (Prototype pattern) with concrete implementations per platform, selected via `#ifdef __linux__` / `#elif defined(_WIN32) || defined(_WIN64)`. Example: `PCAPPrototype` → `LinuxPCAP` / `WindowsNpcapPCAP`.

### Dependencies

Managed via `Settings/.Json/globalDependencies.json`, installed to `Vendors/`:

| Library | Platform | Purpose |
|---------|----------|---------|
| libpcap | Linux | Packet capture |
| nPcap + WinDivert | Windows | Packet capture |
| cJSON | Both | JSON parsing |
| Oracle Instant Client + ocilib | Linux | Database connectivity |

### Adding New Models

1. Create `Headers/` and `Sources/` subdirectories under `Models/<NewModel>/`
2. Add `CMakeLists.txt` following an existing model as a pattern
3. Update `Models/CMakeLists.txt` to include the new subdirectory
4. Add corresponding tests in `Tests/Models/<NewModel>/`
5. Update `Tests/CMakeLists.txt`
