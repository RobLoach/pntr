#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #ifndef PNTR_API
        #define PNTR_API EMSCRIPTEN_KEEPALIVE
    #endif
#endif

// All the bells and whistles
#define PNTR_SUPPORT_DEFAULT_FONT
#define PNTR_SUPPORT_TTF
#define PNTR_SUPPORT_FILTER_SMOOTH

#define PNTR_IMPLEMENTATION
#include "../pntr.h"
