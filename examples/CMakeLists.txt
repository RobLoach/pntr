# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# pntr_examples
add_executable(pntr_examples
    pntr_examples.c
)
target_link_libraries(pntr_examples PUBLIC
    pntr
    m
)

add_custom_target(pntr_examples_output
    COMMAND pntr_examples
)
