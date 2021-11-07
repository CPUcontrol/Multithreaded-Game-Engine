#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "primrect_sdl.h"
#include "primrect_lua_sdl.h"

#include "../../core/allocator.h"
#include "../../core/lua_extra.h"

#include "../../asset/luaasset.h"

#include "renderlist_sdl.h"

#include "luarendernode_sdl.h"
#include "../lua_extra_render.h"

static int luadestroyprimrect(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");

    lua_getmetatable(L, 1);
    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);

    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_SDL *parent = lrn->parent;

    (*rn->onfreedata)(rn->data, rn->ctx, rn->allocdata);
    Enj_RenderListRemove_SDL(parent, rn);

    lrn->rn = NULL;

    //Remove from parent lua rendernode's table
    lua_getiuservalue(L, 1, 1);
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    lua_settable(L, tmpidx+4);

    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);

    return 0;
}

static int luagetprimrectx(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->x);
    return 1;
}
static int luasetprimrectx(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->x = (int)v;
    return 0;
}
static int luagetprimrecty(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->y);
    return 1;
}
static int luasetprimrecty(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->y = (int)v;
    return 0;
}
static int luagetprimrectwidth(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->w);
    return 1;
}
static int luasetprimrectwidth(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->w = (int)v;
    return 0;
}
static int luagetprimrectheight(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->h);
    return 1;
}
static int luasetprimrectheight(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->h = (int)v;
    return 0;
}

static int luagetprimrectr(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->r);
    return 1;
}
static int luasetprimrectr(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->r = (unsigned char)v;
    return 0;
}

static int luagetprimrectg(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->g);
    return 1;
}
static int luasetprimrectg(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->g = (unsigned char)v;
    return 0;
}

static int luagetprimrectb(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->b);
    return 1;
}
static int luasetprimrectb(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->b = (unsigned char)v;
    return 0;
}

static int luagetprimrecta(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->a);
    return 1;
}
static int luasetprimrecta(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->a = (unsigned char)v;
    return 0;
}

static int luagetprimrectcenterx(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->xcen);
    return 1;
}
static int luasetprimrectcenterx(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->xcen = (int)v;
    return 0;
}
static int luagetprimrectcentery(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    lua_pushinteger(L, pr->ycen);
    return 1;
}
static int luasetprimrectcentery(lua_State *L){
    luarendernode_SDL *lrn =
        (luarendernode_SDL *)lua_touserdata(L, 1);
    Enj_RenderNode_SDL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect_SDL *pr = (Enj_PrimRect_SDL *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->ycen = (int)v;
    return 0;
}

static void bindprimrectline(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocprimrect
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");
    //meta
    lua_createtable(L, 0, 4);

    //close metafunction
    lua_pushcfunction(L, luadestroyprimrect);
    lua_setfield(L, 3, "__close");

    //methods
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, luadestroyprimrect);
    lua_setfield(L, 4, "destroy");

    //gets
    lua_createtable(L, 0, 12);
    lua_pushcfunction(L, luagetprimrectx);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetprimrecty);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, luagetprimrectwidth);
    lua_setfield(L, 5, "width");
    lua_pushcfunction(L, luagetprimrectheight);
    lua_setfield(L, 5, "height");
    lua_pushcfunction(L, luagetprimrectr);
    lua_setfield(L, 5, "r");
    lua_pushcfunction(L, luagetprimrectg);
    lua_setfield(L, 5, "g");
    lua_pushcfunction(L, luagetprimrectb);
    lua_setfield(L, 5, "b");
    lua_pushcfunction(L, luagetprimrecta);
    lua_setfield(L, 5, "a");
    lua_pushcfunction(L, luagetprimrectcenterx);
    lua_setfield(L, 5, "pivot_x");
    lua_pushcfunction(L, luagetprimrectcentery);
    lua_setfield(L, 5, "pivot_y");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodePriority);
    lua_setfield(L, 5, "priority");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodeActive);
    lua_setfield(L, 5, "active");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 12);
    lua_pushcfunction(L, luasetprimrectx);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetprimrecty);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luasetprimrectwidth);
    lua_setfield(L, 4, "width");
    lua_pushcfunction(L, luasetprimrectheight);
    lua_setfield(L, 4, "height");
    lua_pushcfunction(L, luasetprimrectr);
    lua_setfield(L, 4, "r");
    lua_pushcfunction(L, luasetprimrectg);
    lua_setfield(L, 4, "g");
    lua_pushcfunction(L, luasetprimrectb);
    lua_setfield(L, 4, "b");
    lua_pushcfunction(L, luasetprimrecta);
    lua_setfield(L, 4, "a");
    lua_pushcfunction(L, luasetprimrectcenterx);
    lua_setfield(L, 4, "pivot_x");
    lua_pushcfunction(L, luasetprimrectcentery);
    lua_setfield(L, 4, "pivot_y");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodePriority);
    lua_setfield(L, 4, "priority");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodeActive);
    lua_setfield(L, 4, "active");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "rectline");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bidirectional relation render table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);

    lua_settop(L, 0);
}

