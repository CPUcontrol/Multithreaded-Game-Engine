#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_Glyph_OpenGL Enj_Glyph_OpenGL;
typedef struct Enj_Sprite_OpenGL{
    float x;
    float y;
    float w;
    float h;

    float xcen;
    float ycen;
    float angle;

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Enj_Glyph_OpenGL *glyph;
} Enj_Sprite_OpenGL;

void Enj_Sprite_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a);
void Enj_Sprite_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
);
#ifdef __cplusplus
}
#endif
