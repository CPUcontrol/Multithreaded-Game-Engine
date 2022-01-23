#include <stddef.h>

#include <SDL_mixer.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "music_lua.h"

#include "../../core/music.h"
#include "../../core/lua_extra.h"

#include "../lua_extra_asset.h"
#include "../luaasset.h"

static int luaplaymusic(lua_State *L){

    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");
    lua_getfield(L, tmpidx+2, "music");
    lua_getmetatable(L, 1);

    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Music *e = (Enj_Music *)la->data;

    Mix_PlayMusic(e->music, 1);

    return 0;
}

void bindmusic(lua_State *L, void *ctx,
    int (*onpreload)(lua_State *),
    int (*onunload)(lua_State *),
    int (*oncanunload)(lua_State *)
    ){

    lua_register(L, "play_music", luaplaymusic);

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
    lua_pushliteral(L, "music");
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
    lua_setglobal(L, "create_music");

    //Bidirectional relation asset table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);


    lua_settop(L, 0);
}
