#include <stdlib.h>

#include "../../../../core/graphics/opengl/texture_opengl.h"
#include "texture_binder_opengl.hpp"


texture_binder_OpenGL::texture_binder_OpenGL(
        multi_dispatch &md,
        std::string &base,
        lua_State *L
): dispatch(md), basepath(base), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Texture_OpenGL));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Texture_OpenGL), sizeof(Enj_Texture_OpenGL));
}

texture_binder_OpenGL::~texture_binder_OpenGL(){
    free(buf);
}
