#include "renderlist_opengl.h"

#include "../../core/allocator.h"

void Enj_RenderListInit_OpenGL(Enj_RenderList_OpenGL *rl, Enj_Allocator *an){
    Enj_ListReset(&rl->list);
    rl->allocnode = an;
}
void Enj_RenderListFree_OpenGL(Enj_RenderList_OpenGL *rl){
    Enj_ListNode *it = (Enj_ListNode *)rl->list.head;
    while(it){
        Enj_ListNode *itn = it->next;

        Enj_Free(
            rl->allocnode,
            (Enj_RenderNode_OpenGL *)
                ((char *)it - offsetof(Enj_RenderNode_OpenGL, listnode))
            );

        it = itn;
    }
    Enj_ListReset(&rl->list);
}
Enj_RenderNode_OpenGL * Enj_RenderListAppend_OpenGL(Enj_RenderList_OpenGL *rl){
    Enj_RenderNode_OpenGL * res = (Enj_RenderNode_OpenGL *)
            Enj_Alloc(rl->allocnode, sizeof(Enj_RenderNode_OpenGL));
    if(!res) return NULL;

    Enj_ListAppend(&rl->list, &res->listnode);

    res->priority = 0;
    return res;
}
void Enj_RenderListRemove_OpenGL(Enj_RenderList_OpenGL *rl, Enj_RenderNode_OpenGL *rn){
    Enj_ListRemove(&rl->list, &rn->listnode);

    Enj_Free(rl->allocnode, rn);
}


void Enj_RenderList_OnFree_OpenGL(void *d, void *ctx, Enj_Allocator *a){
    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)d;

    Enj_RenderListFree_OpenGL(rl);
    Enj_Free(a, rl);
}
