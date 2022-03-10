#pragma once

#include <setjmp.h>

#include <GL/glcorearb.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Renderer_OpenGL {
    size_t numsprite;
    size_t numrectline;
    size_t numrectfill;

    void *mapvbo;
    void *mapibo;

    GLuint program_sprite;
    GLuint program_color;

    GLuint vao_sprite;
    GLuint vbo_sprite;
    GLuint ibo_sprite;

    GLuint vao_color;
    GLuint vbo_color;
    GLuint ibo_color;

    GLuint ubo_transform;

    size_t vbo_sprite_offset;
    size_t ibo_sprite_offset;
    size_t vbo_color_offset;
    size_t ibo_color_offset;

    size_t idxsprite;
    size_t idxcolor;

    size_t batchsize;
    GLuint curtexture;
    char curdraw;

    jmp_buf startenv;

} Enj_Renderer_OpenGL;

#ifdef __cplusplus
}
#endif
