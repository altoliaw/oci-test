#!/bin/bash

# Considering the dependency of the native sizing by the traversal of the structure in the file, globalDependencies.json
Vendors="Vendors"

# Parsing the parameters into the array; these are defined as below:
# --remove | --r | -r
# The array mentioned previously, is for triggering the parameter's processing.
ParameterArray=("$@") # Putting all parameters into the array
for i in "${!ParameterArray[@]}"; do  # ${#ParameterArray[@]} implies the number of element indexes
	Param="${ParameterArray[$i]}"
	case "$Param" in
		--remove|--r|-r)
			rm -rf build
			echo -e "Build directory has been removed."
			;;
		--vendor|--v|-v)
			rm -rf $Vendors
			;;
	esac
done

# Considering the dependency of the native sizing by the traversal of the structure in the file, globalDependencies.json
mkdir -p ExternalResource # Creating the folder for external resources
source $(pwd)/Shells/installVendor.sh && \
dependenciesTraversal $(pwd)/Settings/.Json/globalDependencies.json $(pwd)/$Vendors/.$Vendors.json

# Folders' generations
mkdir -p Outputs
mkdir -p Logs
mkdir -p Bin		# Creating the folder for execution

# Cmake process
OsType="$(uname -s | tr '[:upper:]' '[:lower:]')" # Obtaining the kernel type string and then translating the string with the lower case

# A generic function to configure and build the project.
# It accepts an optional OS flag. If a flag is provided, it's passed to CMake;
# otherwise, CMake's internal OS detection is used.
build_project() {
    local os_flag=$1
    local cmake_args=("-S" "." "-B" "build" "-DBUILD_TEST=OFF" "-DCMAKE_BUILD_TYPE=Debug")

    if [[ -n "$os_flag" ]]; then
        echo "--- Building with explicit OS flag: $os_flag ---"
        cmake_args+=("-DOPERATING_SYSTEM=$os_flag")
    else
        echo "--- Building with default OS detection (for Linux) ---"
    fi

    cmake "${cmake_args[@]}"
    cmake --build build
}

# 1. Create build directory
mkdir -p build

# 2. Execute build(s) and post-processing based on OS
if [[ "$OsType" = "linux" ]]; then
    build_project "" # Call with no flag to trigger auto-detection in CMake
elif [[ "$OsType" = *"mingw"* ]]; then
    # For Windows, build both targets sequentially with a clean step in between
    echo "--- Building for Npcap ---"
    build_project "1"   # Build NPCAP

    echo "--- Cleaning build directory for next target ---"
    cmake --build build --target clean

    echo "--- Building for WinDivert ---"
    build_project "1.1" # Build WinDivert

    # Perform post-processing steps immediately after Windows builds
    echo "--- Performing post-processing for WinDivert ---"
    sc stop WinDivert >/dev/null 2>&1
    sc delete WinDivert >/dev/null 2>&1
    cp -f ./Vendors/WinDivert/Libs/WinDivert.dll ./Bin/
    cp -f ./Vendors/WinDivert/Libs/WinDivert64.sys ./Bin/
    echo -e "The post-processing on the Windows has been executed. Executables are already in Bin."
else
    echo "Unsupported OS: $OsType. Attempting default build."
    build_project ""
fi
exit
# cd build
# cp -ar ./Apps/SizingMain_Windows.exe ../Bin/
# cd ..