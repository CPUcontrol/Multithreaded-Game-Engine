#pragma once
#include "../../../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;

class glyph_binder{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    lua_State *Lmain;

    glyph_binder(multi_dispatch &md, lua_State *L);
    ~glyph_binder();

    glyph_binder(const glyph_binder &o) = delete;
    glyph_binder & operator=(const glyph_binder &o) = delete;

    glyph_binder(glyph_binder &&o) = delete;
    glyph_binder & operator=(glyph_binder &&o) = delete;


    operator bool(){return buf;}
};
