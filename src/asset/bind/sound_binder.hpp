#pragma once
#include "../../core/allocator.h"

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <filesystem>

class sound_binder{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    std::filesystem::path &basepath;

    lua_State *Lmain;

    sound_binder(multi_dispatch &md, std::filesystem::path &base, lua_State *L);
    ~sound_binder();

    sound_binder(const sound_binder &o) = delete;
    sound_binder & operator=(const sound_binder &o) = delete;

    sound_binder(sound_binder &&o) = delete;
    sound_binder & operator=(sound_binder &&o) = delete;


    operator bool(){return buf;}
};
