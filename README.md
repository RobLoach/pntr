# pntr

Low-level image manipulation using the ARGB8888 pixel format in C99.

Work in progress.

## Screenshots

![pntr: Examples - SDL Screenshot](examples/pntr_examples_sdl.png)

![pntr: Examples - raylib Screenshot](examples/pntr_examples_raylib.png)

```
# setup for native build
cmake -B build

# build library and native demos
cmake --build build

# build web-demo
emcc examples/pntr_examples_sdl.c -DPLATFORM_WEB -sUSE_SDL=2 -o web/pntr_demo.js --preload-file examples/resources@/resources
```


## License

[Zlib](LICENSE)
