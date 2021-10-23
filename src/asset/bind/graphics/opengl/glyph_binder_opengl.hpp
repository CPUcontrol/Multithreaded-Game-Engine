#pragma once
#include "../../../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;

class glyph_binder_OpenGL{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    lua_State *Lmain;

    glyph_binder_OpenGL(multi_dispatch &md, lua_State *L);
    ~glyph_binder_OpenGL();

    glyph_binder_OpenGL(const glyph_binder_OpenGL &o) = delete;
    glyph_binder_OpenGL & operator=(const glyph_binder_OpenGL &o) = delete;

    glyph_binder_OpenGL(glyph_binder_OpenGL &&o) = delete;
    glyph_binder_OpenGL & operator=(glyph_binder_OpenGL &&o) = delete;


    operator bool(){return buf;}
};
