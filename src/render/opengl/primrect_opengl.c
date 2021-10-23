#include "primrect_opengl.h"
#include "../../core/allocator.h"

void Enj_PrimRect_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a){
    Enj_PrimRect_OpenGL *pr = (Enj_PrimRect_OpenGL *)d;
    Enj_Free(a, pr);
}
