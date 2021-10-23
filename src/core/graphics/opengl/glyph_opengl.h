#pragma once
#include <GL/glcorearb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Texture_OpenGL Enj_Texture_OpenGL;

typedef struct Enj_Glyph_OpenGL{
    GLfloat u_ul;
    GLfloat v_ul;
    GLfloat u_dr;
    GLfloat v_dr;

    Enj_Texture_OpenGL *texture;
    unsigned short width;
    unsigned short height;
    unsigned char fliprotate;

}Enj_Glyph_OpenGL;

#ifdef __cplusplus
}
#endif
