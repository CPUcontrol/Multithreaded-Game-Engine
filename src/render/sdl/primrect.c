#include <SDL_render.h>

#include "primrect.h"
#include "../../core/allocator.h"

void Enj_PrimRect_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_PrimRect *pr = (Enj_PrimRect *)d;
    Enj_Free(a, pr);
}
void Enj_PrimRect_OnRender(void *d, void *ctx, int xoffset, int yoffset){
    Enj_PrimRect *pr = (Enj_PrimRect *)d;
    SDL_Renderer *rend = (SDL_Renderer *)ctx;

    SDL_Rect drawrect;
    SDL_Point drawcenter;

    drawrect.x = xoffset + pr->x - pr->xcen;
    drawrect.y = yoffset + pr->y - pr->ycen;
    drawrect.w = pr->w;
    drawrect.h = pr->h;

    drawcenter.x = pr->xcen;
    drawcenter.y = pr->ycen;

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend,
        pr->fill[0],
        pr->fill[1],
        pr->fill[2],
        pr->fill[3]);
    SDL_RenderFillRect(rend, &drawrect);
    SDL_SetRenderDrawColor(rend,
        pr->stroke[0],
        pr->stroke[1],
        pr->stroke[2],
        pr->stroke[3]);
    SDL_RenderDrawRect(rend, &drawrect);

}
