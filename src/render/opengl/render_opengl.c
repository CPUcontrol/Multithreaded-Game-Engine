#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl3w.h>

#include "render_opengl.h"
#include "sprite_opengl.h"
#include "primrect_opengl.h"
#include "renderlist_opengl.h"

#include "../../core/allocator.h"
#include "../../core/graphics/opengl/glyph_opengl.h"
#include "../../core/graphics/opengl/texture_opengl.h"

#include "shadersource_opengl.h"

struct vertexsprite{
    GLfloat u_ul;
    GLfloat v_ul;
    GLfloat u_dr;
    GLfloat v_dr;

    GLfloat x;
    GLfloat y;

    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
};

struct vertexcolor{
    GLfloat x;
    GLfloat y;

    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
};



Enj_Renderer_OpenGL * Enj_InitRenderer_OpenGL(){
    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)
        malloc(sizeof(Enj_Renderer_OpenGL));

    if (!rend) return NULL;

    rend->numsprite = 0;
    rend->numrectline = 0;
    rend->numrectfill = 0;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLfloat ortho[16] = {
        2.f / viewport[2],
        0.f,
        0.f,
        0.f,

        0.f,
        -2.f / viewport[3],
        0.f,
        0.f,

        0.f,
        0.f,
        1.f,
        0.f,

        -1.f,
        1.f,
        0.f,
        1.f
    };
    glGenBuffers(1, &rend->ubo_transform);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, rend->ubo_transform);
    glBufferData(
        GL_UNIFORM_BUFFER,
        16 * sizeof(GLfloat),
        ortho,
        GL_DYNAMIC_DRAW
    );

    glGenVertexArrays(1, &rend->vao_sprite);
    glGenBuffers(1, &rend->vbo_sprite);
    glGenBuffers(1, &rend->ibo_sprite);

    glBindVertexArray(rend->vao_sprite);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend->ibo_sprite);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_sprite);
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE,
        sizeof(struct vertexsprite),
        (void *)offsetof(struct vertexsprite, u_ul)
    );
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        sizeof(struct vertexsprite),
        (void *)offsetof(struct vertexsprite, x)
    );
    glVertexAttribPointer(
        2, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(struct vertexsprite),
        (void *)offsetof(struct vertexsprite, r)
    );

    GLuint vertshadersprite = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragshadersprite = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertshadersprite, 1, &vertexshadersource_sprite, 0);
    glShaderSource(fragshadersprite, 1, &fragmentshadersource_sprite, 0);
    glCompileShader(vertshadersprite);
    glCompileShader(fragshadersprite);
    GLuint progsprite = glCreateProgram();
    progsprite = glCreateProgram();
    glBindAttribLocation(progsprite, 0, "sprite_glyph");
    glBindAttribLocation(progsprite, 1, "vertex_position");
    glBindAttribLocation(progsprite, 2, "vertex_color");
    glAttachShader(progsprite, vertshadersprite);
    glAttachShader(progsprite, fragshadersprite);
    glLinkProgram(progsprite);
    glDetachShader(progsprite, vertshadersprite);
    glDetachShader(progsprite, fragshadersprite);
    glDeleteShader(vertshadersprite);
    glDeleteShader(fragshadersprite);
    glUniformBlockBinding(
        progsprite,
        glGetUniformBlockIndex(progsprite, "Transform"),
        0
    );
    rend->program_sprite = progsprite;


    glGenVertexArrays(1, &rend->vao_color);
    glGenBuffers(1, &rend->vbo_color);
    glGenBuffers(1, &rend->ibo_color);

    glBindVertexArray(rend->vao_color);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend->ibo_color);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_color);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        sizeof(struct vertexcolor),
        (void *)offsetof(struct vertexcolor, x)
    );
    glVertexAttribPointer(
        1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(struct vertexcolor),
        (void *)offsetof(struct vertexcolor, r)
    );
    GLuint vertshadercolor = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragshadercolor = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertshadercolor, 1, &vertexshadersource_color, 0);
    glShaderSource(fragshadercolor, 1, &fragmentshadersource_color, 0);
    glCompileShader(vertshadercolor);
    glCompileShader(fragshadercolor);
    GLuint progcolor = glCreateProgram();
    glBindAttribLocation(progcolor, 0, "vertex_position");
    glBindAttribLocation(progcolor, 1, "vertex_color");
    glAttachShader(progcolor, vertshadercolor);
    glAttachShader(progcolor, fragshadercolor);
    glLinkProgram(progcolor);
    glDetachShader(progcolor, vertshadercolor);
    glDetachShader(progcolor, fragshadercolor);
    glDeleteShader(vertshadercolor);
    glDeleteShader(fragshadercolor);
    glUniformBlockBinding(
        progcolor,
        glGetUniformBlockIndex(progcolor, "Transform"),
        0
    );
    rend->program_color = progcolor;

    return rend;
}
void Enj_FreeRenderer_OpenGL(Enj_Renderer_OpenGL *rend){
    glDeleteVertexArrays(1, &rend->vao_sprite);
    glDeleteVertexArrays(1, &rend->vao_color);
    glDeleteBuffers(1, &rend->vbo_sprite);
    glDeleteBuffers(1, &rend->ibo_sprite);
    glDeleteBuffers(1, &rend->vbo_color);
    glDeleteBuffers(1, &rend->ibo_color);
    glDeleteBuffers(1, &rend->ubo_transform);
    glDeleteProgram(rend->program_sprite);
    glDeleteProgram(rend->program_color);

    free(rend);
}

