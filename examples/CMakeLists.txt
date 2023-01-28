# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

set(SDL_SHARED FALSE)
set(SDL2_DISABLE_UNINSTALL TRUE)

# Dependencies
find_package(SDL2 REQUIRED)

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# pntr_sdl_example
add_executable(pntr_sdl_example pntr_sdl_example.c)
target_link_libraries(pntr_sdl_example PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    m
    pntr
)
