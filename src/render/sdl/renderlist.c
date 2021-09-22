#include "renderlist.h"

#include "../../core/allocator.h"

void Enj_RenderListInit(Enj_RenderList *rl, Enj_Allocator *an){
    rl->allocnode = an;
    rl->head = NULL;
    rl->tail = NULL;
    rl->size = 0;
    rl->xoffset = 0;
    rl->yoffset = 0;
}
void Enj_RenderListFree(Enj_RenderList *rl){
    Enj_RenderNode *it = rl->head;
    while(it){
        Enj_RenderNode *itn = it->next;

        Enj_Free(rl->allocnode, it);

        it = itn;
    }
    rl->head = NULL;
    rl->tail = NULL;
    rl->size = 0;
}
Enj_RenderNode * Enj_RenderListAppend(Enj_RenderList *rl){
    Enj_RenderNode * res = (Enj_RenderNode *)
            Enj_Alloc(rl->allocnode, sizeof(Enj_RenderNode));
    if(!res) return NULL;

    if(rl->size == 0){
        rl->head = res;

        res->prev = NULL;
        res->next = NULL;
        res->priority = 0;

        rl->tail = res;
    }
    else{
        res->prev = rl->tail;
        res->next = NULL;
        res->priority = 0;

        rl->tail->next = res;

        rl->tail = res;
    }

    ++rl->size;
    return res;
}
void Enj_RenderListRemove(Enj_RenderList *rl, Enj_RenderNode *rn){
    if(rn == rl->head){
        rl->head = rn->next;
    }
    else{
        rn->prev->next = rn->next;
    }

    if(rn == rl->tail){
        rl->tail = rn->prev;
    }
    else{
        rn->next->prev = rn->prev;
    }

    Enj_Free(rl->allocnode, rn);
    --rl->size;
}

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

void Enj_RenderList_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_RenderList *rl = (Enj_RenderList *)d;

    Enj_RenderListFree(rl);
    Enj_Free(a, rl);
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
