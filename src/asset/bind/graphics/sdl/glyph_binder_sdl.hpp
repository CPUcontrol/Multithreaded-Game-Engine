#pragma once
#include "../../../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;

class glyph_binder_SDL{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    lua_State *Lmain;

    glyph_binder_SDL(multi_dispatch &md, lua_State *L);
    ~glyph_binder_SDL();

    glyph_binder_SDL(const glyph_binder_SDL &o) = delete;
    glyph_binder_SDL & operator=(const glyph_binder_SDL &o) = delete;

    glyph_binder_SDL(glyph_binder_SDL &&o) = delete;
    glyph_binder_SDL & operator=(glyph_binder_SDL &&o) = delete;


    operator bool(){return buf;}
};
