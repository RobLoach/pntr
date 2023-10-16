# libretro-common
include(FetchContent)
FetchContent_Declare(
    libretro
    GIT_REPOSITORY https://github.com/libretro/libretro-common.git
    GIT_TAG 01c6122931a10a7012973054e7067859d2116420
)
FetchContent_GetProperties(libretro)
if (NOT libretro_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(libretro)

    add_library(libretro INTERFACE)
    target_include_directories(libretro INTERFACE ${libretro_SOURCE_DIR}/include)
endif()
