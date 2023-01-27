cmake_minimum_required(VERSION 3.11)
project(pntr
    DESCRIPTION "pntr: Paint the things"
    HOMEPAGE_URL "https://github.com/robloach/pntr"
    VERSION 0.0.1
    LANGUAGES C
)

# pntr
add_library(pntr INTERFACE)
target_include_directories(pntr INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_IS_MAIN TRUE)
else()
    set(PNTR_IS_MAIN FALSE)
endif()
option(PNTR_BUILD_EXAMPLES "Examples" ${PNTR_IS_MAIN})
option(PNTR_BUILD_TESTS "TESTS" ${PNTR_IS_MAIN})

# Examples
if (PNTR_BUILD_EXAMPLES)
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
        add_subdirectory(test)
    endif()
endif()