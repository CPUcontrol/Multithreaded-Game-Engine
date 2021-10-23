#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct Enj_Renderer_OpenGL Enj_Renderer_OpenGL;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;

void bindprimrect_OpenGL(
    lua_State *L,
    Enj_Renderer_OpenGL *rend,
    Enj_Allocator *allocprimrect
);

#ifdef __cplusplus
}
#endif
