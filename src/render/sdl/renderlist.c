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


void Enj_RenderList_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_RenderList *rl = (Enj_RenderList *)d;

    Enj_RenderListFree(rl);
    Enj_Free(a, rl);
}
