cmake_minimum_required(VERSION 3.11)
project(pntr
    DESCRIPTION "pntr: Paint the things"
    HOMEPAGE_URL "https://github.com/robloach/pntr"
    VERSION 0.0.1
    LANGUAGES C
)

set(C_STANDARD 99)

# pntr
add_library(pntr INTERFACE)
target_include_directories(pntr INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_IS_MAIN TRUE)
else()
    set(PNTR_IS_MAIN FALSE)
endif()

option(PNTR_BUILD_EXAMPLES_EMSCRIPTEN "Examples: Emscripten" ${PNTR_IS_MAIN})
option(PNTR_BUILD_EXAMPLES_SDL "Examples: SDL" ${PNTR_IS_MAIN})
option(PNTR_BUILD_EXAMPLES_RAYLIB "Examples: raylib" ${PNTR_IS_MAIN})
option(PNTR_BUILD_EXAMPLES_LIBRETRO "Examples: libretro" ${PNTR_IS_MAIN})
option(PNTR_BUILD_TESTS "Build Tests" ${PNTR_IS_MAIN})

# Examples
if (PNTR_BUILD_EXAMPLES_EMSCRIPTEN OR PNTR_BUILD_EXAMPLES_SDL OR PNTR_BUILD_EXAMPLES_RAYLIB OR PNTR_BUILD_EXAMPLES_LIBRETRO)
    add_subdirectory(examples)
endif()

# Tests
if (PNTR_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        # Always print verbose output when tests fail if run using `make test`.
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        add_subdirectory(test)
    endif()
endif()
