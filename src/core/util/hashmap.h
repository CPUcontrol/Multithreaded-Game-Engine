#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Hashmap{
    void *data;
    size_t size, logcap;
} Enj_Hashmap;


int Enj_InitHashmap(Enj_Hashmap *m);
void Enj_FreeHashmap(Enj_Hashmap *m);

int Enj_HashmapHas(Enj_Hashmap *m, const char *k);
void * Enj_HashmapGet(Enj_Hashmap *m, const char *k);
int Enj_HashmapPut(Enj_Hashmap *m, const char *k, void *v);
void Enj_HashmapRemove(Enj_Hashmap *m, const char *k);

#ifdef __cplusplus
}
#endif
