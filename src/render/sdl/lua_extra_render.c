#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "renderlist.h"

#include "luarendernode.h"

#include "../../core/lua_extra.h"
#include "../lua_extra_render.h"

int Enj_Lua_GetRenderNodePriority(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushinteger(L, rn->priority);
    return 1;
}
int Enj_Lua_SetRenderNodePriority(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rn->priority = (int)v;
    return 0;
}

int Enj_Lua_GetRenderNodeActive(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_pushboolean(L, rn->active);
    return 1;
}
int Enj_Lua_SetRenderNodeActive(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    if(!lua_isboolean(L, 2)){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rn->active = lua_toboolean(L, 2);
    return 0;
}
