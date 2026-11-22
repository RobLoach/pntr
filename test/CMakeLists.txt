# pntr_test
add_executable(pntr_test pntr_test.c)
target_compile_options(pntr_test PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
target_link_libraries(pntr_test PUBLIC
    pntr
    #m
)

# C99 Standard
set_property(TARGET pntr_test PROPERTY C_STANDARD 99)
set_property(TARGET pntr_test PROPERTY C_STANDARD_REQUIRED TRUE)

# Strict Warnings and Errors
if(MSVC)
    target_compile_options(pntr_test PRIVATE /W4 /WX)
else()
    target_compile_options(pntr_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# Set up the test
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
add_test(NAME pntr_test COMMAND pntr_test)

# The same suite with PNTR_ENABLE_TRANSFORM, which also runs every other test through the
# identity transform.
add_executable(pntr_test_transform pntr_test.c)
target_compile_definitions(pntr_test_transform PRIVATE PNTR_ENABLE_TRANSFORM)
target_link_libraries(pntr_test_transform PUBLIC pntr)
set_property(TARGET pntr_test_transform PROPERTY C_STANDARD 99)
set_property(TARGET pntr_test_transform PROPERTY C_STANDARD_REQUIRED TRUE)
if(MSVC)
    target_compile_options(pntr_test_transform PRIVATE /W4 /WX)
else()
    target_compile_options(pntr_test_transform PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion)
endif()
add_test(NAME pntr_test_transform COMMAND pntr_test_transform)
