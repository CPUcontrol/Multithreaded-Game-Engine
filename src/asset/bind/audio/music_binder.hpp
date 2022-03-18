#pragma once
#include "../../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <string>

class music_binder{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    std::string &basepath;

    lua_State *Lmain;

    music_binder(multi_dispatch &md, std::string &base, lua_State *L);
    ~music_binder();

    music_binder(const music_binder &o) = delete;
    music_binder & operator=(const music_binder &o) = delete;

    music_binder(music_binder &&o) = delete;
    music_binder & operator=(music_binder &&o) = delete;


    operator bool(){return buf;}
};
