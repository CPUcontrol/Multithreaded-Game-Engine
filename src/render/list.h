#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_ListNode Enj_ListNode;

typedef struct Enj_ListNode{
    Enj_ListNode *prev;
    Enj_ListNode *next;
} Enj_ListNode;

typedef struct Enj_List{
    Enj_ListNode *head;
    Enj_ListNode *tail;
    size_t size;
} Enj_List;

typedef int (*Enj_ListComp)(Enj_ListNode *, Enj_ListNode *);

void Enj_ListReset(Enj_List *ls);
void Enj_ListAppend(Enj_List *ls, Enj_ListNode *node);
void Enj_ListRemove(Enj_List *ls, Enj_ListNode *node);
void Enj_ListSort(Enj_List *ls, Enj_ListComp cmp);

#ifdef __cplusplus
}
#endif
