#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Renderer_OpenGL Enj_Renderer_OpenGL;
typedef struct Enj_RenderList_OpenGL Enj_RenderList_OpenGL;

Enj_Renderer_OpenGL * Enj_InitRenderer_OpenGL();
void Enj_FreeRenderer_OpenGL(Enj_Renderer_OpenGL *rend);

void Enj_RendererVisit_OpenGL(
    Enj_Renderer_OpenGL *rend,
    Enj_RenderList_OpenGL *rl
);

void Enj_RendererSetResolution_OpenGL(
    Enj_Renderer_OpenGL *rend,
    unsigned int w,
    unsigned int h
);

#ifdef __cplusplus
}
#endif
