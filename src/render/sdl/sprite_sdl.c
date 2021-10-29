#include "sprite_sdl.h"
#include "../../core/allocator.h"

void Enj_Sprite_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a){
    Enj_Sprite_SDL *sp = (Enj_Sprite_SDL *)d;
    Enj_Free(a, sp);
}
