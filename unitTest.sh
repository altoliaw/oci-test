#!/bin/bash
# Dependency process
# Traversal of globalDependencies.json
Vendors="Vendors"
source $(pwd)/Shells/installVendor.sh && \
dependenciesTraversal $(pwd)/Settings/.Json/globalDependencies.json $(pwd)/$Vendors/.$Vendors.json

ORANGE='\033[0;33m'
NC='\033[0m'

# Cmake process
if [[ "$1" = "--remove" ]]; then
	rm -rf build
	printf "${ORANGE}The folder, \"build\", directory has been removed.${NC}\n"
fi
mkdir -p build
cmake -S . -B build
cmake --build build
cd build && ctest --verbose --rerun-failed --output-on-failure
cd ..