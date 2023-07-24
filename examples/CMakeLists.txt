# CMAKE Modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Resources
file(GLOB resources resources/*)
set(examples_resources)
list(APPEND examples_resources ${resources})
file(COPY ${examples_resources} DESTINATION "resources/")

find_package(pntr_app)

if (EMSCRIPTEN)
    add_executable(pntr_examples_emscripten
        pntr_example.c
    )
    target_link_libraries(pntr_examples_emscripten PUBLIC
        pntr
        pntr_app
        m
    )

    set_target_properties(pntr_examples_emscripten PROPERTIES PREFIX "")
    set_target_properties(pntr_examples_emscripten PROPERTIES SUFFIX ".html")
    set_target_properties(pntr_examples_emscripten PROPERTIES OUTPUT_NAME "index")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file resources@/resources --shell-file ${CMAKE_SOURCE_DIR}/example/pntr_examples_emscripten.html")

    # Force disable the other examples
    set(PNTR_BUILD_EXAMPLES_SDL FALSE)
    set(PNTR_BUILD_EXAMPLES_RAYLIB FALSE)
    set(PNTR_BUILD_EXAMPLES_LIBRETRO FALSE)
endif()

# SDL
if (PNTR_BUILD_EXAMPLES_SDL)
    find_package(SDL2 QUIET)
    if (SDL2_FOUND)
        add_executable(pntr_examples_sdl pntr_examples.c)
        target_link_libraries(pntr_examples_sdl PRIVATE
            SDL2::SDL2
            SDL2::SDL2main
            pntr
            pntr_app
            m
        )
        target_compile_definitions(pntr_examples_sdl PUBLIC PNTR_APP_SDL)
    else()
        message(WARNING "SDL not found")
    endif()
endif()

# raylib
if (PNTR_BUILD_EXAMPLES_RAYLIB)
    find_package(raylib QUIET)

    add_executable(pntr_examples_raylib
        pntr_examples.c
    )
    target_link_libraries(pntr_examples_raylib PUBLIC
        raylib
        pntr
        pntr_app
        m
    )
    target_compile_definitions(pntr_examples_raylib PUBLIC PNTR_APP_RAYLIB)
endif()

# libretro
if (PNTR_BUILD_EXAMPLES_LIBRETRO)
    find_package(libretro QUIET)
    add_library(pntr_examples_libretro SHARED
        pntr_examples.c
    )
    target_link_libraries(pntr_examples_libretro PUBLIC
        pntr
        pntr_app
        libretro
        m
    )
    set_target_properties(pntr_examples_libretro PROPERTIES PREFIX "")
    target_compile_definitions(pntr_examples_libretro PUBLIC PNTR_APP_LIBRETRO)
endif()
