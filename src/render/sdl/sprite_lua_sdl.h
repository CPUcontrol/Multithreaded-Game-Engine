#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct Enj_Allocator Enj_Allocator;

void bindsprite_SDL(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocsprite
);

#ifdef __cplusplus
}
#endif
