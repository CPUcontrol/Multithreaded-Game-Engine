#include <condition_variable>
#include <filesystem>
#include <mutex>

#include <stdlib.h>

#include <SDL_mixer.h>
#include <lua.hpp>

#include "../../../core/allocator.h"
#include "../../../core/act_lua.h"
#include "../../../core/instream.h"
#include "../../../core/music.h"

#include "../../util/multi_dispatch.hpp"

#include "../../asset_codes.h"
#include "../../luaasset.h"

#include "music_binder.hpp"
#include "../music_lua_load.h"

int Enj_Lua_MusicOnPreload(lua_State *L){
    if(!lua_isstring(L, 2)) return 0;

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    music_binder *ctx = (music_binder *)la->ctx;
    Enj_Music *e = (Enj_Music *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Music));
    if(!e) return 0;

    e->music = Mix_LoadMUS(lua_tostring(L, 2));
    if(!e->music) {
        std::lock_guard lock(ctx->dispatch.mq.mtx);
        ctx->dispatch.mq.q.push([la, Lmain = ctx->Lmain](){
            luafinishpreloadasset(Lmain, la, ASSET_ERROR_FILE);
        });
    }
    else {
        std::lock_guard lock(ctx->dispatch.mq.mtx);
        ctx->dispatch.mq.q.push([la, Lmain = ctx->Lmain](){
            luafinishpreloadasset(Lmain, la, ASSET_OK);
        });
    }

    la->data = e;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_MusicOnUnload(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Music *e = (Enj_Music *)la->data;
    music_binder *ctx = (music_binder *)la->ctx;

    Mix_FreeMusic(e->music);
    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_MusicOnCanUnload(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