void Enj_RendererBegin_OpenGL(Enj_Renderer_OpenGL *rend){
    rend->curdraw = 0;
    rend->batchsize = 0;

    rend->vbo_sprite_offset = 0;
    rend->ibo_sprite_offset = 0;
    rend->vbo_color_offset = 0;
    rend->ibo_color_offset = 0;

    rend->idxsprite = 0;
    rend->idxcolor = 0;

    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_sprite);
    glBufferData(
        GL_ARRAY_BUFFER,
        rend->numsprite * 4 * sizeof(struct vertexsprite),
        0,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_color);
    glBufferData(
        GL_ARRAY_BUFFER,
        (rend->numrectline + rend->numrectfill)
            * 4 * sizeof(struct vertexcolor),
        0,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(rend->vao_sprite);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        rend->numsprite * 6 * sizeof(GLuint),
        0,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(rend->vao_color);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        (rend->numrectfill*6 + rend->numrectline*8) * sizeof(GLuint),
        0,
        GL_DYNAMIC_DRAW
    );
}

#define APP_ENUM_RENDERER_OPENGL_SPRITE 1
#define APP_ENUM_RENDERER_OPENGL_RECTFILL 2
#define APP_ENUM_RENDERER_OPENGL_RECTLINE 3

static void flushrenderersprite(Enj_Renderer_OpenGL *rend);
static void flushrendererrectfill(Enj_Renderer_OpenGL *rend);
static void flushrendererrectline(Enj_Renderer_OpenGL *rend);

void Enj_RendererFlush_OpenGL(Enj_Renderer_OpenGL *rend){
    switch(rend->curdraw){
    case 0:
        break;
    case APP_ENUM_RENDERER_OPENGL_SPRITE:
        flushrenderersprite(rend);
        break;
    case APP_ENUM_RENDERER_OPENGL_RECTFILL:
        flushrendererrectfill(rend);
        break;
    case APP_ENUM_RENDERER_OPENGL_RECTLINE:
        flushrendererrectline(rend);
        break;
    }
}

static void flushrenderersprite(Enj_Renderer_OpenGL *rend){
    glFlushMappedBufferRange(
        GL_ARRAY_BUFFER,
        0,
        rend->batchsize * 4 * sizeof(struct vertexsprite)
    );
    GLboolean success_vbo = glUnmapBuffer(GL_ARRAY_BUFFER);

    glFlushMappedBufferRange(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        rend->batchsize * 6 * sizeof(GLuint)
    );
    GLboolean success_ibo = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    if (success_vbo & success_ibo)
        glDrawElements(
            GL_TRIANGLES,
            rend->batchsize * 6,
            GL_UNSIGNED_INT,
            (void *)rend->ibo_sprite_offset
        );

    rend->vbo_sprite_offset += rend->batchsize
                                * 4 * sizeof(struct vertexsprite);
    rend->ibo_sprite_offset += rend->batchsize
                                * 6 *sizeof(GLuint);
    rend->batchsize = 0;
}

