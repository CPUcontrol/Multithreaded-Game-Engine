#include <SDL_render.h>

#include "sprite.h"
#include "../core/allocator.h"
#include "../core/glyph.h"
#include "../core/texture.h"

void Enj_Sprite_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_Sprite *sp = (Enj_Sprite *)d;
    Enj_Free(a, sp);
}
void Enj_Sprite_OnRender(void *d, void *ctx, int xoffset, int yoffset){
    Enj_Sprite *sprite = (Enj_Sprite *)d;
    SDL_Renderer *rend = (SDL_Renderer *)ctx;

    SDL_Rect drawrect;
    SDL_Point drawcenter;

    drawrect.x = xoffset + sprite->x - sprite->xcen;
    drawrect.y = yoffset + sprite->y - sprite->ycen;
    drawrect.w = sprite->w;
    drawrect.h = sprite->h;

    drawcenter.x = sprite->xcen;
    drawcenter.y = sprite->ycen;

    SDL_SetTextureColorMod(sprite->glyph->texture->tx,
                            sprite->fill[0],
                            sprite->fill[1],
                            sprite->fill[2]);
    SDL_SetTextureAlphaMod(sprite->glyph->texture->tx,
                            sprite->fill[3]);
    SDL_RenderCopyEx(
        rend,
        sprite->glyph->texture->tx,
        &sprite->glyph->rect,
        &drawrect,
        360. / 65536. * (double)sprite->angle,
        &drawcenter,
        SDL_FLIP_NONE);

}
