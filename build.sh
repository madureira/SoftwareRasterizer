#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PLATFORM="${1:-}"
BUILD_TYPE="${2:-}"

PROJECT_NAME="SoftwareRasterizer"

print_usage() {
    cat <<EOF
Usage:
  ./build.sh init
  ./build.sh mac debug
  ./build.sh mac release
  ./build.sh linux debug
  ./build.sh linux release
  ./build.sh web

Examples:
  ./build.sh init
  ./build.sh mac debug
  ./build.sh mac release
  ./build.sh web
EOF
}

fail() {
    echo "Error: $1" >&2
    exit 1
}

command_exists() {
    command -v "$1" >/dev/null 2>&1
}

init_submodules() {
    echo "Initializing git submodules..."
    git -C "$PROJECT_ROOT" submodule update --init --recursive
    echo "Submodules initialized successfully."
}

normalize_build_type() {
    case "$1" in
    debug)
        echo "Debug"
        ;;
    release)
        echo "Release"
        ;;
    *)
        fail "Invalid build type '$1'. Expected 'debug' or 'release'."
        ;;
    esac
}

build_desktop() {
    local requested_build_type="$1"
    local requested_platform="$2"

    local cmake_build_type
    local build_directory
    local executable_path

    [[ -f "${PROJECT_ROOT}/vendors/SDL3/CMakeLists.txt" ]] ||
        fail "SDL3 submodule not found. Run './build.sh init' first."

    cmake_build_type="$(normalize_build_type "$requested_build_type")"
    build_directory="${PROJECT_ROOT}/build/${requested_build_type}"
    executable_path="${build_directory}/${PROJECT_NAME}"

    case "$requested_platform" in
    mac)
        [[ "$(uname -s)" == "Darwin" ]] ||
            fail "The 'mac' target must be built on macOS."
        ;;

    linux)
        [[ "$(uname -s)" == "Linux" ]] ||
            fail "The 'linux' target must be built on Linux."
        ;;

    *)
        fail "Unsupported desktop platform '$requested_platform'."
        ;;
    esac

    echo "Configuring ${requested_platform} ${cmake_build_type} build..."

    cmake \
        -S "${PROJECT_ROOT}" \
        -B "${build_directory}" \
        -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE="${cmake_build_type}" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

    echo "Building ${requested_platform} ${cmake_build_type}..."

    cmake \
        --build "${build_directory}" \
        --config "${cmake_build_type}" \
        --target "${PROJECT_NAME}" \
        --parallel

    [[ -f "$executable_path" ]] ||
        fail "Build completed, but the executable was not found at: ${executable_path}"

    [[ -x "$executable_path" ]] ||
        fail "The generated file is not executable: ${executable_path}"

    echo
    echo "Build completed successfully."
    echo "Executable: ${executable_path}"
    echo
    echo "Running ${PROJECT_NAME}..."
    echo "----------------------------------------"

    cd "$PROJECT_ROOT"
    "$executable_path"
}

build_web() {
    local build_directory="${PROJECT_ROOT}/build/web"

    [[ -f "${PROJECT_ROOT}/vendors/SDL3/CMakeLists.txt" ]] ||
        fail "SDL3 submodule not found. Run './build.sh init' first."

    command_exists emcmake ||
        fail "The 'emcmake' command was not found. Activate the Emscripten SDK first."

    command_exists emcc ||
        fail "The 'emcc' command was not found. Activate the Emscripten SDK first."

    echo "Configuring WebAssembly Release build..."

    emcmake cmake \
        -S "${PROJECT_ROOT}" \
        -B "${build_directory}" \
        -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE="Release"

    echo "Building WebAssembly Release..."

    cmake \
        --build "${build_directory}" \
        --config "Release" \
        --parallel

    echo
    echo "Web build completed successfully."
    echo "Output directory: ${build_directory}"

    if [[ -f "${build_directory}/${PROJECT_NAME}.html" ]]; then
        echo "Entry point: ${build_directory}/${PROJECT_NAME}.html"
    fi
}

command_exists cmake ||
    fail "CMake was not found."

case "$PLATFORM" in
init)
    init_submodules
    ;;

mac | linux)
    [[ -n "$BUILD_TYPE" ]] || {
        print_usage
        exit 1
    }

    build_desktop "$BUILD_TYPE" "$PLATFORM"
    ;;

web)
    [[ -z "$BUILD_TYPE" ]] ||
        fail "The web target does not accept a build type argument. Use: ./build.sh web"

    build_web
    ;;

-h | --help | help | "")
    print_usage
    ;;

*)
    print_usage
    fail "Unknown platform '$PLATFORM'."
    ;;
esac
