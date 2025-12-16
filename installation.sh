#!/bin/bash

# Automated setup, build and installation script for the "observer" project

set -e  # Exit immediately on error

echo "=== Project Observer: Setup & Installation ==="

# 1. Update package lists
echo -e "\n--- Updating package lists ---"
sudo apt update -y

# 2. Install build tools
echo -e "\n--- Installing development tools ---"
sudo apt install -y \
  build-essential \
  gdb \
  cmake \
  ninja-build \
  ccache \
  clang-format \
  git

# 3. Install external dependencies
echo -e "\n--- Installing external libraries ---"
sudo apt install -y \
  libpoppler-cpp-dev \
  libtinyxml2-dev

# 4. Clone FTXUI if not present
echo -e "\n--- Checking FTXUI dependency ---"

FTXUI_DIR="external/FTXUI"

if [ ! -d "$FTXUI_DIR" ]; then
  echo "Cloning FTXUI..."
  mkdir -p external
  git clone https://github.com/ArthurSonzogni/FTXUI "$FTXUI_DIR"
else
  echo "FTXUI already present. Skipping clone."
fi

# 5. Configure build
echo -e "\n--- Configuring project with CMake (Ninja) ---"

PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
  echo "Removing previous build directory..."
  rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -G Ninja

# 6. Build
echo -e "\n--- Building project ---"
ninja

# 7. Install (system-wide)
echo -e "\n--- Installing observer binary ---"
sudo ninja install

# 8. Final check
echo -e "\n--- Installation complete ---"
if command -v observer >/dev/null 2>&1; then
  echo "✔ observer successfully installed"
  echo "Run it anytime with: observer"
else
  echo "✘ Installation failed: observer not found in PATH"
fi

cd "$PROJECT_DIR"

echo -e "\n=== Setup finished successfully ==="