static void flushrendererrectfill(Enj_Renderer_OpenGL *rend){
    glFlushMappedBufferRange(
        GL_ARRAY_BUFFER,
        0,
        rend->batchsize * 4 * sizeof(struct vertexcolor)
    );
    GLboolean success_vbo = glUnmapBuffer(GL_ARRAY_BUFFER);

    glFlushMappedBufferRange(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        rend->batchsize * 6 * sizeof(GLuint)
    );
    GLboolean success_ibo = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    if (success_vbo & success_ibo)
        glDrawElements(
            GL_TRIANGLES,
            rend->batchsize * 6,
            GL_UNSIGNED_INT,
            (void *)rend->ibo_color_offset
        );

    rend->vbo_color_offset += rend->batchsize
                            * 4 * sizeof(struct vertexcolor);
    rend->ibo_color_offset += rend->batchsize
                            * 6 * sizeof(GLuint);
    rend->batchsize = 0;
}

static void flushrendererrectline(Enj_Renderer_OpenGL *rend){
    glFlushMappedBufferRange(
        GL_ARRAY_BUFFER,
        0,
        rend->batchsize * 4 * sizeof(struct vertexcolor)
    );
    GLboolean success_vbo = glUnmapBuffer(GL_ARRAY_BUFFER);

    glFlushMappedBufferRange(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        rend->batchsize * 8 * sizeof(GLuint)
    );
    GLboolean success_ibo = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    if (success_vbo & success_ibo)
        glDrawElements(
            GL_LINES,
            rend->batchsize * 8,
            GL_UNSIGNED_INT,
            (void *)rend->ibo_color_offset
        );

    rend->vbo_color_offset += rend->batchsize
                            * 4 * sizeof(struct vertexcolor);
    rend->ibo_color_offset += rend->batchsize
                            * 8 * sizeof(GLuint);
    rend->batchsize = 0;
}

static void resumespritebatch(Enj_Renderer_OpenGL *rend){
    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_sprite);
    rend->mapvbo = glMapBufferRange(
        GL_ARRAY_BUFFER,
        rend->vbo_sprite_offset,
        rend->numsprite * 4 * sizeof(struct vertexsprite)
            - rend->vbo_sprite_offset,

        GL_MAP_WRITE_BIT |
        GL_MAP_FLUSH_EXPLICIT_BIT |
        GL_MAP_UNSYNCHRONIZED_BIT
    );

    rend->mapibo = glMapBufferRange(
        GL_ELEMENT_ARRAY_BUFFER,
        rend->ibo_sprite_offset,
        rend->numsprite * 6 * sizeof(GLuint) - rend->ibo_sprite_offset,

        GL_MAP_WRITE_BIT |
        GL_MAP_FLUSH_EXPLICIT_BIT |
        GL_MAP_UNSYNCHRONIZED_BIT
    );
}

static void resumecolorbatch(Enj_Renderer_OpenGL *rend){
    glBindBuffer(GL_ARRAY_BUFFER, rend->vbo_color);
    rend->mapvbo = glMapBufferRange(
        GL_ARRAY_BUFFER,
        rend->vbo_color_offset,
        (rend->numrectline + rend->numrectfill) * 4 * sizeof(struct vertexcolor)
            - rend->vbo_color_offset,

        GL_MAP_WRITE_BIT |
        GL_MAP_FLUSH_EXPLICIT_BIT |
        GL_MAP_UNSYNCHRONIZED_BIT
    );

    rend->mapibo = glMapBufferRange(
        GL_ELEMENT_ARRAY_BUFFER,
        rend->ibo_color_offset,
        (rend->numrectfill*6 + rend->numrectline*8)*sizeof(GLuint)
            - rend->ibo_color_offset,

        GL_MAP_WRITE_BIT |
        GL_MAP_FLUSH_EXPLICIT_BIT |
        GL_MAP_UNSYNCHRONIZED_BIT
    );
}

