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


//RenderList
static void mergesort_r(
    Enj_RenderNode *beg, size_t sz,
    Enj_RenderNode **nh, Enj_RenderNode **nt)
{
    if(sz == 1){
        beg->prev = NULL;
        beg->next = NULL;

        *nh = beg;
        *nt = beg;
        return;
    }

    Enj_RenderNode *mid = beg;
    size_t sz1 = sz/2;
    for(size_t i = 0; i < sz1; i++){
        mid = mid->next;
    }

    Enj_RenderNode *h1;
    Enj_RenderNode *t1;

    Enj_RenderNode *h2;
    Enj_RenderNode *t2;

    mergesort_r(beg, sz1, &h1, &t1);
    mergesort_r(mid, sz-sz1, &h2, &t2);

    Enj_RenderNode *it;
    if(h1->priority <= h2->priority){
        *nh = h1;

        it = h1;
        h1 = h1->next;

        if(h1 == NULL){
            it->next = h2;
            h2->prev = it;

            *nt = t2;
            return;
        }
    }
    else{
        *nh = h2;

        it = h2;
        h2 = h2->next;

        if(h2 == NULL){
            it->next = h1;
            h1->prev = it;

            *nt = t1;
            return;
        }
    }

    for(;;){
        if(h1->priority <= h2->priority){
            it->next = h1;
            h1->prev = it;

            it = h1;
            h1 = h1->next;

            if(h1 == NULL){
                it->next = h2;
                h2->prev = it;

                *nt = t2;
                return;
            }
        }
        else{
            it->next = h2;
            h2->prev = it;

            it = h2;
            h2 = h2->next;

            if(h2 == NULL){
                it->next = h1;
                h1->prev = it;

                *nt = t1;
                return;
            }
        }
    }


}

static void Enj_RenderListSort(Enj_RenderList *rl){
    //Empty list or single list already sorted
    if(rl->size < 2) return;
    //first check if sorted already
    Enj_RenderNode *it = rl->head;
    int prmin = it->priority;
    while(it->next){
        //If not sorted, start to sort
        if(it->next->priority > prmin) {
            Enj_RenderNode *nh;
            Enj_RenderNode *nt;

            mergesort_r(rl->head, rl->size, &nh, &nt);

            rl->head = nh;
            rl->tail = nt;

            return;
        }

        prmin = it->next->priority;
        it = it->next;
    }
}

void Enj_RenderList_OnRender(void *d, void *ctx, int xoffset, int yoffset){
    Enj_RenderList *rl = (Enj_RenderList *)d;
    Enj_RenderListSort(rl);

    Enj_RenderNode *it = rl->head;
    while(it){
        if (it->active)
            (*it->onrender)(
                it->data,
                it->ctx,
                rl->xoffset + xoffset,
                rl->yoffset + yoffset
            );
        it = it->next;
    }

}
