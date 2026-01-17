#!/bin/bash

# yfinance_cpp installation script

set -e  # Exit immediately if a command exits with a non-zero status

echo "yfinance_cpp - Yahoo Finance C++ Library Installation Script"
echo "=============================================================="

# Check if we're on a supported platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
else
    echo "Unsupported platform: $OSTYPE"
    exit 1
fi

echo "Detected platform: $PLATFORM"

# Install dependencies
echo "Installing dependencies..."

if [[ "$PLATFORM" == "linux" ]]; then
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y cmake build-essential libcurl4-openssl-dev nlohmann-json3-dev
    elif command -v yum &> /dev/null; then
        sudo yum install -y cmake gcc gcc-c++ make libcurl-devel json-devel
    elif command -v pacman &> /dev/null; then
        sudo pacman -S cmake gcc base-devel curl nlohmann-json
    else
        echo "Unsupported package manager. Please install cmake, gcc, make, libcurl, and nlohmann-json manually."
        exit 1
    fi
elif [[ "$PLATFORM" == "macos" ]]; then
    if command -v brew &> /dev/null; then
        brew install cmake nlohmann-json curl
    else
        echo "Please install Homebrew (https://brew.sh/) or install dependencies manually."
        exit 1
    fi
fi

# Build the project
echo "Building yfinance_cpp library..."

if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

mkdir build
cd build

cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Build completed successfully!"

echo ""
echo "To use the library in your projects:"
echo "- Link with: -lyfinance_cpp -lcurl"
echo "- Include headers from: $(pwd)/../include"
echo "- Link libraries from: $(pwd)"
echo ""
echo "Example compilation:"
echo "  g++ -std=c++17 your_program.cpp -o your_program -L$(pwd) -I../include -lyfinance_cpp -lcurl"
echo ""

cd ..

echo "Installation completed!"
echo "The library files are located in the build/src directory:"
ls -la build/src/