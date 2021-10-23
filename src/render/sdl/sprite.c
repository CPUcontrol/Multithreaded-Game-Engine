#include "sprite.h"
#include "../../core/allocator.h"

void Enj_Sprite_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_Sprite *sp = (Enj_Sprite *)d;
    Enj_Free(a, sp);
}
