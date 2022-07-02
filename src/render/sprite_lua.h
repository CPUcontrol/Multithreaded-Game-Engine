#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;
typedef struct Enj_RenderList Enj_RenderList;

void bindsprite(
    lua_State *L,
    Enj_RenderList *parentrender,
    SDL_Renderer *rend,
    Enj_Allocator *allocsprite
);

#ifdef __cplusplus
}
#endif
