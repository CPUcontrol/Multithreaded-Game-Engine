#include "sprite_opengl.h"
#include "../../core/allocator.h"

void Enj_Sprite_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a){
    Enj_Sprite_OpenGL *sp = (Enj_Sprite_OpenGL *)d;
    Enj_Free(a, sp);
}
