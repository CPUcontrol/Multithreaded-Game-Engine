#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "button.h"
#include "lua_extra.h"

typedef struct luabutton{
    Enj_Button *button;

    lua_State *L;
    int candestroy;
}luabutton;

static void luaonpress(int x, int y, void *data){
    luabutton *lb = (luabutton *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 1) == LUA_TFUNCTION){
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        lua_call(L, 1+2, 0);
    }
    lua_settop(L, tmpidx);
}
static void luaonunpress(int x, int y, void *data){
    luabutton *lb = (luabutton *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 2) == LUA_TFUNCTION){
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        lua_call(L, 1+2, 0);
    }
    lua_settop(L, tmpidx);
}
static void luaonhover(int x, int y, void *data){
    luabutton *lb = (luabutton *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 3) == LUA_TFUNCTION){
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        lua_call(L, 1+2, 0);
    }
    lua_settop(L, tmpidx);
}
static void luaonunhover(int x, int y, void *data){
    luabutton *lb = (luabutton *)data;
    lua_State *L = lb->L;
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_pushlightuserdata(L, lb);
    lua_gettable(L, tmpidx+1);

    lua_pushcfunction(L, &Enj_Lua_StartThread);
    if(lua_getiuservalue(L, tmpidx+2, 4) == LUA_TFUNCTION){
        lua_pushinteger(L, x);
        lua_pushinteger(L, y);
        lua_call(L, 1+2, 0);
    }
    lua_settop(L, tmpidx);
}

static int luacreatebutton(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    luabutton *sp = (luabutton *)lua_newuserdatauv(L, sizeof(luabutton), 4);
    lua_getfield(L, tmpidx+1, "button");
    Enj_ButtonList *sl =
        (Enj_ButtonList *)lua_touserdata(L, lua_upvalueindex(1));

    lua_State *oL = lua_tothread(L, lua_upvalueindex(2));

    lua_setmetatable(L, tmpidx+3);
    lua_pushlightuserdata(L, sp);
    lua_pushvalue(L, tmpidx+3);
    lua_settable(L, 2);

    sp->button = Enj_CreateButton(sl);

    sp->button->rect.x = 0;
    sp->button->rect.y = 0;
    sp->button->rect.w = 0;
    sp->button->rect.h = 0;

    sp->button->onpress = luaonpress;
    sp->button->onunpress = luaonunpress;
    sp->button->onhover = luaonhover;
    sp->button->onunhover = luaonunhover;
    sp->button->hovering = 0;
    sp->button->pressing = 0;

    sp->button->data = sp;
    sp->L = oL;
    sp->candestroy = 1;

    return 1;
}
static int luadestroybutton(lua_State *L){

    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, LUA_REGISTRYINDEX, "strongreftable");
    lua_getfield(L, tmpidx+1, "button");
    lua_getmetatable(L, 1);

    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }
    if(!sp->candestroy){
        lua_pushliteral(L, "button not allowed to be destroyed");
        return Enj_Lua_Error(L);
    }


    Enj_ButtonList *sl =
        (Enj_ButtonList *)lua_touserdata(L, lua_upvalueindex(1));

    Enj_DestroyButton(sl, sp->button);
    sp->button = NULL;

    //Erase strong reference pinning the struct from C side
    lua_pushlightuserdata(L, sp);
    lua_pushnil(L);
    lua_settable(L, 2);

    return 0;
}
static int luagetbuttonx(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushinteger(L, sp->button->rect.x);
    return 1;
}
static int luasetbuttonx(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->button->rect.x = (int)v;
    return 0;
}
static int luagetbuttony(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushinteger(L, sp->button->rect.y);
    return 1;
}
static int luasetbuttony(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->button->rect.y = (int)v;
    return 0;
}
static int luagetbuttonwidth(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushinteger(L, sp->button->rect.w);
    return 1;
}
static int luasetbuttonwidth(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->button->rect.w = (int)v;
    return 0;
}
static int luagetbuttonheight(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushinteger(L, sp->button->rect.h);
    return 1;
}
static int luasetbuttonheight(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->button->rect.h = (int)v;
    return 0;
}
static int luagetbuttononpress(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 1);
    return 1;
}
static int luasetbuttononpress(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
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
static int luagetbuttononunpress(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 2);
    return 1;
}
static int luasetbuttononunpress(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
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
static int luagetbuttononhover(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 3);
    return 1;
}
static int luasetbuttononhover(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!(lua_isfunction(L, 2) | lua_isnil(L, 2))) {
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 3);
    return 0;
}
static int luagetbuttononunhover(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 4);
    return 1;
}
static int luasetbuttononunhover(lua_State *L){
    luabutton *sp =
        (luabutton *)lua_touserdata(L, 1);

    if(!sp->button){
        lua_pushliteral(L, "button already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!(lua_isfunction(L, 2) | lua_isnil(L, 2))) {
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 4);
    return 0;
}


void bindbutton(lua_State *L, Enj_ButtonList *sl){
    lua_pushlightuserdata(L, sl);
    lua_pushthread(L);
    lua_pushcclosure(L, luacreatebutton, 2);
    lua_setglobal(L, "create_button");

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    //meta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 1);
    lua_pushlightuserdata(L, sl);
    lua_pushcclosure(L, luadestroybutton, 1);

    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "destroy");

    //close metafunction
    lua_setfield(L, 2, "__close");

    //gets
    lua_createtable(L, 0, 8);
    lua_pushcfunction(L, luagetbuttonx);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luagetbuttony);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luagetbuttonwidth);
    lua_setfield(L, 4, "width");
    lua_pushcfunction(L, luagetbuttonheight);
    lua_setfield(L, 4, "height");
    lua_pushcfunction(L, luagetbuttononpress);
    lua_setfield(L, 4, "on_press");
    lua_pushcfunction(L, luagetbuttononunpress);
    lua_setfield(L, 4, "on_unpress");
    lua_pushcfunction(L, luagetbuttononhover);
    lua_setfield(L, 4, "on_hover");
    lua_pushcfunction(L, luagetbuttononunhover);
    lua_setfield(L, 4, "on_unhover");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 2, "__index");

    //sets
    lua_createtable(L, 0, 8);
    lua_pushcfunction(L, luasetbuttonx);
    lua_setfield(L, 3, "x");
    lua_pushcfunction(L, luasetbuttony);
    lua_setfield(L, 3, "y");
    lua_pushcfunction(L, luasetbuttonwidth);
    lua_setfield(L, 3, "width");
    lua_pushcfunction(L, luasetbuttonheight);
    lua_setfield(L, 3, "height");
    lua_pushcfunction(L, luasetbuttononpress);
    lua_setfield(L, 3, "on_press");
    lua_pushcfunction(L, luasetbuttononunpress);
    lua_setfield(L, 3, "on_unpress");
    lua_pushcfunction(L, luasetbuttononhover);
    lua_setfield(L, 3, "on_hover");
    lua_pushcfunction(L, luasetbuttononunhover);
    lua_setfield(L, 3, "on_unhover");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 2, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "button");
    lua_setfield(L, 2, "__metatable");

    lua_setfield(L, 1, "button");
    lua_settop(L, 0);
}
