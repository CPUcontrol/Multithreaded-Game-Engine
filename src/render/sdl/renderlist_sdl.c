#include "renderlist_sdl.h"

#include "../../core/allocator.h"

void Enj_RenderListInit_SDL(Enj_RenderList_SDL *rl, Enj_Allocator *an){
    Enj_ListReset(&rl->list);
    rl->allocnode = an;
}
void Enj_RenderListFree_SDL(Enj_RenderList_SDL *rl){
    Enj_ListNode *it = (Enj_ListNode *)rl->list.head;
    while(it){
        Enj_ListNode *itn = it->next;

        Enj_Free(
            rl->allocnode,
            (Enj_RenderNode_SDL *)
                ((char *)it - offsetof(Enj_RenderNode_SDL, listnode))
            );

        it = itn;
    }
    Enj_ListReset(&rl->list);
}
Enj_RenderNode_SDL * Enj_RenderListAppend_SDL(Enj_RenderList_SDL *rl){
    Enj_RenderNode_SDL * res = (Enj_RenderNode_SDL *)
            Enj_Alloc(rl->allocnode, sizeof(Enj_RenderNode_SDL));
    if(!res) return NULL;

    Enj_ListAppend(&rl->list, &res->listnode);

    res->priority = 0;
    return res;
}
void Enj_RenderListRemove_SDL(Enj_RenderList_SDL *rl, Enj_RenderNode_SDL *rn){
    Enj_ListRemove(&rl->list, &rn->listnode);

    Enj_Free(rl->allocnode, rn);
}


void Enj_RenderList_OnFree_SDL(void *d, void *ctx, Enj_Allocator *a){
    Enj_RenderList_SDL *rl = (Enj_RenderList_SDL *)d;

    Enj_RenderListFree_SDL(rl);
    Enj_Free(a, rl);
}
