#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_PrimRect{
    int x;
    int y;
    int w;
    int h;

    int xcen;
    int ycen;

    unsigned char fill[4];
    unsigned char stroke[4];
} Enj_PrimRect;

void Enj_PrimRect_OnFree(void *d, void *ctx, Enj_Allocator *a);
void Enj_PrimRect_OnRender(void *d, void *ctx, int xoffset, int yoffset);
#ifdef __cplusplus
}
#endif
