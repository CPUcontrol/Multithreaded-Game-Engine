#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_Glyph_SDL Enj_Glyph_SDL;
typedef struct Enj_Sprite_SDL{
    int x;
    int y;
    int w;
    int h;

    int xcen;
    int ycen;

    unsigned char fill[4];
    unsigned short angle;

    Enj_Glyph_SDL *glyph;
} Enj_Sprite_SDL;

void Enj_Sprite_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a);
void Enj_Sprite_OnRender_SDL(void *d, void *ctx, int xoffset, int yoffset);
#ifdef __cplusplus
}
#endif
