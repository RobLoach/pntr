#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #ifndef PNTR_API
        #define PNTR_API EMSCRIPTEN_KEEPALIVE
    #endif
#endif

// All the bells and whistles
#ifndef PNTR_DISABLE_DEFAULT_FONT
    #define PNTR_ENABLE_DEFAULT_FONT
#endif
#ifndef PNTR_DISABLE_TTF
    #define PNTR_ENABLE_TTF
#endif
#ifndef PNTR_DISABLE_FILTER_SMOOTH
    #define PNTR_ENABLE_FILTER_SMOOTH
#endif

#define PNTR_IMPLEMENTATION
#include "../pntr.h"
