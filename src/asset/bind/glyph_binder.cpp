#include <stdlib.h>

#include "../../core/glyph.h"
#include "glyph_binder.hpp"


glyph_binder::glyph_binder(
    multi_dispatch &md,
    lua_State *L
): dispatch(md), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Glyph));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Glyph), sizeof(Enj_Glyph));
}

glyph_binder::~glyph_binder(){
    free(buf);
}
