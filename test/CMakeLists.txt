# pntr_test
add_executable(pntr_test pntr_test.c)
target_compile_options(pntr_test PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
target_link_libraries(pntr_test PUBLIC
    pntr
    m
)
set_property(TARGET pntr_test PROPERTY C_STANDARD 99)

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# Set up the test
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME pntr_test COMMAND pntr_test)
