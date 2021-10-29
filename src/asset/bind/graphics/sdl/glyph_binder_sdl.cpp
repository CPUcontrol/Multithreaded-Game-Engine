#include <stdlib.h>

#include "../../../../core/graphics/sdl/glyph_sdl.h"
#include "glyph_binder_sdl.hpp"


glyph_binder_SDL::glyph_binder_SDL(
    multi_dispatch &md,
    lua_State *L
): dispatch(md), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Glyph_SDL));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Glyph_SDL), sizeof(Enj_Glyph_SDL));
}

glyph_binder_SDL::~glyph_binder_SDL(){
    free(buf);
}
