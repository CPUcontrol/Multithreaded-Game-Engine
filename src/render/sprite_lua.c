#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sprite.h"
#include "sprite_lua.h"

#include "../core/allocator.h"
#include "../core/lua_extra.h"

#include "../core/glyph.h"

#include "../asset/luaasset.h"
#include "../asset/glyph_lua.h"

#include "renderlist.h"

#include "luarendernode.h"
#include "lua_extra_render.h"

static int luadestroysprite(lua_State *L){
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

    lua_getiuservalue(L, 1, 2);
    luaasset *la_glyph = (luaasset *)lua_touserdata(L, tmpidx+4);
    --la_glyph->refcount;
    lua_pushnil(L);
    lua_setiuservalue(L, 1, 2);

    Enj_RenderList *parent = lrn->parent;

    (*rn->onfreedata)(rn->data, rn->ctx, rn->allocdata);
    Enj_RenderListRemove(parent, rn);

    lrn->rn = NULL;

    //Remove from parent luarendernode's table
    lua_getiuservalue(L, 1, 1);
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    lua_settable(L, tmpidx+5);

    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);

    return 0;
}

static int luagetspritex(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->x);
    return 1;
}
static int luasetspritex(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->x = (int)v;
    return 0;
}
static int luagetspritey(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->y);
    return 1;
}
static int luasetspritey(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->y = (int)v;
    return 0;
}
static int luagetspritewidth(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->w);
    return 1;
}
static int luasetspritewidth(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->w = (int)v;
    return 0;
}
static int luagetspriteheight(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->h);
    return 1;
}
static int luasetspriteheight(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->h = (int)v;
    return 0;
}

static int luagetspriteglyph(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    lua_getiuservalue(L, 1, 2);
    return 1;
}

static int luasetspriteglyph(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 3, "asset");
    lua_getfield(L, 4, "glyph");
    lua_getmetatable(L, 2);
    if(!lua_compare(L, 5, 6, LUA_OPEQ)){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }


    luaasset *la_glyph = (luaasset *)lua_touserdata(L, 2);

    sp->glyph = (Enj_Glyph *)la_glyph->data;

    lua_getiuservalue(L, 1, 2);
    luaasset *la_glyph_old = (luaasset *)lua_touserdata(L, 7);

    ++la_glyph->refcount;
    --la_glyph_old->refcount;

    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 2);

    return 0;
}

static int luagetspriter(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->fill[0]);
    return 1;
}
static int luasetspriter(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->fill[0] = (unsigned char)v;
    return 0;
}

static int luagetspriteg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->fill[1]);
    return 1;
}
static int luasetspriteg(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->fill[1] = (unsigned char)v;
    return 0;
}

static int luagetspriteb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->fill[2]);
    return 1;
}
static int luasetspriteb(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->fill[2] = (unsigned char)v;
    return 0;
}

static int luagetspritea(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->fill[3]);
    return 1;
}
static int luasetspritea(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->fill[3] = (unsigned char)v;
    return 0;
}


static int luagetspritecenterx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->xcen);
    return 1;
}
static int luasetspritecenterx(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->xcen = (int)v;
    return 0;
}
static int luagetspritecentery(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushinteger(L, sp->ycen);
    return 1;
}
static int luasetspritecentery(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    sp->ycen = (int)v;
    return 0;
}

static int luagetspriteangle(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    lua_pushnumber(L, 3.1415926535898 / 32768. * (double)sp->angle);
    return 1;
}
static int luasetspriteangle(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_Sprite *sp = (Enj_Sprite *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    //Round to nearest int in [0, 65536)
    double res = 32768. / 3.1415926535898 * v;
    res = floor(res + 0.5);
    res = fmod(res, 65536.);
    //Convert to integral type - now in interval (-65536, 65536)
    long resround = (long)res;
    //Add 65536 if res less than zero
    resround += resround < 0 ? 65536 : 0;


    sp->angle = (unsigned short)resround;
    return 0;
}




void bindsprite(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocsprite
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, luadestroysprite);
    lua_setfield(L, 4, "destroy");

    //gets
    lua_createtable(L, 0, 14);
    lua_pushcfunction(L, luagetspritex);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetspritey);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, luagetspritewidth);
    lua_setfield(L, 5, "width");
    lua_pushcfunction(L, luagetspriteheight);
    lua_setfield(L, 5, "height");
    lua_pushcfunction(L, luagetspriter);
    lua_setfield(L, 5, "r");
    lua_pushcfunction(L, luagetspriteg);
    lua_setfield(L, 5, "g");
    lua_pushcfunction(L, luagetspriteb);
    lua_setfield(L, 5, "b");
    lua_pushcfunction(L, luagetspritea);
    lua_setfield(L, 5, "a");
    lua_pushcfunction(L, luagetspritecenterx);
    lua_setfield(L, 5, "pivot_x");
    lua_pushcfunction(L, luagetspritecentery);
    lua_setfield(L, 5, "pivot_y");
    lua_pushcfunction(L, luagetspriteangle);
    lua_setfield(L, 5, "angle");
    lua_pushcfunction(L, luagetspriteglyph);
    lua_setfield(L, 5, "glyph");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodePriority);
    lua_setfield(L, 5, "priority");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodeActive);
    lua_setfield(L, 5, "active");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 14);
    lua_pushcfunction(L, luasetspritex);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetspritey);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luasetspritewidth);
    lua_setfield(L, 4, "width");
    lua_pushcfunction(L, luasetspriteheight);
    lua_setfield(L, 4, "height");
    lua_pushcfunction(L, luasetspriter);
    lua_setfield(L, 4, "r");
    lua_pushcfunction(L, luasetspriteg);
    lua_setfield(L, 4, "g");
    lua_pushcfunction(L, luasetspriteb);
    lua_setfield(L, 4, "b");
    lua_pushcfunction(L, luasetspritea);
    lua_setfield(L, 4, "a");
    lua_pushcfunction(L, luasetspritecenterx);
    lua_setfield(L, 4, "x_pivot");
    lua_pushcfunction(L, luasetspritecentery);
    lua_setfield(L, 4, "y_pivot");
    lua_pushcfunction(L, luasetspriteangle);
    lua_setfield(L, 4, "angle");
    lua_pushcfunction(L, luasetspriteglyph);
    lua_setfield(L, 4, "glyph");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodePriority);
    lua_setfield(L, 4, "priority");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodeActive);
    lua_setfield(L, 4, "active");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "sprite");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bidirectional relation render table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);

    lua_settop(L, 0);
}
