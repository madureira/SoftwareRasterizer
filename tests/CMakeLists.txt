file(GLOB_RECURSE TEST_IMPL_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_*.c"
)

set(SRC_FILES "")
set(SUITE_EXTERNS "")
set(SUITE_ENTRIES "")

foreach(test_file ${TEST_IMPL_FILES})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${test_file}")
    get_filename_component(dir  "${rel_path}" DIRECTORY)
    get_filename_component(stem "${rel_path}" NAME_WE)

    string(REGEX REPLACE "^test_" "" src_stem "${stem}")

    set(candidate "${CMAKE_SOURCE_DIR}/src/${dir}/${src_stem}.c")
    if(EXISTS "${candidate}")
        list(APPEND SRC_FILES "${candidate}")
    endif()

    string(APPEND SUITE_EXTERNS "extern const TestSuite ${src_stem}_suite;\n")
    string(APPEND SUITE_ENTRIES "    &${src_stem}_suite,\n")
endforeach()

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated_suites.c"
    "#include \"test.h\"\n\n"
    "${SUITE_EXTERNS}\n"
    "const TestSuite *suites[] = {\n${SUITE_ENTRIES}};\n\n"
    "const int suite_count = (int)(sizeof(suites) / sizeof(suites[0]));\n"
)

set(TEST_SOURCES
    test.c
    main.c
    ${TEST_IMPL_FILES}
    ${SRC_FILES}
    "${CMAKE_CURRENT_BINARY_DIR}/generated_suites.c"
)

add_executable(SoftwareRasterizerTests ${TEST_SOURCES})

target_compile_features(SoftwareRasterizerTests PRIVATE c_std_99)

target_precompile_headers(SoftwareRasterizerTests
    PRIVATE ${CMAKE_SOURCE_DIR}/src/core/types.h
)

target_include_directories(SoftwareRasterizerTests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(SoftwareRasterizerTests
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${CMAKE_BINARY_DIR}"
)

target_link_libraries(SoftwareRasterizerTests PRIVATE m)

target_compile_definitions(SoftwareRasterizerTests
    PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

if(MSVC)
    target_compile_options(SoftwareRasterizerTests PRIVATE /W4)
else()
    target_compile_options(SoftwareRasterizerTests PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(SoftwareRasterizerTests PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(SoftwareRasterizerTests PRIVATE
            -fsanitize=address,undefined
        )
    endif()
endif()