void Enj_Sprite_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
)
{
    Enj_Sprite_OpenGL *sprite = (Enj_Sprite_OpenGL *)d;
    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)ctx;

    if(rend->curdraw != APP_ENUM_RENDERER_OPENGL_SPRITE){
        Enj_RendererFlush_OpenGL(rend);
        glBindVertexArray(rend->vao_sprite);
        glUseProgram(rend->program_sprite);
        resumespritebatch(rend);
        glBindTexture(GL_TEXTURE_2D, sprite->glyph->texture->id);
        rend->curdraw = APP_ENUM_RENDERER_OPENGL_SPRITE;
        rend->curtexture = sprite->glyph->texture->id;
    }
    else if(sprite->glyph->texture->id != rend->curtexture){
        flushrenderersprite(rend);
        resumespritebatch(rend);
        glBindTexture(GL_TEXTURE_2D, sprite->glyph->texture->id);
        rend->curtexture = sprite->glyph->texture->id;
    }

    unsigned char i1 = (sprite->glyph->fliprotate >> 0) & 3;
    unsigned char i2 = (sprite->glyph->fliprotate >> 2) & 3;
    unsigned char i3 = (sprite->glyph->fliprotate >> 4) & 3;
    unsigned char i4 = (sprite->glyph->fliprotate >> 6) & 3;

    float xl = sprite->x - sprite->xcen;
    float xr = xl + sprite->w;
    float yu = sprite->y - sprite->ycen;
    float yd = yu + sprite->h;

    float cs = cos(sprite->angle);
    float si = sin(sprite->angle);

    //Clockwise rotation when viewed from below Z plane
    float ul_x = cs * xl - si * yu;
    float ul_y = si * xl + cs * yu;

    float ur_x = cs * xr - si * yu;
    float ur_y = si * xr + cs * yu;

    float dl_x = cs * xl - si * yd;
    float dl_y = si * xl + cs * yd;

    float dr_x = cs * xr - si * yd;
    float dr_y = si * xr + cs * yd;


    struct vertexsprite *vdata = (struct vertexsprite *)rend->mapvbo;
    GLuint *idata = (GLuint *)rend->mapibo;


    vdata[0].u_ul = sprite->glyph->u_ul;
    vdata[0].v_ul = sprite->glyph->v_ul;
    vdata[0].u_dr = sprite->glyph->u_dr;
    vdata[0].v_dr = sprite->glyph->v_dr;
    vdata[0].r = sprite->r;
    vdata[0].g = sprite->g;
    vdata[0].b = sprite->b;
    vdata[0].a = sprite->a;

    vdata[1].u_ul = sprite->glyph->u_ul;
    vdata[1].v_ul = sprite->glyph->v_ul;
    vdata[1].u_dr = sprite->glyph->u_dr;
    vdata[1].v_dr = sprite->glyph->v_dr;
    vdata[1].r = sprite->r;
    vdata[1].g = sprite->g;
    vdata[1].b = sprite->b;
    vdata[1].a = sprite->a;

    vdata[2].u_ul = sprite->glyph->u_ul;
    vdata[2].v_ul = sprite->glyph->v_ul;
    vdata[2].u_dr = sprite->glyph->u_dr;
    vdata[2].v_dr = sprite->glyph->v_dr;
    vdata[2].r = sprite->r;
    vdata[2].g = sprite->g;
    vdata[2].b = sprite->b;
    vdata[2].a = sprite->a;

    vdata[3].u_ul = sprite->glyph->u_ul;
    vdata[3].v_ul = sprite->glyph->v_ul;
    vdata[3].u_dr = sprite->glyph->u_dr;
    vdata[3].v_dr = sprite->glyph->v_dr;
    vdata[3].r = sprite->r;
    vdata[3].g = sprite->g;
    vdata[3].b = sprite->b;
    vdata[3].a = sprite->a;


    vdata[i1].x = m2[0] * ul_x + m2[2] * ul_y + v2[0];
    vdata[i1].y = m2[1] * ul_x + m2[3] * ul_y + v2[1];

    vdata[i2].x = m2[0] * ur_x + m2[2] * ur_y + v2[0];
    vdata[i2].y = m2[1] * ur_x + m2[3] * ur_y + v2[1];

    vdata[i3].x = m2[0] * dl_x + m2[2] * dl_y + v2[0];
    vdata[i3].y = m2[1] * dl_x + m2[3] * dl_y + v2[1];

    vdata[i4].x = m2[0] * dr_x + m2[2] * dr_y + v2[0];
    vdata[i4].y = m2[1] * dr_x + m2[3] * dr_y + v2[1];

    idata[0] = rend->idxsprite + i1;
    idata[1] = rend->idxsprite + i3;
    idata[2] = rend->idxsprite + i2;
    idata[3] = rend->idxsprite + i2;
    idata[4] = rend->idxsprite + i3;
    idata[5] = rend->idxsprite + i4;


    rend->mapvbo = (struct vertexsprite *) rend->mapvbo + 4;
    rend->mapibo = (GLuint *)              rend->mapibo + 6;
    ++rend->batchsize;
    rend->idxsprite += 4;
}

