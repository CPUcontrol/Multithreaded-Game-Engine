#include <stdlib.h>

#include "allocator_malloc.h"
#include "allocator.h"

static void * malloc_acate(size_t size, void *data);
static void malloc_decate(void *p, void *data);

void Enj_InitMallocAllocator(Enj_Allocator *a){
    a->alloc = &malloc_acate;
    a->dealloc = &malloc_decate;
}

static void * malloc_acate(size_t size, void *data){
    return malloc(size);
}
static void malloc_decate(void *p, void *data){
    free(p);
}
