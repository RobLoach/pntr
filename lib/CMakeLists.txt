# libpntr
add_library(libpntr STATIC
    libpntr.c
)

set_target_properties(libpntr PROPERTIES
    # Already has a lib prefix
    PREFIX ""
)
