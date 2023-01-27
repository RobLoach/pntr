# pntr_test
add_executable(pntr_test pntr_test.c)
target_compile_options(pntr_test PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
target_link_libraries(pntr_test PUBLIC
    pntr
)

# Set up the test
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME pntr_test COMMAND pntr_test)