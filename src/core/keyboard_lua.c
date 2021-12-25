#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "keyboard.h"
#include "lua_extra.h"

typedef struct luakeyboard{
    Enj_Keyboard *keyboard;

    lua_State *L;
    int candestroy;
}luakeyboard;

static void luaonpress(const char *key, void *data){
    luakeyboard *lb = (luakeyboard *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 1) == LUA_TFUNCTION){
        lua_pushstring(L, key);
        lua_call(L, 1+1, 0);
    }
    lua_settop(L, tmpidx);
}
static void luaonunpress(const char *key, void *data){
    luakeyboard *lb = (luakeyboard *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 2) == LUA_TFUNCTION){
        lua_pushstring(L, key);
        lua_call(L, 1+1, 0);
    }
    lua_settop(L, tmpidx);
}
static void luaoninput(const char *key, void *data){
    luakeyboard *lb = (luakeyboard *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 3) == LUA_TFUNCTION){
        lua_pushstring(L, key);
        lua_call(L, 1+1, 0);
    }
    lua_settop(L, tmpidx);
}


static int luacreatekeyboard(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    luakeyboard *sp = (luakeyboard *)lua_newuserdatauv(L, sizeof(luakeyboard), 3);
    lua_getfield(L, tmpidx+1, "keyboard");
    Enj_KeyboardList *sl =
        (Enj_KeyboardList *)lua_touserdata(L, lua_upvalueindex(1));

    lua_State *oL = lua_tothread(L, lua_upvalueindex(2));

    lua_setmetatable(L, tmpidx+3);
    lua_pushlightuserdata(L, sp);
    lua_pushvalue(L, tmpidx+3);
    lua_settable(L, 2);

    sp->keyboard = Enj_CreateKeyboard(sl);

    sp->keyboard->onpress = luaonpress;
    sp->keyboard->onunpress = luaonunpress;
    sp->keyboard->oninput = luaoninput;
    sp->keyboard->textmode = 0;
    sp->keyboard->active = 1;

    sp->keyboard->data = sp;
    sp->L = oL;
    sp->candestroy = 1;

    return 1;
}
static int luadestroykeyboard(lua_State *L){

    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_getfield(L, tmpidx+1, "keyboard");
    lua_getmetatable(L, 1);

    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }
    if(!sp->candestroy){
        lua_pushliteral(L, "keyboard not allowed to be destroyed");
        return Enj_Lua_Error(L);
    }


    Enj_KeyboardList *sl =
        (Enj_KeyboardList *)lua_touserdata(L, lua_upvalueindex(1));

    Enj_DestroyKeyboard(sl, sp->keyboard);
    sp->keyboard = NULL;

    //Erase strong reference pinning the struct from C side
    lua_pushlightuserdata(L, sp);
    lua_pushnil(L);
    lua_settable(L, 2);

    return 0;
}

static int luagetkeyboardactive(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushboolean(L, sp->keyboard->active);
    return 1;
}
static int luasetkeyboardactive(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!lua_isboolean(L, 2)){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->keyboard->active = lua_toboolean(L, 2);
    return 0;
}
static int luagetkeyboardonpress(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 1);
    return 1;
}
static int luasetkeyboardonpress(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!(lua_isfunction(L, 2) | lua_isnil(L, 2))){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 1);
    return 0;
}
static int luagetkeyboardonunpress(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 2);
    return 1;
}
static int luasetkeyboardonunpress(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!(lua_isfunction(L, 2) | lua_isnil(L, 2))) {
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 2);
    return 0;
}
static int luagetkeyboardoninput(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 3);
    return 1;
}
static int luasetkeyboardoninput(lua_State *L){
    luakeyboard *sp =
        (luakeyboard *)lua_touserdata(L, 1);

    if(!sp->keyboard){
        lua_pushliteral(L, "keyboard already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!(lua_isfunction(L, 2) | lua_isnil(L, 2))) {
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }
    //If nil, turn off text mode else on for function
    sp->keyboard->textmode = lua_isfunction(L, 2);
    //Set function in uservalue reference
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 3);


    return 0;
}


void bindkeyboard(lua_State *L, Enj_KeyboardList *sl){
    lua_pushlightuserdata(L, sl);
    lua_pushthread(L);
    lua_pushcclosure(L, luacreatekeyboard, 2);
    lua_setglobal(L, "create_keyboard");

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    //meta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 1);
    lua_pushlightuserdata(L, sl);
    lua_pushcclosure(L, luadestroykeyboard, 1);

    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "destroy");

    //close metafunction
    lua_setfield(L, 2, "__close");

    //gets
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luagetkeyboardactive);
    lua_setfield(L, 4, "active");
    lua_pushcfunction(L, luagetkeyboardonpress);
    lua_setfield(L, 4, "on_press");
    lua_pushcfunction(L, luagetkeyboardonunpress);
    lua_setfield(L, 4, "on_unpress");
    lua_pushcfunction(L, luagetkeyboardoninput);
    lua_setfield(L, 4, "on_input");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 2, "__index");

    //sets
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luasetkeyboardactive);
    lua_setfield(L, 3, "active");
    lua_pushcfunction(L, luasetkeyboardonpress);
    lua_setfield(L, 3, "on_press");
    lua_pushcfunction(L, luasetkeyboardonunpress);
    lua_setfield(L, 3, "on_unpress");
    lua_pushcfunction(L, luasetkeyboardoninput);
    lua_setfield(L, 3, "on_input");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 2, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "keyboard");
    lua_setfield(L, 2, "__metatable");

    lua_setfield(L, 1, "keyboard");
    lua_settop(L, 0);
}
