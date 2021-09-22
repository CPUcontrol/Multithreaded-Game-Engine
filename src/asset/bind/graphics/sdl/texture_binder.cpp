#include <stdlib.h>

#include "../../../../core/graphics/sdl/texture.h"
#include "texture_binder.hpp"


texture_binder::texture_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L
): dispatch(md), basepath(base), rend(rend), Lmain(L)
{
    buf = malloc(1<<12);
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, 1<<12, sizeof(Enj_Texture));
}

texture_binder::~texture_binder(){
    free(buf);
}
