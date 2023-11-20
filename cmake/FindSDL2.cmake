# SDL2
include(FetchContent)
FetchContent_Declare(
    SDL2Source
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.1
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(SDL2Source)
include_directories(${SDL2Source_SOURCE_DIR})
