#pragma once

#include <GL/glcorearb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Renderer_OpenGL {
    size_t numsprite;
    size_t numline;
    size_t numquad;

    GLuint vao_sprite;
    GLuint vbo_sprite;

    GLuint vao_line;
    GLuint vbo_line;

    GLuint vao_quad;
    GLuint vbo_quad;

    size_t batchsize;
    GLuint curtexture;
    char curdraw;

} Enj_Renderer_OpenGL;

#ifdef __cplusplus
}
#endif
