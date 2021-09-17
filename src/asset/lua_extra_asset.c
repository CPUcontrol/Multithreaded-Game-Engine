#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../core/lua_extra.h"
#include "lua_extra_asset.h"

#include "luaasset.h"

int Enj_Lua_GetAssetDispatch(lua_State *L){
    luaasset *la = lua_touserdata(L, 1);
    if(!(la->flag & (1<<0))){
        lua_pushliteral(L, "asset not loaded");
        return Enj_Lua_Error(L);
    }
    //Check for method
    lua_pushvalue(L, 2);
    if(lua_gettable(L, lua_upvalueindex(1)) != LUA_TNIL)
        return 1;
    //Check for field
    lua_pushvalue(L, 2);
    if(lua_gettable(L, lua_upvalueindex(2)) == LUA_TNIL){
        lua_pushfstring(L,
            "cannot read from field '%s'", lua_tostring(L, 2));
        return Enj_Lua_Error(L);
    }

    lua_pushvalue(L, 1);
    lua_call(L, 1, 1);
    return 1;
}

int Enj_Lua_SetAssetDispatch(lua_State *L){
    luaasset *la = lua_touserdata(L, 1);
    if(!(la->flag & (1<<0))){
        lua_pushliteral(L, "asset not loaded");
        return Enj_Lua_Error(L);
    }

    lua_pushvalue(L, 2);
    if(lua_gettable(L, lua_upvalueindex(1)) == LUA_TNIL) {
        lua_pushfstring(L,
            "cannot write to field '%s'", lua_tostring(L, 2));
        return Enj_Lua_Error(L);
    }

    lua_pushvalue(L, 1);
    lua_pushvalue(L, 3);
    lua_call(L, 2, 0);
    return 0;
}


int Enj_Lua_CreateAsset(lua_State *L){
    lua_pushvalue(L, lua_upvalueindex(1));
    int tmp = lua_gettop(L);

    luaasset *la = (luaasset *)
        lua_newuserdatauv(L,
            sizeof(luaasset),
            (int)lua_tointeger(L, lua_upvalueindex(6))
        );
    la->ctx = lua_touserdata(L, lua_upvalueindex(4));
    la->data = NULL;
    la->refcount = 0;
    la->flag = 0;

    la->onunload = lua_tocfunction(L, lua_upvalueindex(2));
    la->oncanunload = lua_tocfunction(L, lua_upvalueindex(3));

    lua_rotate(L, 1, 2);
    lua_call(L, tmp, 1);

    if(!lua_isuserdata(L, 1)){
        lua_pushliteral(L, "cannot create asset");
        return Enj_Lua_Error(L);
    }



    //Set registry so that asset C code can access the full userdata
    //with asset pointer
    lua_getfield(L, LUA_REGISTRYINDEX, "assetweaktable");
    lua_pushlightuserdata(L, la->data);
    lua_pushvalue(L, 1);
    lua_settable(L, 2);
    //set metatable of lua asset
    lua_pushvalue(L, lua_upvalueindex(5));
    lua_setmetatable(L, 1);


    lua_pushvalue(L, 1);
    return 1;
}
