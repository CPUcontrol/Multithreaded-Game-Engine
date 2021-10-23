#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct Enj_Renderer_OpenGL Enj_Renderer_OpenGL;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;

void bindrenderlist_OpenGL(
    lua_State *L,
    Enj_Renderer_OpenGL *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode,
    Enj_Allocator *allocsprite,
    Enj_Allocator *allocprimrect
);

Enj_RenderList_OpenGL * bindroot_renderlist_OpenGL(
    lua_State *L,
    Enj_Renderer_OpenGL *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode
);

#ifdef __cplusplus
}
#endif
