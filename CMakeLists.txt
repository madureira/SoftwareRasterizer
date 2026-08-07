cmake_minimum_required(VERSION 3.25)

project(
  SoftwareRasterizer
  VERSION 0.1.0
  LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

add_executable(
  SoftwareRasterizer
  ${PROJECT_SOURCES}
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
  target_compile_options(
    SoftwareRasterizer
    PRIVATE
    /W4
  )
else()
  target_compile_options(
    SoftwareRasterizer
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
  )
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

if(EMSCRIPTEN)
  target_compile_definitions(
    SoftwareRasterizer
    PRIVATE
    ENABLE_PROFILING=0
  )

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
