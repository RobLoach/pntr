# pntr

Low-level image manipulation using the ARGB8888 pixel format in C99.

Work in progress.

## Screenshots

![pntr: Examples - SDL Screenshot](examples/pntr_examples_sdl.png)

![pntr: Examples - raylib Screenshot](examples/pntr_examples_raylib.png)

``` bash
# setup for native build
cmake -B build

# build library and native demos
cmake --build build
```

## Web Assembly

``` bash
# Emscripten
emcc examples/pntr_examples_web.c -o build/index.html --preload-file examples/resources@/resources --shell-file examples/pntr_examples_web.html

# Emscripten with SDL
emcc examples/pntr_examples_sdl.c -sUSE_SDL=2 -o build/index.html --preload-file examples/resources@/resources --shell-file examples/pntr_examples_web.html
```

## License

[Zlib](LICENSE)
