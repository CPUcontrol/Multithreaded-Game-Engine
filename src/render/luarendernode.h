#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_RenderNode Enj_RenderNode;
typedef struct Enj_RenderList Enj_RenderList;

typedef struct luarendernode{
    Enj_RenderNode *rn;
    Enj_RenderList *parent;
}luarendernode;

#ifdef __cplusplus
}
#endif
