#include "renderlist.h"

#include "../../core/allocator.h"

void Enj_RenderListInit(Enj_RenderList *rl, Enj_Allocator *an){
    Enj_ListReset(&rl->list);
    rl->allocnode = an;
    rl->xoffset = 0;
    rl->yoffset = 0;
}
void Enj_RenderListFree(Enj_RenderList *rl){
    Enj_ListNode *it = (Enj_ListNode *)rl->list.head;
    while(it){
        Enj_ListNode *itn = it->next;

        Enj_Free(
            rl->allocnode,
            (Enj_RenderNode *)
                ((char *)it - offsetof(Enj_RenderNode, listnode))
            );

        it = itn;
    }
    Enj_ListReset(&rl->list);
}
Enj_RenderNode * Enj_RenderListAppend(Enj_RenderList *rl){
    Enj_RenderNode * res = (Enj_RenderNode *)
            Enj_Alloc(rl->allocnode, sizeof(Enj_RenderNode));
    if(!res) return NULL;

    Enj_ListAppend(&rl->list, &res->listnode);

    res->priority = 0;
    return res;
}
void Enj_RenderListRemove(Enj_RenderList *rl, Enj_RenderNode *rn){
    Enj_ListRemove(&rl->list, &rn->listnode);

    Enj_Free(rl->allocnode, rn);
}


void Enj_RenderList_OnFree(void *d, void *ctx, Enj_Allocator *a){
    Enj_RenderList *rl = (Enj_RenderList *)d;

    Enj_RenderListFree(rl);
    Enj_Free(a, rl);
}
