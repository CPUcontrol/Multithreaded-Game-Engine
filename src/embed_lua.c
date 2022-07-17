#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "embed_lua.h"

static int lualoadpref(lua_State *L){
    lua_settop(L, 1);
    if(!lua_isstring(L, 1)){
        return 0;
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 1);
    lua_concat(L, 2);

    FILE *f = fopen(lua_tostring(L, 2), "rb");
    if(!f){
        lua_pushliteral(L, "");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t sz = (size_t)ftell(f);
    rewind(f);

    char *buf = (char *)malloc(sz);
    fread(buf, 1, sz, f);
    lua_pushlstring(L, buf, sz);

    free(buf);
    fclose(f);

    return 1;
}

static int luasavepref(lua_State *L){
    lua_settop(L, 2);
    if(!lua_isstring(L, 1) || !lua_isstring(L, 2)){
        return 0;
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
    lua_pushvalue(L, 3);
    lua_pushliteral(L, "~");
    lua_concat(L, 2);

    const char *dstfile = lua_tostring(L, 3);
    const char *tmpfile = lua_tostring(L, 4);

    FILE *f = fopen(tmpfile, "wb");
    if(!f){
        return 0;
    }
    size_t sz;
    const char *data = lua_tolstring(L, 2, &sz);

    fwrite(data, 1, sz, f);
    fclose(f);

    //Replace old destination file with temp file
    remove(dstfile);
    rename(tmpfile, dstfile);

    return 0;
}

static int luasetlogfunction(lua_State *L){
    lua_settop(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, "logfunction");
    return 0;
}

static int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx);
static int luadofilebasepath(lua_State *L){
    lua_settop(L, 1);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 3, 2);
        lua_pop(L, 1);
    }

    lua_callk(L, 1, LUA_MULTRET, 0, luadofilebasepath_cont);
    return luadofilebasepath_cont(L, LUA_OK, 0);
}
static int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx){
    return lua_gettop(L);
}

static int lualoadfilebasepath(lua_State *L){
    lua_settop(L, 3);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 5, 2);
        lua_pop(L, 1);
    }

    lua_call(L, 3, LUA_MULTRET);
    return lua_gettop(L);
}

void embed_lua(lua_State *L, const char *basepath, const char *prefpath){
    luaL_requiref(L, "basic", luaopen_base, 0);
    luaL_requiref(L, "math", luaopen_math, 0);
    luaL_requiref(L, "string", luaopen_string, 0);
    luaL_requiref(L, "table", luaopen_table, 0);
    luaL_requiref(L, "utf8", luaopen_utf8, 0);
    lua_settop(L, 0);

    luaL_requiref(L, "package", luaopen_package, 0);

    lua_pushstring(L, basepath);
    //Make dofile, loadfile work with base path
    lua_getglobal(L, "dofile");
    lua_pushvalue(L, 2);
    lua_pushcclosure(L, luadofilebasepath, 2);
    lua_setglobal(L, "dofile");

    lua_getglobal(L, "loadfile");
    lua_pushvalue(L, 2);
    lua_pushcclosure(L, lualoadfilebasepath, 2);
    lua_setglobal(L, "loadfile");

    //Set module search path
    lua_pushliteral(L, "modules/?.lua;");
    lua_pushvalue(L, 2);
    lua_pushliteral(L, "modules/?.lc");
    lua_concat(L, 4);

    lua_setfield(L, 1, "path");

    //Delete c library loader + all-in-one loader
    lua_getfield(L, 1, "searchers");
    lua_pushnil(L);
    lua_seti(L, 2, 4);
    lua_pushnil(L);
    lua_seti(L, 2, 3);

    //Delete loadlib function
    lua_pushnil(L);
    lua_setfield(L, 1, "loadlib");
    lua_settop(L, 0);

    //Pref file functions with pref path
    lua_pushstring(L, prefpath);
    lua_pushvalue(L, 1);

    lua_pushcclosure(L, lualoadpref, 1);
    lua_setglobal(L, "load_pref");
    lua_pushcclosure(L, luasavepref, 1);
    lua_setglobal(L, "save_pref");

    //Modify print function to use custom logging
    lua_getglobal(L, "print");
    lua_setfield(L, LUA_REGISTRYINDEX, "logfunction");
    lua_register(L, "set_logger", luasetlogfunction);
}
