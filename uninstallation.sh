#!/bin/bash

# This script removes the files installed by 'installation.sh'.

set -e # Exit immediately if an error occurs

echo "=== Project Observer: Starting uninstallation ==="

# 1. Set path to the manifest file
# Ensure that this path is correct, based on your installation.sh
PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
MANIFEST_FILE="${BUILD_DIR}/install_manifest.txt"

# Check if the manifest file exists
if [ ! -f "$MANIFEST_FILE" ]; then
    echo "Error: Manifest-file '$MANIFEST_FILE' not found."
    echo "Ensure that project was created and installed by using 'installation.sh'."
    exit 1
fi

echo "Manifest file found. Starting removal of installed files..."

# 2. Loop through the manifest file and delete every listed file
# Reading the manifest file MUST be done with root privileges,
# because installed files (e.g., in /usr/local/bin) can only be removed with sudo.

if sudo xargs -a "$MANIFEST_FILE" rm -fv; then
    echo "✔ All flies were deleted successfully!"
    # 3. Cleanup (Deleting the manifest file itself)
    rm -f "$MANIFEST_FILE"
else
    echo "✘ ERROR during uninstallation of some files."
fi

echo -e "\n=== Uninstallation complete ==="