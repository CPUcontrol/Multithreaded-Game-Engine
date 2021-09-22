#include <SDL2_render.h>
#include <SDL2_video.h>

#include "render_sdl.h"
#include "render.h"

Enj_Renderer * Enj_InitRenderer(SDL_Window *window){
    Enj_Renderer *r = malloc(sizeof(Enj_Renderer));
    SDL_RaiseWindow(r->window);


    r->sdlrend = SDL_CreateRenderer(
        r->window,
        -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    );
    SDL_SetRenderDrawColor(r->sdlrend,
                0,
                0,
                0,
                255);
    SDL_RenderClear(r->sdlrend);
    SDL_RenderPresent(r->sdlrend);

    return r;
}

void Enj_FreeRenderer(Enj_Renderer *r){
    SDL_DestroyRenderer(r->sdlrend);
    free(r);
}
