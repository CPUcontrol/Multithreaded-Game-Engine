#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_Glyph Enj_Glyph;
typedef struct Enj_Sprite{
    int x;
    int y;
    int w;
    int h;

    int xcen;
    int ycen;

    unsigned char fill[4];
    unsigned short angle;

    Enj_Glyph *glyph;
} Enj_Sprite;

void Enj_Sprite_OnFree(void *d, void *ctx, Enj_Allocator *a);
void Enj_Sprite_OnRender(void *d, void *ctx, int xoffset, int yoffset);
#ifdef __cplusplus
}
#endif
