#include <stdlib.h>

#include "../../../../core/graphics/opengl/glyph_opengl.h"
#include "glyph_binder_opengl.hpp"


glyph_binder_OpenGL::glyph_binder_OpenGL(
    multi_dispatch &md,
    lua_State *L
): dispatch(md), Lmain(L)
{
    buf = malloc((1<<18)*sizeof(Enj_Glyph_OpenGL));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<18)*sizeof(Enj_Glyph_OpenGL), sizeof(Enj_Glyph_OpenGL));
}

glyph_binder_OpenGL::~glyph_binder_OpenGL(){
    free(buf);
}
