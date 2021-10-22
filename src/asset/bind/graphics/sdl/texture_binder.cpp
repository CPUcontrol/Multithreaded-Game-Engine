#include <stdlib.h>

#include "../../../../core/graphics/sdl/texture_sdl.h"
#include "texture_binder.hpp"


texture_binder::texture_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L
): dispatch(md), basepath(base), rend(rend), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Texture_SDL));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Texture_SDL), sizeof(Enj_Texture_SDL));
}

texture_binder::~texture_binder(){
    free(buf);
}
