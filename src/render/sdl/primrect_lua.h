#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;

void bindprimrect(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocprimrect
);

#ifdef __cplusplus
}
#endif
