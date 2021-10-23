#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_PrimRect_OpenGL{
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
} Enj_PrimRect_OpenGL;

void Enj_PrimRect_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a);
void Enj_PrimRectLine_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
);
void Enj_PrimRectFill_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
);
#ifdef __cplusplus
}
#endif
