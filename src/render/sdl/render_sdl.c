#include <SDL_render.h>

#include "sprite.h"
#include "primrect.h"
#include "renderlist.h"

#include "../../core/allocator.h"
#include "../../core/graphics/sdl/glyph_sdl.h"
#include "../../core/graphics/sdl/texture_sdl.h"

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

void Enj_PrimRectLine_OnRender(void *d, void *ctx, int xoffset, int yoffset){
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
        pr->r,
        pr->g,
        pr->b,
        pr->a);
    SDL_RenderDrawRect(rend, &drawrect);
}

void Enj_PrimRectFill_OnRender(void *d, void *ctx, int xoffset, int yoffset){
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
        pr->r,
        pr->g,
        pr->b,
        pr->a);
    SDL_RenderFillRect(rend, &drawrect);
}

static int rendernodecmp(Enj_ListNode *a, Enj_ListNode *b){
    Enj_RenderNode *nodea = (Enj_RenderNode *)
            ((char *)a - offsetof(Enj_RenderNode, listnode));

    Enj_RenderNode *nodeb = (Enj_RenderNode *)
            ((char *)b - offsetof(Enj_RenderNode, listnode));

    return   (nodea->priority > nodeb->priority)
            -(nodea->priority < nodeb->priority);
}

void Enj_RenderList_OnRender(void *d, void *ctx, int xoffset, int yoffset){
    Enj_RenderList *rl = (Enj_RenderList *)d;
    Enj_ListSort(&rl->list, &rendernodecmp);

    int xo = rl->xoffset + xoffset;
    int yo = rl->yoffset + yoffset;

    Enj_ListNode *it = rl->list.head;
    while(it){
        Enj_RenderNode *node = (Enj_RenderNode *)
            ((char *)it - offsetof(Enj_RenderNode, listnode));

        if (node->active)
            (*node->onrender)(
                node->data,
                node->ctx,
                xo,
                yo
            );
        it = it->next;
    }

}
