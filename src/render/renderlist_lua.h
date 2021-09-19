#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;

void bindrenderlist(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode,
    Enj_Allocator *allocsprite,
    Enj_Allocator *allocprimrect
);

Enj_RenderList * bindroot_renderlist(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode
);

#ifdef __cplusplus
}
#endif
