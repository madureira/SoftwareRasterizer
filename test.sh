#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/debug"
TEST_BINARY="${BUILD_DIR}/SoftwareRasterizerTests"

[[ -f "${PROJECT_ROOT}/vendors/SDL3/CMakeLists.txt" ]] ||
    { echo "Error: SDL3 submodule not found. Run './build.sh init' first." >&2; exit 1; }

cmake \
    -S "${PROJECT_ROOT}" \
    -B "${BUILD_DIR}" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    --log-level=WARNING

cmake \
    --build "${BUILD_DIR}" \
    --config "Debug" \
    --target SoftwareRasterizerTests \
    --parallel

echo ""
echo "Running tests..."
echo "----------------------------------------"

"${TEST_BINARY}"
