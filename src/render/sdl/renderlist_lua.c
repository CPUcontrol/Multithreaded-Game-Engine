#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sprite.h"
#include "primrect.h"
#include "renderlist.h"
#include "renderlist_lua.h"

#include "../../core/allocator.h"
#include "../../core/lua_extra.h"

#include "../../core/graphics/sdl/glyph_sdl.h"

#include "../../asset/luaasset.h"
#include "../../asset/graphics/glyph_lua.h"

#include "luarendernode.h"
#include "../lua_extra_render.h"

static int luacreatesprite_fromrenderlist(lua_State *L){
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");
    lua_getfield(L, tmpidx+1, "asset");

    lua_getmetatable(L, 1);
    lua_getfield(L, tmpidx+2, "renderlist");
    if(!lua_compare(L, tmpidx+4, tmpidx+5, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    lua_getfield(L, tmpidx+3, "glyph");
    lua_getmetatable(L, 2);
    if(!lua_compare(L, tmpidx+6, tmpidx+7, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }
    luaasset *la_glyph = (luaasset *)lua_touserdata(L, 2);
    if(!(la_glyph->flag & 1<<0)){
        lua_pushliteral(L, "glyph not loaded");
        return Enj_Lua_Error(L);
    }
    luarendernode *lrn_parent = (luarendernode *)lua_touserdata(L, 1);

    Enj_RenderList *parent =
        (Enj_RenderList *)lrn_parent->rn->data;
    Enj_Allocator *allocsprite =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode *lrn = (luarendernode *)
        lua_newuserdatauv(L, sizeof(luarendernode), 2);

    Enj_Sprite *sp = (Enj_Sprite *)
        Enj_Alloc(allocsprite, sizeof(Enj_Sprite));
    if(!sp) {
        lua_pushliteral(L, "max sprites exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode *newrn = Enj_RenderListAppend(parent);
    if(!newrn) {
        Enj_Free(allocsprite, sp);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = sp;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocsprite;
    newrn->onfreedata = Enj_Sprite_OnFree;
    newrn->onrender = Enj_Sprite_OnRender;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;


    lua_getfield(L, tmpidx+2, "sprite");
    lua_setmetatable(L, tmpidx+8);

    sp->glyph = (Enj_Glyph_SDL *)la_glyph->data;
    ++la_glyph->refcount;
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, tmpidx+8, 2);

    sp->fill[0]=255;
    sp->fill[1]=255;
    sp->fill[2]=255;
    sp->fill[3]=255;

    sp->x = 0;
    sp->y = 0;
    sp->w = sp->glyph->rect.w;
    sp->h = sp->glyph->rect.h;
    sp->xcen = 0;
    sp->ycen = 0;
    sp->angle = 0;

    //Update parent lua renderlist's table of children
    lua_getiuservalue(L, 1, 2);
    lua_pushvalue(L, tmpidx+8);
    lua_getmetatable(L, tmpidx+8);
    lua_settable(L, tmpidx+9);
    //Put parent table of children into new render node
    lua_setiuservalue(L, tmpidx+8, 1);

    return 1;
}

static int luacreateprimrectline_fromrenderlist(lua_State *L){
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");

    lua_getmetatable(L, 1);
    lua_getfield(L, tmpidx+2, "renderlist");
    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luarendernode *lrn_parent = (luarendernode *)lua_touserdata(L, 1);

    Enj_RenderList *parent =
        (Enj_RenderList *)lrn_parent->rn->data;
    Enj_Allocator *allocprimrect =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode *lrn = (luarendernode *)
        lua_newuserdatauv(L, sizeof(luarendernode), 2);

    Enj_PrimRect *pr = (Enj_PrimRect *)
        Enj_Alloc(allocprimrect, sizeof(Enj_PrimRect));
    if(!pr) {
        lua_pushliteral(L, "max rectangles exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode *newrn = Enj_RenderListAppend(parent);
    if(!newrn) {
        Enj_Free(allocprimrect, pr);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = pr;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocprimrect;
    newrn->onfreedata = Enj_PrimRect_OnFree;
    newrn->onrender = Enj_PrimRectLine_OnRender;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;


    lua_getfield(L, tmpidx+2, "rectline");
    lua_setmetatable(L, tmpidx+5);

    pr->r=255;
    pr->g=255;
    pr->b=255;
    pr->a=255;

    pr->x = 0;
    pr->y = 0;
    pr->w = 0;
    pr->h = 0;
    pr->xcen = 0;
    pr->ycen = 0;

    //Update parent lua renderlist's table of children
    lua_getiuservalue(L, 1, 2);
    lua_pushvalue(L, tmpidx+5);
    lua_getmetatable(L, tmpidx+5);
    lua_settable(L, tmpidx+6);
    //Put parent table of children into new render node
    lua_setiuservalue(L, tmpidx+5, 1);

    return 1;
}

static int luacreateprimrectfill_fromrenderlist(lua_State *L){
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");

    lua_getmetatable(L, 1);
    lua_getfield(L, tmpidx+2, "renderlist");
    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luarendernode *lrn_parent = (luarendernode *)lua_touserdata(L, 1);

    Enj_RenderList *parent =
        (Enj_RenderList *)lrn_parent->rn->data;
    Enj_Allocator *allocprimrect =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode *lrn = (luarendernode *)
        lua_newuserdatauv(L, sizeof(luarendernode), 2);

    Enj_PrimRect *pr = (Enj_PrimRect *)
        Enj_Alloc(allocprimrect, sizeof(Enj_PrimRect));
    if(!pr) {
        lua_pushliteral(L, "max rectangles exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode *newrn = Enj_RenderListAppend(parent);
    if(!newrn) {
        Enj_Free(allocprimrect, pr);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = pr;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocprimrect;
    newrn->onfreedata = Enj_PrimRect_OnFree;
    newrn->onrender = Enj_PrimRectFill_OnRender;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;


    lua_getfield(L, tmpidx+2, "rectfill");
    lua_setmetatable(L, tmpidx+5);

    pr->r=255;
    pr->g=255;
    pr->b=255;
    pr->a=255;

    pr->x = 0;
    pr->y = 0;
    pr->w = 0;
    pr->h = 0;
    pr->xcen = 0;
    pr->ycen = 0;

    //Update parent lua renderlist's table of children
    lua_getiuservalue(L, 1, 2);
    lua_pushvalue(L, tmpidx+5);
    lua_getmetatable(L, tmpidx+5);
    lua_settable(L, tmpidx+6);
    //Put parent table of children into new render node
    lua_setiuservalue(L, tmpidx+5, 1);

    return 1;
}

static int luacreaterenderlist_fromrenderlist(lua_State *L){
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "render");

    lua_getmetatable(L, 1);
    lua_getfield(L, tmpidx+2, "renderlist");
    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    luarendernode *lrn_parent = (luarendernode *)lua_touserdata(L, 1);

    Enj_RenderList *parent =
        (Enj_RenderList *)lrn_parent->rn->data;
    Enj_Allocator *allocrl =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    Enj_Allocator *allocrn =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(3));

    luarendernode *lrn = (luarendernode *)
        lua_newuserdatauv(L, sizeof(luarendernode), 2);

    Enj_RenderList *rl = (Enj_RenderList *)
        Enj_Alloc(allocrl, sizeof(Enj_RenderList));
    if(!rl) {
        lua_pushliteral(L, "max renderlists exceeded");
        return Enj_Lua_Error(L);
    }
    Enj_RenderListInit(rl, allocrn);

    Enj_RenderNode *newrn = Enj_RenderListAppend(parent);
    if(!newrn) {
        Enj_Free(allocrl, rl);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = rl;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocrl;
    newrn->onfreedata = Enj_RenderList_OnFree;
    newrn->onrender = Enj_RenderList_OnRender;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;

    lua_getfield(L, tmpidx+2, "renderlist");
    lua_setmetatable(L, tmpidx+5);

    rl->xoffset = 0;
    rl->yoffset = 0;

    //Create empty table for renderlist to store children
    lua_createtable(L, 0, 2);
    lua_setiuservalue(L, tmpidx+5, 2);

    //Update parent lua renderlist's table of children
    lua_getiuservalue(L, 1, 2);
    lua_pushvalue(L, tmpidx+5);
    lua_getmetatable(L, tmpidx+5);
    lua_settable(L, tmpidx+6);
    //Put parent table of children into new render node
    lua_setiuservalue(L, tmpidx+5, 1);

    return 1;
}

static int luagetrenderlistxoffset(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList *rl = (Enj_RenderList *)rn->data;

    lua_pushinteger(L, rl->xoffset);
    return 1;
}
static int luasetrenderlistxoffset(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList *rl = (Enj_RenderList *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->xoffset = (int)v;
    return 0;
}

static int luagetrenderlistyoffset(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList *rl = (Enj_RenderList *)rn->data;

    lua_pushinteger(L, rl->yoffset);
    return 1;
}
static int luasetrenderlistyoffset(lua_State *L){
    luarendernode *lrn =
        (luarendernode *)lua_touserdata(L, 1);
    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList *rl = (Enj_RenderList *)rn->data;

    int isint;
    lua_Integer v = lua_tointegerx(L, 2, &isint);
    if(!isint){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->yoffset = (int)v;
    return 0;
}


Enj_RenderList * bindroot_renderlist(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");

    luarendernode *lrn = (luarendernode *)
        lua_newuserdatauv(L, sizeof(luarendernode), 2);

    Enj_RenderList *rl = (Enj_RenderList *)
        Enj_Alloc(allocrenderlist, sizeof(Enj_RenderList));
    if(!rl) {
        return NULL;
    }
    Enj_RenderListInit(rl, allocrendernode);

    Enj_RenderNode *newrn = (Enj_RenderNode *)
        Enj_Alloc(allocrendernode, sizeof(Enj_RenderNode));
    if(!newrn) {
        Enj_Free(allocrenderlist, rl);
        return NULL;
    }
    newrn->data = rl;
    newrn->ctx = rend;
    newrn->allocdata = allocrenderlist;
    newrn->onfreedata = Enj_RenderList_OnFree;
    newrn->onrender = Enj_RenderList_OnRender;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = NULL;
    lrn->rn = newrn;

    lua_getfield(L, 2, "renderlist");
    lua_setmetatable(L, 3);

    rl->xoffset = 0;
    rl->yoffset = 0;

    //Create empty table for renderlist to store children
    lua_createtable(L, 0, 2);
    lua_setiuservalue(L, 3, 2);

    lua_setglobal(L, "root_render");
    lua_settop(L, 0);
    return rl;
}

static int luadestroyrenderlist(lua_State *L){
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

    if(!lrn->parent){
        lua_pushliteral(L, "cannot destroy root node");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    //Destroy all children lua render nodes
    lua_getiuservalue(L, 1, 2);
    lua_pushnil(L);
    while (lua_next(L, tmpidx+4)){
        lua_getfield(L, tmpidx+5, "destroy");
        lua_pushvalue(L, tmpidx+5);
        lua_call(L, 1, 0);
        lua_pop(L, 1);
    }
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

void bindrenderlist(
    lua_State *L,
    SDL_Renderer *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode,
    Enj_Allocator *allocsprite,
    Enj_Allocator *allocprimrect
    )
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");
    //spritemeta
    lua_createtable(L, 0, 4);

    //methods
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luadestroyrenderlist);
    lua_setfield(L, 4, "destroy");
    lua_pushlightuserdata(L, rend);
    lua_pushlightuserdata(L, allocrenderlist);
    lua_pushlightuserdata(L, allocrendernode);
    lua_pushcclosure(L, luacreaterenderlist_fromrenderlist, 3);
    lua_setfield(L, 4, "create_renderlist");
    lua_pushlightuserdata(L, rend);
    lua_pushlightuserdata(L, allocsprite);
    lua_pushcclosure(L, luacreatesprite_fromrenderlist, 2);
    lua_setfield(L, 4, "create_sprite");
    lua_pushlightuserdata(L, rend);
    lua_pushlightuserdata(L, allocprimrect);
    lua_pushcclosure(L, luacreateprimrectline_fromrenderlist, 2);
    lua_setfield(L, 4, "create_rectline");
    lua_pushlightuserdata(L, rend);
    lua_pushlightuserdata(L, allocprimrect);
    lua_pushcclosure(L, luacreateprimrectfill_fromrenderlist, 2);
    lua_setfield(L, 4, "create_rectfill");

    //gets
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luagetrenderlistxoffset);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetrenderlistyoffset);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodePriority);
    lua_setfield(L, 5, "priority");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodeActive);
    lua_setfield(L, 5, "active");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 4);
    lua_pushcfunction(L, luasetrenderlistxoffset);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetrenderlistyoffset);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodePriority);
    lua_setfield(L, 4, "priority");
    lua_pushcfunction(L, Enj_Lua_SetRenderNodeActive);
    lua_setfield(L, 4, "active");
    lua_pushcclosure(L, Enj_Lua_SetDispatch, 1);

    lua_setfield(L, 3, "__newindex");

    //rest of the meta
    lua_pushliteral(L, "renderlist");
    lua_pushvalue(L, 4);
    lua_setfield(L, 3, "__metatable");

    //Bidirectional relation render table
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 3);
    lua_settable(L, 2);
    lua_settable(L, 2);

    lua_settop(L, 0);
}
