#include <stdlib.h>

#include "../../core/sound.h"
#include "sound_binder.hpp"


sound_binder::sound_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        lua_State *L
): dispatch(md), basepath(base), Lmain(L)
{
    buf = malloc(1<<12);
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, 1<<12, sizeof(Enj_Sound));
}

sound_binder::~sound_binder(){
    free(buf);
}
