#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Arraylist{
    void **data;
    size_t size, capacity;
} Enj_Arraylist;


int Enj_InitArraylist(Enj_Arraylist *al);
void Enj_FreeArraylist(Enj_Arraylist *al);

int Enj_ArraylistPush(Enj_Arraylist *al, void *e);

#ifdef __cplusplus
}
#endif
