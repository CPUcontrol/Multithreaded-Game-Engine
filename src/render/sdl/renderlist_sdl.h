#pragma once

#include <stddef.h>

#include "../list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

typedef struct Enj_RenderNode_SDL{
    Enj_ListNode listnode;

    void *data;
    void *ctx;
    //data, render ctx, x offset, y offset
    void (*onrender)(void *, void *, int, int);
    Enj_Allocator *allocdata;
    //data, render ctx, data allocator
    void (*onfreedata)(void *, void *, Enj_Allocator *a);

    int priority;
    int active;
} Enj_RenderNode_SDL;

typedef struct Enj_RenderList_SDL{
    Enj_List list;
    Enj_Allocator *allocnode;

    int xoffset;
    int yoffset;
} Enj_RenderList_SDL;

void Enj_RenderListInit_SDL(Enj_RenderList_SDL *rl, Enj_Allocator *an);
void Enj_RenderListFree_SDL(Enj_RenderList_SDL *rl);
Enj_RenderNode_SDL * Enj_RenderListAppend_SDL(Enj_RenderList_SDL *rl);
void Enj_RenderListRemove_SDL(Enj_RenderList_SDL *rl, Enj_RenderNode_SDL *rn);

void Enj_RenderList_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a);
void Enj_RenderList_OnRender_SDL(void *d, void *ctx, int xoffset, int yoffset);

#ifdef __cplusplus
}
#endif
