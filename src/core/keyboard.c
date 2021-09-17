#include "keyboard.h"
#include "allocator.h"
void Enj_InitKeyboardList(Enj_KeyboardList *sl, Enj_Allocator *a){
    sl->head = NULL;
    sl->tail = NULL;
    sl->alloc = a;

    sl->size = 0;
}
void Enj_FreeKeyboardList(Enj_KeyboardList *sl){
    Enj_Keyboard *it = sl->head;
    while(it){
        Enj_Free(sl->alloc, it);
        it = it->next;
    }
}
Enj_Keyboard * Enj_CreateKeyboard(Enj_KeyboardList *sl){
    Enj_Keyboard *res =
        (Enj_Keyboard *)Enj_Alloc(sl->alloc, sizeof(Enj_Keyboard));
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
void Enj_DestroyKeyboard(Enj_KeyboardList *sl, Enj_Keyboard *s){
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
Enj_Keyboard * Enj_GetKeyboardListHead(Enj_KeyboardList *sl){
    return sl->head;
}
Enj_Keyboard * Enj_GetKeyboardListTail(Enj_KeyboardList *sl){
    return sl->tail;
}
size_t Enj_GetKeyboardListSize(Enj_KeyboardList *sl){
    return sl->size;
}
