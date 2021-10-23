#pragma once
#include <GL/glcorearb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Texture_OpenGL{
    GLuint id;
    unsigned short width;
    unsigned short height;
}Enj_Texture_OpenGL;

#ifdef __cplusplus
}
#endif
