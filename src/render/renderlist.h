#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;
typedef struct Enj_RenderNode Enj_RenderNode;

typedef struct Enj_RenderNode{
    void *data;
    void *ctx;
    //data, render ctx, x offset, y offset
    void (*onrender)(void *, void *, int, int);
    Enj_Allocator *allocdata;
    //data, render ctx, data allocator
    void (*onfreedata)(void *, void *, Enj_Allocator *a);

    Enj_RenderNode *prev;
    Enj_RenderNode *next;
    int priority;
    int active;
} Enj_RenderNode;

typedef struct Enj_RenderList{
    Enj_Allocator *allocnode;
    Enj_RenderNode *head;
    Enj_RenderNode *tail;
    size_t size;

    int xoffset;
    int yoffset;
} Enj_RenderList;

void Enj_RenderListInit(Enj_RenderList *rl, Enj_Allocator *an);
void Enj_RenderListFree(Enj_RenderList *rl);
Enj_RenderNode * Enj_RenderListAppend(Enj_RenderList *rl);
void Enj_RenderListRemove(Enj_RenderList *rl, Enj_RenderNode *rn);

void Enj_RenderList_OnFree(void *d, void *ctx, Enj_Allocator *a);
void Enj_RenderList_OnRender(void *d, void *ctx, int xoffset, int yoffset);

#ifdef __cplusplus
}
#endif
