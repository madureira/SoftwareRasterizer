cmake_minimum_required(VERSION 3.25)

project(
  SoftwareRasterizer
  VERSION 0.1.0
  LANGUAGES C
)

# Hand-written assembly in this project uses GAS/AT&T syntax (.S) on
# macOS and Linux, assembled by clang/gcc's GNU-compatible assembler
# under CMake's "ASM" language. On Windows this project builds with
# MSVC (see CMakePresets.json's "windows-debug"/"windows-release",
# which use the "Visual Studio 17 2022" generator), and cl.exe cannot
# assemble GAS syntax -- so Windows assembly is written in Intel/MASM
# syntax (.asm) instead, assembled via CMake's "ASM_MASM" language
# (ml64.exe).
if(WIN32)
  enable_language(ASM_MASM)
else()
  enable_language(ASM)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ENABLE_PROFILING "Enable frame profiler" OFF)
option(ENABLE_SIMD      "Enable SIMD optimizations (SSE2 on x86-64, NEON on ARM64)" OFF)

# SDL3 cannot be included via add_subdirectory for Emscripten builds;
# Emscripten ships its own SDL3 port activated through linker flags.
if(NOT EMSCRIPTEN)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TEST OFF CACHE BOOL "" FORCE)
  set(SDL_TESTS OFF CACHE BOOL "" FORCE)
  set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
  add_subdirectory(vendors/SDL3)
endif()

file(
  GLOB_RECURSE PROJECT_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(
  GLOB_RECURSE PROJECT_HEADERS
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Per-OS/per-architecture assembly is grouped by OS under
# src/platform/<os>/ (e.g. src/platform/macos/, src/platform/linux/,
# src/platform/windows/), with the architecture encoded in the file
# name (<name>_arm64 / <name>_x86_64). Selection must key off both OS
# and architecture together -- a glob keyed on OS alone would match
# multiple architectures' files and duplicate symbols at link time.
# The file extension also differs by OS: .S (GAS) on macOS/Linux,
# .asm (MASM) on Windows.
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(ASM_OS "macos")
  set(ASM_EXT "S")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(ASM_OS "linux")
  set(ASM_EXT "S")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(ASM_OS "windows")
  set(ASM_EXT "asm")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|ARM64)$")
  set(ASM_ARCH "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
  set(ASM_ARCH "x86_64")
endif()

set(PROJECT_ASM_SOURCES "")
if(ASM_OS AND ASM_ARCH)
  file(
    GLOB PROJECT_ASM_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${ASM_OS}/*_${ASM_ARCH}.${ASM_EXT}"
  )
endif()

add_executable(
  SoftwareRasterizer
  ${PROJECT_SOURCES}
  ${PROJECT_ASM_SOURCES}
  ${PROJECT_HEADERS}
)

target_include_directories(
  SoftwareRasterizer
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(
  SoftwareRasterizer
  SYSTEM PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/vendors"
)

target_precompile_headers(
  SoftwareRasterizer
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core/types.h"
)

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PREFIX "Source Files"
  FILES
  ${PROJECT_SOURCES}
  ${PROJECT_HEADERS}
)

if(NOT EMSCRIPTEN)
  target_link_libraries(
    SoftwareRasterizer
    PRIVATE
    SDL3::SDL3-static
  )
endif()

set_target_properties(
  SoftwareRasterizer
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}"
)

if(MSVC)
  # Scoped to C only: ml64.exe (ASM_MASM) is a separate tool from cl.exe
  # and does not understand these flags -- passing them unscoped produces
  # "MASM : warning A4008: invalid command-line option value" noise when
  # PROJECT_ASM_SOURCES (the .asm file) is part of this target.
  target_compile_options(
    SoftwareRasterizer
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:/W4>
    $<$<COMPILE_LANGUAGE:C>:/arch:SSE2>
    "$<$<COMPILE_LANGUAGE:C>:/d1trimfile:${CMAKE_SOURCE_DIR}\\>"
  )
else()
  target_compile_options(
    SoftwareRasterizer
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/="
  )
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    target_compile_options(SoftwareRasterizer PRIVATE -msse2)
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Darwin"))
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(SoftwareRasterizer PRIVATE
      -fsanitize=address,undefined
      -fno-omit-frame-pointer
    )
    target_link_options(SoftwareRasterizer PRIVATE
      -fsanitize=address,undefined
    )
  endif()
endif()

if(NOT EMSCRIPTEN)
  add_subdirectory(tests)
endif()

if(EMSCRIPTEN OR NOT ENABLE_SIMD)
  target_compile_definitions(SoftwareRasterizer PRIVATE ENABLE_SIMD=0)
else()
  target_compile_definitions(SoftwareRasterizer PRIVATE ENABLE_SIMD=1)
endif()

if(EMSCRIPTEN OR NOT ENABLE_PROFILING)
  target_compile_definitions(
    SoftwareRasterizer
    PRIVATE
    ENABLE_PROFILING=0
  )
elseif(ENABLE_PROFILING)
  target_compile_definitions(
    SoftwareRasterizer
    PRIVATE
    ENABLE_PROFILING=1
  )
endif()

if(EMSCRIPTEN)
  set_target_properties(
    SoftwareRasterizer
    PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"
  )

  target_link_options(
    SoftwareRasterizer
    PRIVATE
    "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/index.html"
    "SHELL:--js-library ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/platform_web.js"
    "SHELL:-sALLOW_MEMORY_GROWTH=1"
    "SHELL:-sMAX_WEBGL_VERSION=2"
    "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts@/assets/fonts"
    "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/config.ini@/config.ini"
  )

  set_property(
    TARGET SoftwareRasterizer
    APPEND PROPERTY LINK_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
  )
endif()
