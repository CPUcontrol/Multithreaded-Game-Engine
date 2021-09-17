#pragma once
#include "../../core/allocator.h"

typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <filesystem>

class texture_binder{
private:
    void *buf;
    Enj_PoolAllocatorData dat;
public:
    Enj_Allocator alloc;
    multi_dispatch &dispatch;
    std::filesystem::path &basepath;

    SDL_Renderer *rend;
    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    texture_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L
    );
    ~texture_binder();

    texture_binder(const texture_binder &o) = delete;
    texture_binder & operator=(const texture_binder &o) = delete;

    texture_binder(texture_binder &&o) = delete;
    texture_binder & operator=(texture_binder &&o) = delete;


    operator bool(){return buf;}
};
