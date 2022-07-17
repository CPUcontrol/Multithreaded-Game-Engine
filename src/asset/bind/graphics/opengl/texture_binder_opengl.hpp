#pragma once
#include "../../../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;

class texture_binder_OpenGL{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    const char *basepath;

    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    texture_binder_OpenGL(
        multi_dispatch &md,
        const char *base,
        lua_State *L
    );
    ~texture_binder_OpenGL();

    texture_binder_OpenGL(const texture_binder_OpenGL &o) = delete;
    texture_binder_OpenGL & operator=(const texture_binder_OpenGL &o) = delete;

    texture_binder_OpenGL(texture_binder_OpenGL &&o) = delete;
    texture_binder_OpenGL & operator=(texture_binder_OpenGL &&o) = delete;


    operator bool(){return buf;}
};
