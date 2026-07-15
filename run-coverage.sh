#!/usr/bin/env bash

set -euo pipefail

PRESET="gcc-coverage"
BUILD_DIR="build/${PRESET}"
REPORT_DIR="coverage-html"
INFO_FILE="coverage.info"

GCOV_TOOL="gcov-15"

# 0. Clean up (after previous runs)
rm -rf "$BUILD_DIR"

# 1. Configure and build
echo "Configuring ..."
cmake --preset "$PRESET" 

echo "Building ..."
cmake --build "$BUILD_DIR"

# 2. Reset counters to a clean baseline
echo "Zeroing previous coverage counters ..."
lcov --zerocounters --directory "$BUILD_DIR"

lcov --capture --initial --directory "$BUILD_DIR" \
    --gcov-tool "$GCOV_TOOL" \
    --output-file "${INFO_FILE}.base" \
    --ignore-errors mismatch,gcov 2>/dev/null || true

# 3. Run tests
echo "Running tests ..."
ctest --test-dir "$BUILD_DIR" --output-on-failure

# 4. Capture the coverage data
echo "Capturing coverage data ..."
lcov --capture --directory "$BUILD_DIR" \
    --gcov-tool "$GCOV_TOOL" \
    --output-file "${INFO_FILE}.run" \
    --ignore-errors mismatch,gcov

# Combine base + run results
if [[ -f "${INFO_FILE}.base" ]] then
    lcov --add-tracefile "${INFO_FILE}.base" \
        --add-tracefile "${INFO_FILE}.run" \
        --output-file "$INFO_FILE" \
        --ignore-errors mismatch
else 
    cp "${INFO_FILE}.run" "$INFO_FILE"
fi

# 5. Filter out everything that is not my sources
echo "Filtering out redundant pieces ..."
lcov --remove "$INFO_FILE" \
    '/usr/*' \
    '*/googletest/*' \
    '*/_deps/*' \
    '*/tests/*' \
    --output-file "$INFO_FILE" \
    --ignore-errors unused

# 6. Generate an HTML report
echo "Generating an HTML report ..."
genhtml "$INFO_FILE" \
    --output-directory "$REPORT_DIR" \
    --title "OrderBook coverage" \
    --legend --show-details

echo "Done. The report is at ${REPORT_DIR}"

rm -f "${INFO_FILE}.base" "${INFO_FILE}.run" "$INFO_FILE"