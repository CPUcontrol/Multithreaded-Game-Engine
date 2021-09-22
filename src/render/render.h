#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_Window SDL_Window;
typedef struct Enj_Renderer Enj_Renderer;


Enj_Renderer * Enj_InitRenderer(SDL_Window *window);

void Enj_FreeRenderer(Enj_Renderer *r);

#ifdef __cplusplus
}
#endif
