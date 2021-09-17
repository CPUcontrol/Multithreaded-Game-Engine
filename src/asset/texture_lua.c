#include <stddef.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "texture_lua.h"

#include "../core/texture.h"

#include "lua_extra_asset.h"
#include "luaasset.h"


static int luagettexturewidth(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    lua_pushinteger(L, ((Enj_Texture *)la->data)->width);
    return 1;
}

static int luagettextureheight(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    lua_pushinteger(L, ((Enj_Texture *)la->data)->height);
    return 1;
}

void bindtexture(lua_State *L, void *ctx,
    int (*onpreload)(lua_State *),
    int (*onunload)(lua_State *),
    int (*oncanunload)(lua_State *)
    ){


    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "asset");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 0);

    //gets
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, luagettexturewidth);
    lua_setfield(L, 5, "width");
    lua_pushcfunction(L, luagettextureheight);
    lua_setfield(L, 5, "height");
    lua_pushcclosure(L, Enj_Lua_GetAssetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 0);
    lua_pushcclosure(L, Enj_Lua_SetAssetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "texture");
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
    lua_setglobal(L, "create_texture");

    //Bidirectional relation asset table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);


    lua_settop(L, 0);
}
