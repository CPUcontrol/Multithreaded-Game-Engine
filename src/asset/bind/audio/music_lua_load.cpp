#include <condition_variable>
#include <mutex>
#include <string>

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
    if(!lua_isstring(L, 2)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    music_binder *ctx = (music_binder *)la->ctx;
    Enj_Music *e = (Enj_Music *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Music));
    if(!e) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_POOL);
        return 2;
    }

    {
        std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
        ctx->dispatch.mq.q.push([la, e, path = ctx->basepath + lua_tostring(L, 2), ctx](){
            e->music = Mix_LoadMUS(path.c_str());
            if(!e->music) {
                luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
                Enj_Free(&ctx->alloc, e);
            }
            else {
                luafinishpreloadasset(ctx->Lmain, la, ASSET_OK);
            }
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
