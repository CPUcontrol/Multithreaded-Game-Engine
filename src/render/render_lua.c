#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "render_lua.h"

void bindrender(lua_State *L){
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_createtable(L, 0, 8);
    lua_setfield(L, 1, "render");

    lua_settop(L, 0);
}
