#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Instream Enj_Instream;

typedef struct Enj_PNGLoader{
    unsigned int width;
    unsigned int height;
    int bit_depth;
    int color_type;

    void *png_ptr;
    void *info_ptr;
} Enj_PNGLoader;

int Enj_OpenPNG(Enj_PNGLoader *p, Enj_Instream *rs);

int Enj_ReadPNG(Enj_PNGLoader *p, void *buf, size_t size, size_t pitch);

void Enj_ClosePNG(Enj_PNGLoader *p);

#ifdef __cplusplus
}
#endif
