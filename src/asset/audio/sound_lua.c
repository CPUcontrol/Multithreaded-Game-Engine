#include <stddef.h>

#include <SDL_mixer.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sound_lua.h"

#include "../../core/sound.h"
#include "../../core/lua_extra.h"

#include "../lua_extra_asset.h"
#include "../luaasset.h"

static int luaplaysound(lua_State *L){

    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");
    lua_getfield(L, tmpidx+2, "sound");
    lua_getmetatable(L, 1);

    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Sound *e = (Enj_Sound *)la->data;

    Mix_PlayChannel(-1, e->chunk, 0);

    return 0;
}

void bindsound(lua_State *L, void *ctx,
    int (*onpreload)(lua_State *),
    int (*onunload)(lua_State *),
    int (*oncanunload)(lua_State *)
    ){

    lua_register(L, "play_sound", luaplaysound);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "asset");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 0);

    //gets
    lua_createtable(L, 0, 0);
    lua_pushcclosure(L, Enj_Lua_GetAssetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 0);
    lua_pushcclosure(L, Enj_Lua_SetAssetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "sound");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bind 3 create functions + context + metatable + num values into closure
    lua_pushcfunction(L, onpreload);
    lua_pushcfunction(L, onunload);
    lua_pushcfunction(L, oncanunload);
    lua_pushlightuserdata(L, ctx);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, 0);
    lua_pushcclosure(L, Enj_Lua_CreateAsset, 6);
    lua_setglobal(L, "create_sound");

    //Bidirectional relation asset table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);


    lua_settop(L, 0);
}