void Enj_PrimRectFill_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
)
{
    Enj_PrimRect_OpenGL *pr = (Enj_PrimRect_OpenGL *)d;
    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)ctx;

    if(rend->curdraw != APP_ENUM_RENDERER_OPENGL_RECTFILL){
        Enj_RendererFlush_OpenGL(rend);
        //Same VAO check
        if(rend->curdraw != APP_ENUM_RENDERER_OPENGL_RECTLINE){
            glBindVertexArray(rend->vao_color);
            glUseProgram(rend->program_color);
        }

        resumecolorbatch(rend);
        rend->curdraw = APP_ENUM_RENDERER_OPENGL_RECTFILL;
    }

    float xl = pr->x - pr->xcen;
    float xr = xl + pr->w;
    float yu = pr->y - pr->ycen;
    float yd = yu + pr->h;

    float cs = cos(pr->angle);
    float si = sin(pr->angle);


    float ul_x = cs * xl - si * yu;
    float ul_y = si * xl + cs * yu;

    float ur_x = cs * xr - si * yu;
    float ur_y = si * xr + cs * yu;

    float dl_x = cs * xl - si * yd;
    float dl_y = si * xl + cs * yd;

    float dr_x = cs * xr - si * yd;
    float dr_y = si * xr + cs * yd;


    struct vertexcolor *vdata = (struct vertexcolor *)rend->mapvbo;
    GLuint *idata = (GLuint *)rend->mapibo;

    vdata[0].x = m2[0] * ul_x + m2[2] * ul_y + v2[0];
    vdata[0].y = m2[1] * ul_x + m2[3] * ul_y + v2[1];
    vdata[0].r = pr->r;
    vdata[0].g = pr->g;
    vdata[0].b = pr->b;
    vdata[0].a = pr->a;

    vdata[1].x = m2[0] * ur_x + m2[2] * ur_y + v2[0];
    vdata[1].y = m2[1] * ur_x + m2[3] * ur_y + v2[1];
    vdata[1].r = pr->r;
    vdata[1].g = pr->g;
    vdata[1].b = pr->b;
    vdata[1].a = pr->a;

    vdata[2].x = m2[0] * dl_x + m2[2] * dl_y + v2[0];
    vdata[2].y = m2[1] * dl_x + m2[3] * dl_y + v2[1];
    vdata[2].r = pr->r;
    vdata[2].g = pr->g;
    vdata[2].b = pr->b;
    vdata[2].a = pr->a;

    vdata[3].x = m2[0] * dr_x + m2[2] * dr_y + v2[0];
    vdata[3].y = m2[1] * dr_x + m2[3] * dr_y + v2[1];
    vdata[3].r = pr->r;
    vdata[3].g = pr->g;
    vdata[3].b = pr->b;
    vdata[3].a = pr->a;


    idata[0] = rend->idxcolor + 0;
    idata[1] = rend->idxcolor + 2;
    idata[2] = rend->idxcolor + 1;
    idata[3] = rend->idxcolor + 1;
    idata[4] = rend->idxcolor + 2;
    idata[5] = rend->idxcolor + 3;


    rend->mapvbo = (struct vertexcolor *)  rend->mapvbo + 4;
    rend->mapibo = (GLuint *)              rend->mapibo + 6;
    ++rend->batchsize;
    rend->idxcolor += 4;
}

