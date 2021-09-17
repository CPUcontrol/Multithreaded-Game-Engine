#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int Enj_Lua_Error(lua_State *L){
    lua_Debug db;
    int level = 0;

    while(lua_getstack(L, level++, &db)){
        lua_getinfo(L, "Sl", &db);
        if(db.currentline != -1) break;
    }

    if (db.currentline != -1) lua_pushfstring(L, "%s:%d: %s",
        db.short_src,
        db.currentline,
        lua_tostring(L, lua_gettop(L))
        );

    return lua_error(L);
}

int Enj_Lua_GetDispatch(lua_State *L){
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

int Enj_Lua_SetDispatch(lua_State *L){
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
