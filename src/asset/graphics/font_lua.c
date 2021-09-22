#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "font_lua.h"

#include "../../core/lua_extra.h"
#include "../lua_extra_asset.h"
#include "../luaasset.h"
#include "luafont.h"

static int luagetfontcharxoffset(lua_State *L){
    luafontchar *lfc = (luafontchar *)lua_touserdata(L, 1);

    lua_pushinteger(L, lfc->xoffset);
    return 1;
}

static int luagetfontcharyoffset(lua_State *L){
    luafontchar *lfc = (luafontchar *)lua_touserdata(L, 1);

    lua_pushinteger(L, lfc->yoffset);
    return 1;
}

static int luagetfontcharadvance(lua_State *L){
    luafontchar *lfc = (luafontchar *)lua_touserdata(L, 1);

    lua_pushinteger(L, lfc->advance);
    return 1;
}

static int luagetfontcharglyph(lua_State *L){
    luafontchar *lfc = (luafontchar *)lua_touserdata(L, 1);

    lua_getiuservalue(L, 1, 1);
    return 1;
}

static int luagetfontchars(lua_State *L){
    lua_getiuservalue(L, 1, 3);
    return 1;
}

static int luagetfontheight(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    lua_pushinteger(L, ((luafont *)la->data)->height);
    return 1;
}

static void bindfontchar(lua_State *L){
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 0);

    //gets
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luagetfontcharxoffset);
    lua_setfield(L, 4, "offset_x");
    lua_pushcfunction(L, luagetfontcharyoffset);
    lua_setfield(L, 4, "offset_y");
    lua_pushcfunction(L, luagetfontcharadvance);
    lua_setfield(L, 4, "advance");
    lua_pushcfunction(L, luagetfontcharglyph);
    lua_setfield(L, 4, "glyph");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 2, "__index");

    //sets
    lua_createtable(L, 0, 0);
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 2, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "fontchar");
    lua_pushvalue(L, 3);
    lua_setfield(L, 2, "__metatable");

    lua_pushvalue(L, 2);
    lua_settable(L, 1);


    lua_settop(L, 0);
}

void bindfont(lua_State *L, void *ctx,
    int (*onpreload)(lua_State *),
    int (*onunload)(lua_State *),
    int (*oncanunload)(lua_State *)
)
{
    bindfontchar(L);


    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "asset");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 0);

    //gets
    lua_createtable(L, 0, 2);
    lua_pushcfunction(L, luagetfontchars);
    lua_setfield(L, 5, "chars");
    lua_pushcfunction(L, luagetfontheight);
    lua_setfield(L, 5, "height");
    lua_pushcclosure(L, Enj_Lua_GetAssetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 0);
    lua_pushcclosure(L, Enj_Lua_SetAssetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "font");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bind 3 create functions + context + metatable + num values into closure
    lua_pushcfunction(L, onpreload);
    lua_pushcfunction(L, onunload);
    lua_pushcfunction(L, oncanunload);
    lua_pushlightuserdata(L, ctx);
    lua_pushvalue(L, 3);
    lua_pushinteger(L, 3);
    lua_pushcclosure(L, Enj_Lua_CreateAsset, 6);
    lua_setglobal(L, "create_font");

    //Bidirectional relation asset table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);


    lua_settop(L, 0);
}
