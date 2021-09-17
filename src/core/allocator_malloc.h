#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Allocator Enj_Allocator;

void Enj_InitMallocAllocator(
    Enj_Allocator *a);

#ifdef __cplusplus
}
#endif
