#pragma once

#include "instream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_ZIPLoader{
    char* entry_name;
    unsigned short entry_size;


    FILE *file_central, *file_local;
    void *z_ptr;
    unsigned long file_sizecomp, file_sizeuncomp;

    unsigned int dataoffset;
    unsigned int file_method;

} Enj_ZIPLoader;

int Enj_OpenZIP(Enj_ZIPLoader *p, const char *filename);

Enj_Instream * Enj_ZIPGetInstream(Enj_ZIPLoader *p);

void Enj_ZIPNext(Enj_ZIPLoader *p);

void Enj_CloseZIP(Enj_ZIPLoader *p);

#ifdef __cplusplus
}
#endif
