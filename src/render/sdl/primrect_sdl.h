#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_PrimRect_SDL{
    int x;
    int y;
    int w;
    int h;

    int xcen;
    int ycen;

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Enj_PrimRect_SDL;

void Enj_PrimRect_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a);
void Enj_PrimRectLine_OnRender_SDL(void *d, void *ctx, int xoffset, int yoffset);
void Enj_PrimRectFill_OnRender_SDL(void *d, void *ctx, int xoffset, int yoffset);
#ifdef __cplusplus
}
#endif
