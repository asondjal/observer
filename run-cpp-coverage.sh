#!/bin/bash

set -e

find ./src ./cpp_tests ./inc -regex '.*\.\(cpp\|hpp\)$' -exec clang-format -i {} \;

REPORT_DIR="cpp-coverage"
mkdir -p "$REPORT_DIR"

gcovr -r . \
  --filter='cpp_tests/' \
  --exclude='src/' \
  --exclude='external/' \
  --txt "$REPORT_DIR/summary.txt"

find ./cpp-coverage -type f \( -name "*.html" -o -name "*.css" \) -delete

echo "Coverage-Report: $REPORT_DIR/summary.txt"