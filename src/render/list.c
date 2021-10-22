#include "list.h"

void Enj_ListReset(Enj_List *ls){
    ls->head = NULL;
    ls->tail = NULL;
    ls->size = 0;
}
void Enj_ListAppend(Enj_List *ls, Enj_ListNode *node){
    if(ls->size == 0){
        ls->head = node;
        ls->tail = node;

        node->prev = NULL;
        node->next = NULL;
    }
    else{
        node->prev = ls->tail;
        node->next = NULL;

        ls->tail->next = node;
        ls->tail = node;
    }

    ++ls->size;
}
void Enj_ListRemove(Enj_List *ls, Enj_ListNode *node){
    if(node == ls->head){
        ls->head = node->next;
    }
    else{
        node->prev->next = node->next;
    }

    if(node == ls->tail){
        ls->tail = node->prev;
    }
    else{
        node->next->prev = node->prev;
    }

    --ls->size;
}


static void mergesort_r(
    Enj_ListNode *beg, size_t sz, Enj_ListComp cmp,
    Enj_ListNode **nh, Enj_ListNode **nt)
{
    if(sz == 1){
        beg->prev = NULL;
        beg->next = NULL;

        *nh = beg;
        *nt = beg;
        return;
    }

    Enj_ListNode *mid = beg;
    size_t sz1 = sz/2;
    for(size_t i = 0; i < sz1; i++){
        mid = mid->next;
    }

    Enj_ListNode *h1;
    Enj_ListNode *t1;

    Enj_ListNode *h2;
    Enj_ListNode *t2;

    mergesort_r(beg, sz1, cmp, &h1, &t1);
    mergesort_r(mid, sz-sz1, cmp, &h2, &t2);

    Enj_ListNode *it;
    if((*cmp)(h1, h2) <= 0){
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
        if((*cmp)(h1, h2) <= 0){
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

void Enj_ListSort(Enj_List *ls, Enj_ListComp cmp){
    //Empty list or single list already sorted
    if(ls->size < 2) return;
    //first check if sorted already
    Enj_ListNode *it = ls->head;
    while(it->next){
        //If not sorted, start to sort
        if((*cmp)(it, it->next) == 1) {
            Enj_ListNode *nh;
            Enj_ListNode *nt;

            mergesort_r(ls->head, ls->size, cmp, &nh, &nt);

            ls->head = nh;
            ls->tail = nt;

            return;
        }

        it = it->next;
    }
}
