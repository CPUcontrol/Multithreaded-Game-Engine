#include <stdlib.h>

#include "../../../core/music.h"
#include "music_binder.hpp"


music_binder::music_binder(
        multi_dispatch &md,
        const char *base,
        lua_State *L
): dispatch(md), basepath(base), Lmain(L)
{
    buf = malloc((1<<12)*sizeof(Enj_Music));
    if(!buf) return;

    Enj_InitPoolAllocator(&alloc, &dat, buf, (1<<12)*sizeof(Enj_Music), sizeof(Enj_Music));
}

music_binder::~music_binder(){
    free(buf);
}
