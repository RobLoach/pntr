# libretro-common
include(FetchContent)
FetchContent_Declare(
    libretro
    GIT_REPOSITORY https://github.com/libretro/libretro-common.git
    GIT_TAG 7edbfaf17baffa1b8a00231762aa7ead809711b5
)
FetchContent_GetProperties(libretro)
if (NOT libretro_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(libretro)

    add_library(libretro INTERFACE)
    target_include_directories(libretro INTERFACE ${libretro_SOURCE_DIR}/include)
endif()
