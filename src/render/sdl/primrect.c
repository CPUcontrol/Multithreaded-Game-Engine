#include "primrect.h"
#include "../../core/allocator.h"

void Enj_PrimRect_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_PrimRect *pr = (Enj_PrimRect *)d;
    Enj_Free(a, pr);
}