void Enj_PrimRectLine_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
)
{
    Enj_PrimRect_OpenGL *pr = (Enj_PrimRect_OpenGL *)d;
    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)ctx;

    if(rend->curdraw != APP_ENUM_RENDERER_OPENGL_RECTLINE){
        Enj_RendererFlush_OpenGL(rend);
        //Same VAO check
        if(rend->curdraw != APP_ENUM_RENDERER_OPENGL_RECTFILL){
            glBindVertexArray(rend->vao_color);
            glUseProgram(rend->program_color);
        }
        resumecolorbatch(rend);
        rend->curdraw = APP_ENUM_RENDERER_OPENGL_RECTLINE;
    }

    float xl = pr->x - pr->xcen;
    float xr = xl + pr->w;
    float yu = pr->y - pr->ycen;
    float yd = yu + pr->h;

    //Offset by 0.5 to align with center of pixel
    xl += 0.5;
    xr -= 0.5;
    yu += 0.5;
    yd -= 0.5;

    float cs = cos(pr->angle);
    float si = sin(pr->angle);


    float ul_x = cs * xl - si * yu;
    float ul_y = si * xl + cs * yu;

    float ur_x = cs * xr - si * yu;
    float ur_y = si * xr + cs * yu;

    float dl_x = cs * xl - si * yd;
    float dl_y = si * xl + cs * yd;

    float dr_x = cs * xr - si * yd;
    float dr_y = si * xr + cs * yd;


    struct vertexcolor *vdata = (struct vertexcolor *)rend->mapvbo;
    GLuint *idata = (GLuint *)rend->mapibo;

    vdata[0].x = m2[0] * ul_x + m2[2] * ul_y + v2[0];
    vdata[0].y = m2[1] * ul_x + m2[3] * ul_y + v2[1];
    vdata[0].r = pr->r;
    vdata[0].g = pr->g;
    vdata[0].b = pr->b;
    vdata[0].a = pr->a;

    vdata[1].x = m2[0] * ur_x + m2[2] * ur_y + v2[0];
    vdata[1].y = m2[1] * ur_x + m2[3] * ur_y + v2[1];
    vdata[1].r = pr->r;
    vdata[1].g = pr->g;
    vdata[1].b = pr->b;
    vdata[1].a = pr->a;

    vdata[2].x = m2[0] * dl_x + m2[2] * dl_y + v2[0];
    vdata[2].y = m2[1] * dl_x + m2[3] * dl_y + v2[1];
    vdata[2].r = pr->r;
    vdata[2].g = pr->g;
    vdata[2].b = pr->b;
    vdata[2].a = pr->a;

    vdata[3].x = m2[0] * dr_x + m2[2] * dr_y + v2[0];
    vdata[3].y = m2[1] * dr_x + m2[3] * dr_y + v2[1];
    vdata[3].r = pr->r;
    vdata[3].g = pr->g;
    vdata[3].b = pr->b;
    vdata[3].a = pr->a;


    idata[0] = rend->idxcolor + 0;
    idata[1] = rend->idxcolor + 1;
    idata[2] = rend->idxcolor + 1;
    idata[3] = rend->idxcolor + 3;
    idata[4] = rend->idxcolor + 3;
    idata[5] = rend->idxcolor + 2;
    idata[6] = rend->idxcolor + 2;
    idata[7] = rend->idxcolor + 0;


    rend->mapvbo = (struct vertexcolor *)  rend->mapvbo + 4;
    rend->mapibo = (GLuint *)              rend->mapibo + 8;
    ++rend->batchsize;
    rend->idxcolor += 4;
}

static int rendernodecmp(Enj_ListNode *a, Enj_ListNode *b){
    Enj_RenderNode_OpenGL *nodea = (Enj_RenderNode_OpenGL *)
            ((char *)a - offsetof(Enj_RenderNode_OpenGL, listnode));

    Enj_RenderNode_OpenGL *nodeb = (Enj_RenderNode_OpenGL *)
            ((char *)b - offsetof(Enj_RenderNode_OpenGL, listnode));

    return   (nodea->priority > nodeb->priority)
            -(nodea->priority < nodeb->priority);
}

void Enj_RenderList_OnRender_OpenGL(
    void *d,
    void *ctx,
    const float *m2,
    const float *v2
)
{
    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)d;
    Enj_ListSort(&rl->list, &rendernodecmp);

    float cs = cos(rl->angle);
    float si = sin(rl->angle);

    float w = rl->scalex;
    float h = rl->scaley;

    float x = rl->translatex;
    float y = rl->translatey;

    float m2n[4] = {
        (m2[0] * cs + m2[2] * si) * w,
        (m2[1] * cs + m2[3] * si) * w,

        (m2[0] *-si + m2[2] * cs) * h,
        (m2[1] *-si + m2[3] * cs) * h
    };
    float v2n[2] = {
        m2[0] * x + m2[2] * y + v2[0],
        m2[1] * x + m2[3] * y + v2[1],
    };

    Enj_ListNode *it = rl->list.head;
    while(it){
        Enj_RenderNode_OpenGL *node = (Enj_RenderNode_OpenGL *)
            ((char *)it - offsetof(Enj_RenderNode_OpenGL, listnode));
        if (node->active)
            (*node->onrender)(
                node->data,
                node->ctx,
                m2n,
                v2n
            );
        it = it->next;
    }

}
