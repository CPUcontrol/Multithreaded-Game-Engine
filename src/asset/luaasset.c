#include "luaasset.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../core/lua_extra.h"
void luafinishpreloadasset(lua_State *L, luaasset *la, int code){
    la->errcode = code;

    if(code == 0){
        //Set can pass bit, so load_asset can set ready bit
        la->flag |= 1 << 2;
    }
    else{
        la->flag |= 1 << 1;

        //Unset registry
        lua_getfield(L, LUA_REGISTRYINDEX, "assetweaktable");
        lua_pushlightuserdata(L, la->data);
        lua_pushnil(L);
        lua_settable(L, 1);
    }

    //Wake waiters
    lua_pushcfunction(L, Enj_Lua_SignalThreads);
    lua_pushlightuserdata(L, la->data);
    lua_call(L, 1, 0);

    lua_settop(L, 0);
}
