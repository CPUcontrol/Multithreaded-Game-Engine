#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_RenderNode_SDL Enj_RenderNode_SDL;
typedef struct Enj_RenderList_SDL Enj_RenderList_SDL;

typedef struct luarendernode_SDL{
    Enj_RenderNode_SDL *rn;
    Enj_RenderList_SDL *parent;
}luarendernode_SDL;

#ifdef __cplusplus
}
#endif
