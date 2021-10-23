#pragma once

#include <stddef.h>

#include "../list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_RenderNode_OpenGL{
    Enj_ListNode listnode;

    void *data;
    void *ctx;
    //data, render ctx, matrix, translate
    void (*onrender)(void *, void *, const float *, const float *);
    Enj_Allocator *allocdata;
    //data, render ctx, data allocator
    void (*onfreedata)(void *, void *, Enj_Allocator *a);

    int priority;
    int active;
} Enj_RenderNode_OpenGL;

typedef struct Enj_RenderList_OpenGL{
    Enj_List list;
    Enj_Allocator *allocnode;

    float scalex;
    float scaley;
    float translatex;
    float translatey;
    float angle;
} Enj_RenderList_OpenGL;

void Enj_RenderListInit_OpenGL(Enj_RenderList_OpenGL *rl, Enj_Allocator *an);
void Enj_RenderListFree_OpenGL(Enj_RenderList_OpenGL *rl);
Enj_RenderNode_OpenGL * Enj_RenderListAppend_OpenGL(Enj_RenderList_OpenGL *rl);
void Enj_RenderListRemove_OpenGL(Enj_RenderList_OpenGL *rl, Enj_RenderNode_OpenGL *rn);

void Enj_RenderList_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a);
void Enj_RenderList_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
);

#ifdef __cplusplus
}
#endif