static void bindprimrectfill(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocprimrect
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");
    //meta
    lua_createtable(L, 0, 4);

    //close metafunction
    lua_pushcfunction(L, luadestroyprimrect);
    lua_setfield(L, 3, "__close");

    //methods
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, luadestroyprimrect);
    lua_setfield(L, 4, "destroy");

    //gets
    lua_createtable(L, 0, 12);
    lua_pushcfunction(L, luagetprimrectx);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetprimrecty);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, luagetprimrectwidth);
    lua_setfield(L, 5, "width");
    lua_pushcfunction(L, luagetprimrectheight);
    lua_setfield(L, 5, "height");
    lua_pushcfunction(L, luagetprimrectr);
    lua_setfield(L, 5, "r");
    lua_pushcfunction(L, luagetprimrectg);
    lua_setfield(L, 5, "g");
    lua_pushcfunction(L, luagetprimrectb);
    lua_setfield(L, 5, "b");
    lua_pushcfunction(L, luagetprimrecta);
    lua_setfield(L, 5, "a");
    lua_pushcfunction(L, luagetprimrectcenterx);
    lua_setfield(L, 5, "pivot_x");
    lua_pushcfunction(L, luagetprimrectcentery);
    lua_setfield(L, 5, "pivot_y");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodePriority);
    lua_setfield(L, 5, "priority");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodeActive);
    lua_setfield(L, 5, "active");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 12);
    lua_pushcfunction(L, luasetprimrectx);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetprimrecty);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luasetprimrectwidth);
    lua_setfield(L, 4, "width");
    lua_pushcfunction(L, luasetprimrectheight);
    lua_setfield(L, 4, "height");
    lua_pushcfunction(L, luasetprimrectr);
    lua_setfield(L, 4, "r");
    lua_pushcfunction(L, luasetprimrectg);
    lua_setfield(L, 4, "g");
    lua_pushcfunction(L, luasetprimrectb);
    lua_setfield(L, 4, "b");
    lua_pushcfunction(L, luasetprimrecta);
    lua_setfield(L, 4, "a");
    lua_pushcfunction(L, luasetprimrectcenterx);
    lua_setfield(L, 4, "pivot_x");
    lua_pushcfunction(L, luasetprimrectcentery);
    lua_setfield(L, 4, "pivot_y");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodePriority);
    lua_setfield(L, 4, "priority");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodeActive);
    lua_setfield(L, 4, "active");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "rectfill");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bidirectional relation render table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);

    lua_settop(L, 0);
}

void bindprimrect_SDL(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocprimrect
)
{
    bindprimrectline(L, rend, allocprimrect);
    bindprimrectfill(L, rend, allocprimrect);
}
