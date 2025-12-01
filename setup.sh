#!/bin/bash

# This script installs all necessary C++ development dependencies for the "observer" project,
# clones required external libraries, and initiates the CMake build process.

echo "Starting C++ development dependencies installation and build process..."

# 1. Update package lists
echo -e "\n--- Updating package lists ---"
sudo apt update -y

# 2. Install Build System, Basic Tools, and clang-format
echo -e "\n--- Installing basic development tools (Compilers, CMake, Ninja, Ccache, GDB) and clang-format ---"
# Added 'clang-format' for automated code style enforcement.
sudo apt install build-essential gdb cmake ninja-build ccache clang-format -y

# 3. Install Poppler-CPP and TinyXML2
echo -e "\n--- Installing Poppler and TinyXML2 development libraries ---"
sudo apt install libpoppler-cpp-dev libtinyxml2-dev -y

# 4. Clone FTXUI library
echo -e "\n--- Cloning FTXUI library ---"

FTXUI_DIR="external/FTXUI" 

if [ ! -d "$FTXUI_DIR" ]; then
    # Ensure 'git' is installed
    if ! command -v git &> /dev/null; then
        echo "Git is not installed. Installing Git..."
        sudo apt install git -y
    fi
    
    # Clone the FTXUI library
    echo "Cloning FTXUI to $FTXUI_DIR..."
    mkdir -p external 
    git clone https://github.com/ArthurSonzogni/FTXUI "$FTXUI_DIR"
else
    echo "FTXUI directory already exists. Skipping cloning."
fi

# 5. Build Project
echo -e "\n--- Starting CMake Configuration and Build (Ninja) ---"

# Get current directory and define build path
PROJECT_DIR=$(pwd)
BUILD_DIR="${PROJECT_DIR}/build"

# Clean previous build
echo "Cleaning up previous build directory..."
rm -rf "$BUILD_DIR"

# Create and enter the build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake configuration (using Ninja generator). 
# This will generate the 'format' target based on the updated CMakeLists.txt.
echo "Running CMake configuration..."
if cmake .. -G Ninja; then
    # Run Ninja build
    echo "Running Ninja build..."
    if ninja; then
        echo -e "\n--- Build successful! ---"
        echo "The executable 'observer' should be located in the 'build' folder."
    else
        echo -e "\n--- Build FAILED ---\n"
        echo "Compilation failed. Check C++ source files for errors."
    fi
else
    echo -e "\n--- CMake configuration FAILED ---\n"
    echo "Configuration failed. Check your CMakeLists.txt file for errors."
fi

cd "$PROJECT_DIR" # Return to project root

echo -e "\nInstallation and Build process complete."

