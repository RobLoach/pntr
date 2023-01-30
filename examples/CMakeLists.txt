# Dependencies
find_package(SDL2 REQUIRED)

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# pntr_sdl_example
add_executable(pntr_examples_sdl pntr_examples_sdl.c)
target_link_libraries(pntr_examples_sdl PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    pntr
)
