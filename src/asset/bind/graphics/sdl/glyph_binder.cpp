#include <stdlib.h>

#include "../../../../core/graphics/sdl/glyph_sdl.h"
#include "glyph_binder.hpp"


glyph_binder::glyph_binder(
    multi_dispatch &md,
    lua_State *L
): dispatch(md), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Glyph_SDL));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Glyph_SDL), sizeof(Enj_Glyph_SDL));
}

glyph_binder::~glyph_binder(){
    free(buf);
}
