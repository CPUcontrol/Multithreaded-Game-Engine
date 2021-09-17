#include <stdlib.h>
#include <string.h>

#include "arraylist.h"


int Enj_InitArraylist(Enj_Arraylist *al){
    al->data = (void **)malloc(8*sizeof(void *));
    al->size = 0;
    al->capacity = 8;

    if(!al->data) return 1;

    return 0;
}
void Enj_FreeArraylist(Enj_Arraylist *al){
    free(al->data);
}

int Enj_ArraylistPush(Enj_Arraylist *al, void *e){
    if(al->size == al->capacity){
        void **newbuf = (void **)malloc(al->capacity*2*sizeof(void *));
        if(!newbuf) return 1;

        memcpy(newbuf, al->data, al->capacity*sizeof(void *));
        free(al->data);
        al->data = newbuf;
        al->capacity *= 2;
    }

    al->data[al->size++] = e;
    return 0;
}
