cmake_minimum_required(VERSION 3.11)
project(pntr
    DESCRIPTION "pntr: Header-only CPU graphics library for C99 and C++"
    HOMEPAGE_URL "https://github.com/robloach/pntr"
    VERSION 0.1.0
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

# Examples
option(PNTR_BUILD_EXAMPLES "Examples" ${PNTR_IS_MAIN})
if (PNTR_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
option(PNTR_BUILD_TESTS "Build Tests" ${PNTR_IS_MAIN})
if (PNTR_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        # Always print verbose output when tests fail if run using `make test`.
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure -V --debug")
        add_subdirectory(test)
    endif()
endif()

# Documentation
#
# Not gated on PNTR_BUILD_EXAMPLES: the example screenshots the docs embed are committed to
# the repository, so the documentation can be generated without rebuilding them. Gating on
# the examples meant the target could not even be configured anywhere they are turned off,
# which includes CI.
option(PNTR_BUILD_DOCS "Build Documentation" OFF)
if (PNTR_BUILD_DOCS)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        add_custom_target(pntr_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/.Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )

        # Refresh the screenshots first when the examples are being built anyway.
        if (PNTR_BUILD_EXAMPLES)
            add_dependencies(pntr_doxygen pntr_examples_output)
        endif()
    else (DOXYGEN_FOUND)
        message("Doxygen is required to build documentation")
    endif (DOXYGEN_FOUND)
endif()
