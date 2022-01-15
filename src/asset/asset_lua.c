#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../core/lua_extra.h"

#include "asset_codes.h"
#include "asset_lua.h"
#include "luaasset.h"


static int lualoadasset(lua_State *L);
static int lualoadasset_cont(lua_State *L, int status, lua_KContext ctx);
static int luaunloadasset(lua_State *L);
static int luacanunloadasset(lua_State *L);


void bindasset(lua_State *L){
    //assetweaktable stores weak references to asset userdata,
    //guaranteed to be available when needed by C side
    lua_createtable(L, 0, 8);
    lua_createtable(L, 0, 1);
    lua_pushliteral(L, "v");
    lua_setfield(L, 2, "__mode");
    lua_setmetatable(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, "assetweaktable");


    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_createtable(L, 0, 8);
    lua_setfield(L, 1, "asset");

    lua_register(L, "load_asset", lualoadasset);
    lua_register(L, "unload_asset", luaunloadasset);



    lua_settop(L, 0);
}

//---Lua functions below---


static int lualoadasset(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");

    //Asset table contains bidirectional map between name and metatable
    lua_getmetatable(L, 1);

    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    if(la->flag & (1<<1)) {
        //Error in loading
        lua_pushfstring(L, "cannot load asset - %s", getasseterrormsg(la->errcode));
        return Enj_Lua_Error(L);
    }
    if(!la->data){
        lua_pushliteral(L, "asset already destroyed");
        return Enj_Lua_Error(L);
    }

    if(la->flag & (1<<0 | 1<<2)) {
        //Set ready flag, now that load_asset successfully finished
        la->flag |= 1 << 0;
        return 0;
    }
    else{
        lua_pushcfunction(L, &Enj_Lua_WaitForSignal);
        lua_pushlightuserdata(L, la->data);
        lua_callk(L, 1, 0, LUA_OK, &lualoadasset_cont);
        return lualoadasset_cont(L, LUA_OK, 0);
    }

}

static int lualoadasset_cont(lua_State *L, int status, lua_KContext ctx){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    if(la->flag & (1<<1)) {
        //Error in loading
        lua_pushfstring(L, "cannot load asset - %s", getasseterrormsg(la->errcode));
        return Enj_Lua_Error(L);
    }
    if(!la->data){
        lua_pushliteral(L, "asset already destroyed");
        return Enj_Lua_Error(L);
    }

    //Set ready flag, now that load_asset successfully finished
    la->flag |= 1 << 0;

    return 0;
}

static int luaunloadasset(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");

    //Asset table contains bidirectional map between metatable and name
    lua_getmetatable(L, 1);
    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    //If data is null or flag ready bit not set, silently abort
    if(!la->data | !(la->flag & (1<<0))) {
        return 0;
    }
    //Check if can unload first

    lua_pushcfunction(L, &luacanunloadasset);
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);

    if(!lua_toboolean(L, tmpidx+4)){
        lua_pushliteral(L, "cannot unload asset");
        return Enj_Lua_Error(L);
    }

    lua_pushcfunction(L, la->onunload);
    lua_pushvalue(L, 1);
    lua_call(L, 1, 0);


    //Unset registry
    lua_getfield(L, LUA_REGISTRYINDEX, "assetweaktable");
    lua_pushlightuserdata(L, la->data);
    lua_pushnil(L);
    lua_settable(L, tmpidx+5);

    la->data = NULL;
    la->flag &= ~(1<<0);
    return 0;
}

static int luacanunloadasset(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");

    //Asset table contains bidirectional map between metatable and name
    lua_getmetatable(L, 1);
    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    if(la->refcount > 0){
        lua_pushboolean(L, 0);
        return 1;
    }

    if(!(la->flag & (1<<0))) {
        //If not ready, unload will do nothing, so technically yes
        lua_pushboolean(L, 1);
        return 1;
    }
    //Call the asset-specific check
    lua_pushcfunction(L, la->oncanunload);
    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);

    return 1;
}
