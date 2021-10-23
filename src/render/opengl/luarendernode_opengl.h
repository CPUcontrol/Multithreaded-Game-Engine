#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_RenderNode_OpenGL Enj_RenderNode_OpenGL;
typedef struct Enj_RenderList_OpenGL Enj_RenderList_OpenGL;

typedef struct luarendernode_OpenGL{
    Enj_RenderNode_OpenGL *rn;
    Enj_RenderList_OpenGL *parent;
}luarendernode_OpenGL;

#ifdef __cplusplus
}
#endif
