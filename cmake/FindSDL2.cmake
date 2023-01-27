include(FetchContent)
FetchContent_Declare(
    SDL2Source
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.1
)
FetchContent_MakeAvailable(SDL2Source)
include_directories(${SDL2Source_SOURCE_DIR})