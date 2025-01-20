#!/bin/bash

# Lina uses LLVM-19.1.7. Download from github releases for your platform and pass it's directory
# into this script. Check the Github README for more information.

# XCode might ignore CXX_COMPILER and similar flags.
# That's why we use this script to always build Lina and related Game projects with correct compiler,
# compiler flags and alike. Always use this to generate project files.

# Note: using -lc++ expects to have LLVM/lib/c++ folder where libc++ libraries will be in.
# This is unfortunately not the case when you download from github releases...

if [ -z "$1" ]; then
    echo "Error: Please provide the path to your LLVM installation as an argument."
    echo "Usage: $0 /path/to/llvm"
    exit 1
fi

# Set up paths
BUILD_DIR="build_x64"
GENERATOR="Xcode"
TOOLCHAIN_FILE="CMake/Toolchain.cmake"
LLVM_DIR="$1"
CCOMPILER="$LLVM_DIR/bin/clang"
CPPCOMPILER="$LLVM_DIR/bin/clang++"
AR="$LLVM_DIR/bin/llvm-ar"
RANLIB="$LLVM_DIR/bin/llvm-ranlib"
LINKER="$LLVM_DIR/bin/lld"
LIBCXX_INCLUDE="$LLVM_DIR/include/c++/v1"
LIBCXX_LIB="$LLVM_DIR/lib"

# Check if the LLVM directory exists and contains the necessary tools
if [ ! -d "$LLVM_DIR" ] || [ ! -f "$CCOMPILER" ] || [ ! -f "$CPPCOMPILER" ]; then
    echo "Error: LLVM installation not found or incomplete at $LLVM_DIR."
    exit 1
fi

echo "Preventing LLVM dynamic libraries from Apple privacy protection..."
sudo xattr -r -d com.apple.quarantine "$LIBCXX_LIB"/*.dylib
sudo xattr -r -d com.apple.quarantine "$LLVM_DIR"/bin/clang
sudo xattr -r -d com.apple.quarantine "$LLVM_DIR"/bin/clang++
sudo codesign -s - "$LLVM_DIR"/bin/*
sudo codesign -s - "$LLVM_DIR"/lib/*

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

export CXXFLAGS="-std=c++23 -stdlib=libc++ -nostdinc++ -I$LIBCXX_INCLUDE"
export LDFLAGS="-stdlib=libc++ -L$LIBCXX_LIB -lc++abi -Wl,-rpath,$LIBCXX_LIB"
export CC="$CCOMPILER"
export CXX="$CPPCOMPILER"
unset CMAKE_C_COMPILER
unset CMAKE_CXX_COMPILER

# -fms-compatibility-version=19.1

# Run CMake
cmake \
      -DCMAKE_AR="$AR" -DCMAKE_RANLIB="$RANLIB" \
      -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
      -DCMAKE_C_COMPILER="$CCOMPILER" -DCMAKE_CXX_COMPILER="$CPPCOMPILER" \
      -DCMAKE_CXX_FLAGS="$CXXFLAGS" -DCMAKE_C_FLAGS="$CFLAGS" \
      -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
      -B "$BUILD_DIR" -G "$GENERATOR" \

# -DCMAKE_C_LINKER="$LINKER" -DCMAKE_CXX_LINKER="$LINKER" \

echo "Generation complete."

# Build the project
# cmake --build "$BUILD_DIR" --config Debug --verbose

