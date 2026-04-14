# OCI — C++ Oracle Packet Connection Application

**Project**: OCI tools
**Author**: Nick Liao
**Started**: May 15th, 2024
**Language**: C++17
**Build System**: CMake 3.14+
**Platform**: Linux (RHEL 8.7+), Windows (MSYS2)

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Overview](#overview)
- [Architecture](#architecture)
- [Directory Structure](#directory-structure)
- [Building the Project](#building-the-project)
- [Testing](#testing)
- [Adding New Features](#adding-new-features)
- [Coding Standards](#coding-standards)
- [Dependencies](#dependencies)

---

## Prerequisites

The following tools and libraries shall be installed on the system before building. Missing prerequisites will cause the build to fail.

### Linux (RHEL 8.7 / CentOS 8)

#### System Packages

```bash
# Development tools (compiler, linker, make, etc.)
sudo yum groupinstall "Development Tools"

# CMake 3.14 or higher
sudo yum install cmake

# jq — required by all build and vendor scripts
sudo yum install jq

# wget — required to download third-party vendor archives
sudo yum install wget

# unzip — required to extract nPcap SDK and Oracle Instant Client ZIPs
sudo yum install unzip

# rpm2cpio and cpio — required to extract Oracle Instant Client RPMs
# (typically pre-installed; verify with: rpm2cpio --version)
sudo yum install rpm2cpio cpio

# flex and bison — required to build libpcap from source
sudo yum install flex bison

# autoconf, automake, libtool — required to configure libpcap
sudo yum install autoconf automake libtool
```

#### Verifying Required Tool Versions

```bash
g++ --version      # shall be GCC 8.5.0 or higher
cmake --version    # shall be 3.14 or higher
jq --version       # shall be 1.6 or higher
wget --version
rpm2cpio --version
flex --version
bison --version
```

### Windows (MinGW / MSYS2)

- MinGW-w64 toolchain with GCC 8+ and G++
- CMake 3.14+
- Git for Windows
- `jq` for Windows (available via `winget install jqlang.jq` or MSYS2)
- `wget` or `curl` for downloading vendor archives
- `unzip`

---

## Quick Start

### Production Build

```bash
# Clone the repository
git clone <repository-url>
cd <repository>

# Install vendors and build production application
./processMake.sh -r

# Executables are generated in build/Apps/
```

### Running Tests

```bash
# Run all unit tests (installs vendors automatically)
./unitTest.sh

# Clean build and run tests
./unitTest.sh --remove
```

---

## Overview

The Native Sizing project is a high-performance C++ application for traffic monitoring, PCAP file analysis, and network sizing calculations. The system integrates with Oracle databases for data persistence and provides comprehensive network traffic analysis capabilities.

### Key Features

- **Packet Capture Analysis**: Processes PCAP files for network traffic analysis
- **Cross-Platform Support**: Runs on Linux (libpcap) and Windows (nPcap/WinDivert)
- **Oracle Database Integration**: Connectivity via both raw OCI API and the ocilib wrapper
- **Modular Architecture**: Layered MVC design with clear separation between models, controllers, and applications
- **Comprehensive Testing**: GoogleTest-based unit testing framework

---

## Architecture

The project follows a layered **MVC (Model-View-Controller)** architecture:

```
┌─────────────────────────────────────────────┐
│              Applications (Apps/)           │
│         Entry points for executables        │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│            Controllers (Sources/)           │
│       Business logic and orchestration      │
└────────────────┬────────────────────────────┘
                 │
┌────────────────▼────────────────────────────┐
│               Models (Models/)              │
│         Reusable libraries and utilities    │
└─────────────────────────────────────────────┘
```

### Platform Abstraction

Platform-specific implementations are selected at compile time via conditional compilation (`#ifdef __linux__` / `#elif defined(_WIN32) || defined(_WIN64)`). Abstract base classes define the interface; concrete classes provide the platform-specific behavior (e.g., `PCAPPrototype` → `LinuxPCAP` / `WindowsNpcapPCAP`).

---

## Directory Structure

| Directory | Purpose |
|-----------|---------|
| `Apps/` | Executable entry points (`Sizing/`, `Oci/`, `OciRaw/`) |
| `Sources/` | Business logic controllers (`SizingControllers/`, `OciControllers/`, `OciRawControllers/`) |
| `Models/` | Reusable libraries (`Commons/`, `FileParsers/`, `PCAP/`, `DataModels/`, `DBUtils/`) |
| `Headers/` | Public interface headers, organized by module |
| `Services/` | High-level service layer coordinating controllers and models |
| `Tests/` | GoogleTest unit tests mirroring `Models/` structure |
| `Settings/` | CMake modules (`.Cmake/`), JSON runtime config (`.Json/`) |
| `Vendors/` | Third-party dependencies installed by the build system |
| `ExternalResource/` | Offline Oracle Instant Client ZIP archives (placed manually) |
| `Outputs/` | Runtime output files (auto-generated) |
| `Logs/` | Application logs (auto-generated) |
| `build/` | CMake build artifacts (auto-generated) |
| `Bin/` | Deployment executables (auto-generated) |

### Notable Subdirectories

- **`Apps/Sizing/`** — Main sizing application entry point
- **`Apps/Oci/`** — Oracle database test application (using ocilib wrapper)
- **`Apps/OciRaw/`** — Oracle database test application (using raw OCI API)
- **`Models/Commons/`** — Core utilities: `HashTable`, `StringImplement`, `Time`, `IOExporters`, `POSIXErrors`
- **`Models/PCAP/`** — Packet capture: `PCAPPrototype`, `LinuxPCAP`, `WindowsNpcapPCAP`, `WindowsWinDivertPCAP`
- **`Models/DBUtils/`** — `DBConnector` (Singleton), `DBSql` for batch SQL operations

---

## Building the Project

CMake is the **only** supported build system. The provided shell scripts shall be used instead of invoking `cmake` or `make` directly.

### Build Scripts

#### `processMake.sh` — Production Build

```bash
# Standard build
./processMake.sh -r

# Clean build (removes build/ first)
./processMake.sh --remove

# Clean build and reinstall all vendor dependencies
./processMake.sh --remove --vendor
```

The script performs the following operations in order:

1. Parses `Settings/.Json/globalDependencies.json` and installs missing vendors to `Vendors/`
2. Creates required runtime directories (`Outputs/`, `Logs/`, `Bin/`)
3. Detects the operating system and configures CMake (`BUILD_TEST=OFF`)
4. Builds all production code
5. On Windows (MinGW): builds both nPcap and WinDivert targets sequentially and copies WinDivert DLLs to `Bin/`

Executables are generated in `build/Apps/`.

#### `unitTest.sh` — Test Build

```bash
# Run all unit tests
./unitTest.sh

# Clean build and run all tests
./unitTest.sh --remove
```

The script installs vendors if needed, configures CMake (`BUILD_TEST=ON`), compiles test code together with GoogleTest, and runs all tests with verbose output.

### Manual CMake Invocation (Advanced)

```bash
# Testing mode (default)
cmake -S . -B build
cmake --build build
cd build && ctest -V

# Run a single test by name
cd build && ctest -R Test_HashTable -V

# Production mode
cmake -S . -B build -DBUILD_TEST=OFF
cmake --build build
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TEST` | `ON` | `ON` builds GoogleTest tests; `OFF` builds production executables |
| `CMAKE_BUILD_TYPE` | `Debug` | Build type passed to the compiler |
| `OPERATING_SYSTEM` | auto | Override OS detection (used on Windows: `1` = nPcap, `1.1` = WinDivert) |

---

## Testing

### Running All Tests

```bash
./unitTest.sh           # install vendors + build + run all tests
./unitTest.sh --remove  # clean build + install vendors + run all tests
```

### Running a Specific Test

```bash
cmake -S . -B build && cmake --build build
cd build && ctest -R Test_HashTable -V
```

### Test Structure

Tests mirror the `Models/` directory structure:

```
Tests/
  Models/
    Commons/      ← tests for Models/Commons/
    FileParsers/  ← tests for Models/FileParsers/
  Shells/         ← tests for shell script utilities
```

### Adding New Tests

1. Create `Tests/Models/<NewModel>/Test_<Name>.cpp`
2. Follow the GoogleTest naming convention (`Test_<ModuleName>`)
3. Update `Tests/CMakeLists.txt` to include the new test target
4. Run `./unitTest.sh` to verify

---

## Adding New Features

When adding new features, the following decision tree shall be followed to determine the correct location:

| Characteristic | Location |
|---------------|----------|
| Standalone application (has its own `main()`) | `Apps/<Name>/` |
| Business logic or orchestration | `Sources/<Name>Controllers/` |
| Reusable utility or library | `Models/<Category>/` |
| High-level service coordinating controllers | `Services/<Name>Services/` |

### Adding a New Model

1. Create `Models/<NewModel>/Headers/` and `Models/<NewModel>/Sources/` subdirectories
2. Add `Models/<NewModel>/CMakeLists.txt` following an existing model as a pattern
3. Update `Models/CMakeLists.txt` to include the new subdirectory (`add_subdirectory`)
4. Add corresponding tests in `Tests/Models/<NewModel>/`
5. Update `Tests/CMakeLists.txt`

### Design Considerations

- **Platform compatibility**: if platform-specific behavior is required, an abstract base class (Prototype pattern) shall be defined with separate Linux and Windows implementations
- **Error handling**: the `Commons::POSIXErrors` enum shall be used for all return values; `POSIXErrors::OK` is returned on success
- **Memory management**: pointers shall be set to `nullptr` after deletion; RAII principles shall be applied
- **Testing**: all `Models/` code shall have corresponding unit tests

---

## File Placement Guide

The following rules shall be applied when placing new source files into the project.
Each file type has a designated location determined by its role in the MVC architecture.

### Header Files (`.hpp`)

Header files shall be placed in the `Headers/` directory, **not** alongside their source files.
The subdirectory under `Headers/` shall mirror the module it belongs to.

| Module | Header Location |
|--------|----------------|
| Sizing controllers | `Headers/SizingControllers/` |
| OCI controllers | `Headers/OciControllers/` |
| OCI Raw controllers | `Headers/OciRawControllers/` |
| Models | `Models/<Category>/Headers/` |

> Headers for `Models/` are an exception — they reside inside the model's own subdirectory
> (e.g., `Models/Commons/Headers/`), not in the top-level `Headers/` directory.

### Source Files (`.cpp`)

| File Type | Location |
|-----------|----------|
| Application entry point (has `main()`) | `Apps/<AppName>/` |
| Controller (business logic / orchestration) | `Sources/<Name>Controllers/` |
| Model (reusable library / utility) | `Models/<Category>/Sources/` |
| Service (coordinates controllers + models) | `Services/<Name>Services/` |
| Unit test | `Tests/Models/<Category>/` |

### Decision Flow

Use the following flow to decide where a new file belongs:

```
Does the file contain main()?
  YES → Apps/<AppName>/

Does the file coordinate controllers and models at a high level?
  YES → Services/<Name>Services/

Does the file contain business logic or orchestration?
  YES → Sources/<Name>Controllers/    (header → Headers/<Name>Controllers/)

Is the file a reusable utility / data structure / library?
  YES → Models/<Category>/Sources/    (header → Models/<Category>/Headers/)

Is the file a unit test?
  YES → Tests/Models/<Category>/
```

### Concrete Examples

| New component | Source file location | Header file location |
|---------------|---------------------|---------------------|
| New sizing controller | `Sources/SizingControllers/NewController.cpp` | `Headers/SizingControllers/NewController.hpp` |
| New OCI Raw controller | `Sources/OciRawControllers/NewOciRaw.cpp` | `Headers/OciRawControllers/NewOciRaw.hpp` |
| New common utility | `Models/Commons/Sources/NewUtil.cpp` | `Models/Commons/Headers/NewUtil.hpp` |
| New DB utility | `Models/DBUtils/Sources/NewDb.cpp` | `Models/DBUtils/Headers/NewDb.hpp` |
| New application entry | `Apps/NewApp/main.cpp` | — |
| Unit test for utility | `Tests/Models/Commons/Test_NewUtil.cpp` | — |

### CMakeLists.txt Updates Required

After placing new files, the following CMake files shall be updated:

| Change | File to update |
|--------|---------------|
| New model added | `Models/CMakeLists.txt` — add `add_subdirectory` |
| New controller added | `Sources/CMakeLists.txt` — add source file or subdirectory |
| New application added | `Apps/CMakeLists.txt` — add executable target |
| New test added | `Tests/CMakeLists.txt` — add test target |

---

## Coding Standards

All code contributions shall follow the conventions defined in `CODING_STANDARDS.md`.

### Quick Reference

```cpp
// Header file template
#pragma once
/** @file FileName.hpp
 * Brief description
 *
 * @author Nick, Liao
 * @date YYYY/MM/DD
 */
```

```cpp
// Function documentation (IEEE style)
/**
 * Obtaining the value from the hash table by using the column name;
 * if the element does not exist, the returned value will be 0x0
 *
 * @param columnName [char*] The column name
 * @param value [void**] The address of the value
 * @return [char] The number of hit elements; if the value is 0x0, no element is found
 */
```

### Naming Conventions

| Construct | Convention | Example |
|-----------|-----------|---------|
| Class / Namespace | PascalCase | `HashTable`, `Commons` |
| Function / Variable | camelCase | `getValueByName`, `columnName` |

### Git Commit Format (Conventional Commits)

```
type(scope): brief description in lowercase

Examples:
feat(OCI): Add fetch data function
fix(oracle): Adding the OCI test procedure for the OciRawController
docs(readme): Update prerequisites section
```

---

## Dependencies

### Vendor Dependencies (auto-installed by build scripts)

| Library | Platform | Version | Purpose |
|---------|----------|---------|---------|
| libpcap | Linux | 1.9.1 | Packet capture (built from source) |
| cJSON | Both | latest | JSON parsing |
| nPcap SDK | Windows | 1.13 | Windows packet capture |
| Oracle Instant Client (OCI) | Linux | 19.10 | Oracle DB connectivity (RPM) |
| ocilib | Linux | latest | OCI C++ wrapper (built from source) |

Additional Oracle Instant Client versions (10.1.0.5, 12.1.0.2, 12.2.0.1, 19.30.0.0) are supported via offline ZIP archives placed in `ExternalResource/`. The appropriate version shall be selected in `Settings/.Json/globalDependencies.json`.

### Build Dependencies (shall be installed manually)

| Tool | Minimum Version | Required For |
|------|----------------|-------------|
| GCC / G++ | 8.5.0 | C++17 compilation |
| CMake | 3.14 | Build orchestration |
| Git | 2.x | Source control; GoogleTest fetch |
| jq | 1.6 | Vendor install scripts |
| wget | any | Downloading vendor archives |
| unzip | any | Extracting ZIP archives |
| rpm2cpio + cpio | any | Extracting Oracle RPM packages |
| flex + bison | any | Building libpcap from source |
| autoconf + automake + libtool | any | Configuring libpcap from source |

GoogleTest is fetched automatically by CMake during the test build; no manual installation is required.

---

## Adding a Third-Party Dependency

Third-party dependencies are defined in `Settings/.Json/globalDependencies.json` and
installed automatically by `processMake.sh` and `unitTest.sh`. Each entry in the
`dependencies` array shall follow the schema below.

### Entry Schema

```json
{
    "name": "<VendorName>",
    "download": "<shell command to fetch the source>",
    "command": "<shell command to build or extract>",
    "includes": ["<glob pattern(s) for header files>"],
    "libs": ["<glob pattern(s) for library files>"],
    "others": ["<optional post-install shell commands>"],
    "reference": "<upstream URL for reference>",
    "remove": "rm -rf {{name}}",
    "windows": true,
    "linux": true
}
```

### Field Reference

| Field | Required | Description |
|-------|----------|-------------|
| `name` | Yes | Vendor directory name; becomes `Vendors/<name>/`. Used as `{{name}}` in commands. |
| `download` | Yes | Shell command executed from inside `Vendors/<name>/` to fetch the source. |
| `command` | Yes | Shell command executed from inside `Vendors/<name>/` to build or extract. |
| `includes` | Yes | Glob patterns for header files; matched files are copied to `Vendors/<name>/Includes/`. |
| `libs` | Yes | Glob patterns for library files; matched files are copied to `Vendors/<name>/Libs/`. |
| `others` | No | Additional shell commands run after `includes`/`libs` are copied (e.g., creating symlinks). |
| `reference` | Yes | Upstream URL for documentation or download page. |
| `remove` | Yes | Command to delete the vendor directory (typically `rm -rf {{name}}`). |
| `windows` | Yes | `true` if the dependency shall be installed on Windows builds. |
| `linux` | Yes | `true` if the dependency shall be installed on Linux builds. |

### Available Placeholders

| Placeholder | Expands to |
|-------------|-----------|
| `{{name}}` | The value of the `name` field (e.g., `myLib`) |
| `{{projectVendors}}` | Absolute path to the `Vendors/` directory |
| `{{projectRootDir}}` | Absolute path to the project root directory |

### Dependency Types

#### Online Dependency (downloaded at build time)

Used when the library is fetched from a public URL or Git repository:

```json
{
    "name": "myLib",
    "download": "wget -O {{name}}/mylib.tar.gz https://example.com/mylib.tar.gz",
    "command": "tar zxvf mylib.tar.gz && cd mylib && ./configure --prefix={{projectVendors}}/{{name}} && make && make install && cd ..",
    "includes": ["include/*"],
    "libs": ["lib/*"],
    "others": [],
    "reference": "https://example.com/mylib",
    "remove": "rm -rf {{name}}",
    "windows": false,
    "linux": true
}
```

#### Offline Dependency (from `ExternalResource/`)

Used when the library archive is provided manually and placed in `ExternalResource/<version>/`:

```json
{
    "name": "myLib_1.0.0",
    "download": "cp -pr {{projectRootDir}}/ExternalResource/1.0.0/* {{name}}",
    "command": "unzip mylib-1.0.0.zip",
    "includes": ["mylib_1_0/sdk/include/*"],
    "libs": ["mylib_1_0/*.so", "mylib_1_0/*.so.*"],
    "others": ["cd {{projectRootDir}}/Vendors/myLib_1.0.0/Libs && ln -sf libmylib.so.1.0 libmylib.so"],
    "reference": "https://example.com/mylib/downloads",
    "remove": "rm -rf {{name}}",
    "windows": false,
    "linux": true
}
```

> Offline archives shall be placed in `ExternalResource/<version>/` before running the build scripts.

### Installation Result

After the build script processes an entry, the following structure is created:

```
Vendors/
  <name>/
    Includes/   ← header files matched by "includes"
    Libs/       ← library files matched by "libs"
```

CMake modules in `Settings/.Cmake/` resolve `Vendors/<name>/Includes/` and
`Vendors/<name>/Libs/` as include and link paths for the build targets.

---

## Additional Resources

- **CODING_STANDARDS.md** — Detailed coding conventions and IEEE writing style reference
- **Settings/.Json/globalDependencies.json** — Vendor dependency definitions
- **Settings/.Cmake/** — CMake module files for platform detection and build routing

---

**Last Updated**: 2026/04/10
**Project Start**: 2024/05/15
