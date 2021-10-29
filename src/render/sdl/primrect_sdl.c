#include "primrect_sdl.h"
#include "../../core/allocator.h"

void Enj_PrimRect_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a){
    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)d;
    Enj_Free(a, pr);
}
