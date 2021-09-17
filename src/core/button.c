#include "button.h"
#include "allocator.h"
void Enj_InitButtonList(Enj_ButtonList *sl, Enj_Allocator *a){
    sl->head = NULL;
    sl->tail = NULL;
    sl->alloc = a;

    sl->size = 0;
}
void Enj_FreeButtonList(Enj_ButtonList *sl){
    Enj_Button *it = sl->head;
    while(it){
        Enj_Free(sl->alloc, it);
        it = it->next;
    }
}
Enj_Button * Enj_CreateButton(Enj_ButtonList *sl){
    Enj_Button *res =
        (Enj_Button *)Enj_Alloc(sl->alloc, sizeof(Enj_Button));
    if (!res) return NULL;


    res->prev = sl->tail;
    res->next = NULL;
    if (!sl->head){
        sl->head = res;
    }
    else{
        sl->tail->next = res;
    }


    sl->tail = res;
    sl->size++;
    return res;
}
void Enj_DestroyButton(Enj_ButtonList *sl, Enj_Button *s){
    if(s == sl->head){
        sl->head = s->next;
    }
    else{
        s->prev->next = s->next;
    }

    if(s == sl->tail){
        sl->tail = s->prev;
    }
    else{
        s->next->prev = s->prev;
    }

    sl->size--;
    Enj_Free(sl->alloc, s);
}
Enj_Button * Enj_GetButtonListHead(Enj_ButtonList *sl){
    return sl->head;
}
Enj_Button * Enj_GetButtonListTail(Enj_ButtonList *sl){
    return sl->tail;
}
size_t Enj_GetButtonListSize(Enj_ButtonList *sl){
    return sl->size;
}
