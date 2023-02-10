# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

# SDL
if (PNTR_BUILD_EXAMPLES_SDL)
    find_package(SDL2 REQUIRED)
    if (SDL2_FOUND)
        add_executable(pntr_examples_sdl pntr_examples_sdl.c)
        target_link_libraries(pntr_examples_sdl PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            pntr
            m
        )
    else()
        message(WARNING "SDL not found")
    endif()
endif()

# raylib
if (PNTR_BUILD_EXAMPLES_RAYLIB)
    find_package(raylib QUIET)
    if (NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 4.2.0
            GIT_SHALLOW 1
        )
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED)
            set(FETCHCONTENT_QUIET NO)
            FetchContent_Populate(raylib)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
            set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

            add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
        endif()
    endif()

    # Make sure raylib was found correctly
    if (TARGET raylib)
        add_executable(pntr_examples_raylib
            pntr_examples_raylib.c
        )
        target_link_libraries(pntr_examples_raylib PUBLIC
            raylib
            pntr
            m
        )
    endif()
endif()

# libretro
if (PNTR_BUILD_EXAMPLES_LIBRETRO)
    include(FetchContent)
    FetchContent_Declare(
        libretro
        GIT_REPOSITORY https://github.com/libretro/libretro-common.git
        GIT_TAG 458bcd842bc48f730e12732fe8b3280e834d45ff
    )
    FetchContent_GetProperties(libretro)
    if (NOT libretro_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(libretro)

        add_library(libretro INTERFACE)
        target_include_directories(pntr INTERFACE ${libretro_SOURCE_DIR}/include)
    endif()

    add_library(pntr_examples_libretro SHARED
        pntr_examples_libretro.c
    )
    target_link_libraries(pntr_examples_libretro PUBLIC
        pntr
        libretro
        m
    )
    set_target_properties(pntr_examples_libretro PROPERTIES PREFIX "")
endif()
