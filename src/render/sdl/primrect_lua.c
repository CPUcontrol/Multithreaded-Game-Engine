#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "primrect.h"
#include "primrect_lua.h"

#include "../../core/allocator.h"
#include "../../core/lua_extra.h"

#include "../../asset/luaasset.h"

#include "renderlist.h"

#include "luarendernode.h"
#include "lua_extra_render.h"

static int luadestroyprimrect(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");

    lua_getmetatable(L, 1);
    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);

    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList *parent = lrn->parent;

    (*rn->onfreedata)(rn->data, rn->ctx, rn->allocdata);
    Enj_RenderListRemove(parent, rn);

    lrn->rn = NULL;

    //Remove from parent luarendernode's table
    lua_getiuservalue(L, 1, 1);
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    lua_settable(L, tmpidx+4);

    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);

    return 0;
}

static int luagetprimrectx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->x);
    return 1;
}
static int luasetprimrectx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

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
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->y);
    return 1;
}
static int luasetprimrecty(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

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
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->w);
    return 1;
}
static int luasetprimrectwidth(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

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
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->h);
    return 1;
}
static int luasetprimrectheight(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->h = (int)v;
    return 0;
}

static int luagetprimrectstroker(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->stroke[0]);
    return 1;
}
static int luasetprimrectstroker(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->stroke[0] = (unsigned char)v;
    return 0;
}

static int luagetprimrectstrokeg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->stroke[1]);
    return 1;
}
static int luasetprimrectstrokeg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->stroke[1] = (unsigned char)v;
    return 0;
}

static int luagetprimrectstrokeb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->stroke[2]);
    return 1;
}
static int luasetprimrectstrokeb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->stroke[2] = (unsigned char)v;
    return 0;
}

static int luagetprimrectstrokea(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->stroke[3]);
    return 1;
}
static int luasetprimrectstrokea(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->stroke[3] = (unsigned char)v;
    return 0;
}


static int luagetprimrectfillr(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->fill[0]);
    return 1;
}
static int luasetprimrectfillr(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->fill[0] = (unsigned char)v;
    return 0;
}

static int luagetprimrectfillg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->fill[1]);
    return 1;
}
static int luasetprimrectfillg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->fill[1] = (unsigned char)v;
    return 0;
}

static int luagetprimrectfillb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->fill[2]);
    return 1;
}
static int luasetprimrectfillb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->fill[2] = (unsigned char)v;
    return 0;
}

static int luagetprimrectfilla(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->fill[3]);
    return 1;
}
static int luasetprimrectfilla(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->fill[3] = (unsigned char)v;
    return 0;
}


static int luagetprimrectcenterx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->xcen);
    return 1;
}
static int luasetprimrectcenterx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

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
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    lua_pushinteger(L, pr->ycen);
    return 1;
}
static int luasetprimrectcentery(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_PrimRect *pr = (Enj_PrimRect *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    pr->ycen = (int)v;
    return 0;
}



void bindprimrect(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocprimrect
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");
    //primrectmeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, luadestroyprimrect);
    lua_setfield(L, 4, "destroy");

    //gets
    lua_createtable(L, 0, 16);
    lua_pushcfunction(L, luagetprimrectx);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetprimrecty);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, luagetprimrectwidth);
    lua_setfield(L, 5, "width");
    lua_pushcfunction(L, luagetprimrectheight);
    lua_setfield(L, 5, "height");
    lua_pushcfunction(L, luagetprimrectfillr);
    lua_setfield(L, 5, "fill_r");
    lua_pushcfunction(L, luagetprimrectfillg);
    lua_setfield(L, 5, "fill_g");
    lua_pushcfunction(L, luagetprimrectfillb);
    lua_setfield(L, 5, "fill_b");
    lua_pushcfunction(L, luagetprimrectfilla);
    lua_setfield(L, 5, "fill_a");
    lua_pushcfunction(L, luagetprimrectstroker);
    lua_setfield(L, 5, "stroke_r");
    lua_pushcfunction(L, luagetprimrectstrokeg);
    lua_setfield(L, 5, "stroke_g");
    lua_pushcfunction(L, luagetprimrectstrokeb);
    lua_setfield(L, 5, "stroke_b");
    lua_pushcfunction(L, luagetprimrectstrokea);
    lua_setfield(L, 5, "stroke_a");
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
    lua_createtable(L, 0, 16);
    lua_pushcfunction(L, luasetprimrectx);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetprimrecty);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luasetprimrectwidth);
    lua_setfield(L, 4, "width");
    lua_pushcfunction(L, luasetprimrectheight);
    lua_setfield(L, 4, "height");
    lua_pushcfunction(L, luasetprimrectfillr);
    lua_setfield(L, 4, "fill_r");
    lua_pushcfunction(L, luasetprimrectfillg);
    lua_setfield(L, 4, "fill_g");
    lua_pushcfunction(L, luasetprimrectfillb);
    lua_setfield(L, 4, "fill_b");
    lua_pushcfunction(L, luasetprimrectfilla);
    lua_setfield(L, 4, "fill_a");
    lua_pushcfunction(L, luasetprimrectstroker);
    lua_setfield(L, 4, "stroke_r");
    lua_pushcfunction(L, luasetprimrectstrokeg);
    lua_setfield(L, 4, "stroke_g");
    lua_pushcfunction(L, luasetprimrectstrokeb);
    lua_setfield(L, 4, "stroke_b");
    lua_pushcfunction(L, luasetprimrectstrokea);
    lua_setfield(L, 4, "stroke_a");
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
    lua_pushliteral(L, "primrect");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bidirectional relation render table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);

    lua_settop(L, 0);
}
