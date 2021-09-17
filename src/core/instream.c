#include <stdio.h>
#include <string.h>
#include "instream.h"

size_t Enj_ReadBytes(Enj_Instream *is, void *buf, size_t n){
    return (*is->onread)(buf, n, &is->bytes, is->data);
}

void Enj_FreeInstream(Enj_Instream *is){
    (*is->onfree)(is->data);
}


static void freeinstreamfile(void *data){
    fclose((FILE *)data);
}
static size_t readinstreamfile(void *buf, size_t size, size_t *bytes, void *data){
    return fread(buf, 1, size, (FILE *)data);
}

int Enj_InitInstreamFromFile(Enj_Instream *is, const char *file){
    FILE *f = fopen(file, "rb");

    if(!f) return 1;
    is->data = f;
    is->bytes = 0;
    is->onfree = freeinstreamfile;
    is->onread = readinstreamfile;
    return 0;
}

static void freeinstreammemory(void *data){

}
static size_t readinstreammemory(void *buf, size_t size, size_t *bytes, void *data){
    size_t bytesread = size < *bytes ? size : *bytes;
    memcpy(buf, data, bytesread);
    *bytes -= bytesread;
    return bytesread;
}

int Enj_InitInstreamFromMemory(Enj_Instream *is, const void *buf, size_t n){
    is->data = (void *)buf;
    is->bytes = n;
    is->onfree = freeinstreammemory;
    is->onread = readinstreammemory;
    return 0;
}
