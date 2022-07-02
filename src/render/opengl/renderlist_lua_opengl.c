#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "render_opengl.h"
#include "sprite_opengl.h"
#include "primrect_opengl.h"
#include "renderlist_opengl.h"
#include "renderlist_lua_opengl.h"

#include "../../core/allocator.h"
#include "../../core/lua_extra.h"

#include "../../core/graphics/opengl/glyph_opengl.h"

#include "../../asset/luaasset.h"
#include "../../asset/graphics/glyph_lua.h"

#include "luarendernode_opengl.h"
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
    luarendernode_OpenGL *lrn_parent = (luarendernode_OpenGL *)lua_touserdata(L, 1);

    Enj_RenderList_OpenGL *parent =
        (Enj_RenderList_OpenGL *)lrn_parent->rn->data;
    Enj_Allocator *allocsprite =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode_OpenGL *lrn = (luarendernode_OpenGL *)
        lua_newuserdatauv(L, sizeof(luarendernode_OpenGL), 2);

    Enj_Sprite_OpenGL *sp = (Enj_Sprite_OpenGL *)
        Enj_Alloc(allocsprite, sizeof(Enj_Sprite_OpenGL));
    if(!sp) {
        lua_pushliteral(L, "max sprites exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode_OpenGL *newrn = Enj_RenderListAppend_OpenGL(parent);
    if(!newrn) {
        Enj_Free(allocsprite, sp);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = sp;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocsprite;
    newrn->onfreedata = Enj_Sprite_OnFree_OpenGL;
    newrn->onrender = Enj_Sprite_OnRender_OpenGL;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;

    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)newrn->ctx;
    ++rend->numsprite;

    lua_getfield(L, tmpidx+2, "sprite");
    lua_setmetatable(L, tmpidx+8);

    sp->glyph = (Enj_Glyph_OpenGL *)la_glyph->data;
    ++la_glyph->refcount;
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, tmpidx+8, 2);

    sp->r=255;
    sp->g=255;
    sp->b=255;
    sp->a=255;

    sp->x = 0;
    sp->y = 0;
    sp->w = sp->glyph->width;
    sp->h = sp->glyph->height;
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

    luarendernode_OpenGL *lrn_parent = (luarendernode_OpenGL *)
        lua_touserdata(L, 1);

    Enj_RenderList_OpenGL *parent =
        (Enj_RenderList_OpenGL *)lrn_parent->rn->data;
    Enj_Allocator *allocprimrect =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode_OpenGL *lrn = (luarendernode_OpenGL *)
        lua_newuserdatauv(L, sizeof(luarendernode_OpenGL), 2);

    Enj_PrimRect_OpenGL *pr = (Enj_PrimRect_OpenGL *)
        Enj_Alloc(allocprimrect, sizeof(Enj_PrimRect_OpenGL));
    if(!pr) {
        lua_pushliteral(L, "max primrects exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode_OpenGL *newrn = Enj_RenderListAppend_OpenGL(parent);
    if(!newrn) {
        Enj_Free(allocprimrect, pr);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = pr;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocprimrect;
    newrn->onfreedata = Enj_PrimRect_OnFree_OpenGL;
    newrn->onrender = Enj_PrimRectLine_OnRender_OpenGL;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;

    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)newrn->ctx;
    ++rend->numrectline;

    lua_getfield(L, tmpidx+2, "rectline");
    lua_setmetatable(L, tmpidx+5);

    pr->r = 255;
    pr->g = 255;
    pr->b = 255;
    pr->a = 255;

    pr->x = 0;
    pr->y = 0;
    pr->w = 0;
    pr->h = 0;
    pr->xcen = 0;
    pr->ycen = 0;
    pr->angle = 0;

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

    luarendernode_OpenGL *lrn_parent = (luarendernode_OpenGL *)
        lua_touserdata(L, 1);

    Enj_RenderList_OpenGL *parent =
        (Enj_RenderList_OpenGL *)lrn_parent->rn->data;
    Enj_Allocator *allocprimrect =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    luarendernode_OpenGL *lrn = (luarendernode_OpenGL *)
        lua_newuserdatauv(L, sizeof(luarendernode_OpenGL), 2);

    Enj_PrimRect_OpenGL *pr = (Enj_PrimRect_OpenGL *)
        Enj_Alloc(allocprimrect, sizeof(Enj_PrimRect_OpenGL));
    if(!pr) {
        lua_pushliteral(L, "max primrects exceeded");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode_OpenGL *newrn = Enj_RenderListAppend_OpenGL(parent);
    if(!newrn) {
        Enj_Free(allocprimrect, pr);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = pr;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocprimrect;
    newrn->onfreedata = Enj_PrimRect_OnFree_OpenGL;
    newrn->onrender = Enj_PrimRectFill_OnRender_OpenGL;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;

    Enj_Renderer_OpenGL *rend = (Enj_Renderer_OpenGL *)newrn->ctx;
    ++rend->numrectfill;

    lua_getfield(L, tmpidx+2, "rectfill");
    lua_setmetatable(L, tmpidx+5);

    pr->r = 255;
    pr->g = 255;
    pr->b = 255;
    pr->a = 255;

    pr->x = 0;
    pr->y = 0;
    pr->w = 0;
    pr->h = 0;
    pr->xcen = 0;
    pr->ycen = 0;
    pr->angle = 0;

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

    luarendernode_OpenGL *lrn_parent = (luarendernode_OpenGL *)lua_touserdata(L, 1);

    Enj_RenderList_OpenGL *parent =
        (Enj_RenderList_OpenGL *)lrn_parent->rn->data;
    Enj_Allocator *allocrl =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(2));
    Enj_Allocator *allocrn =
        (Enj_Allocator *)lua_touserdata(L, lua_upvalueindex(3));

    luarendernode_OpenGL *lrn = (luarendernode_OpenGL *)
        lua_newuserdatauv(L, sizeof(luarendernode_OpenGL), 2);

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)
        Enj_Alloc(allocrl, sizeof(Enj_RenderList_OpenGL));
    if(!rl) {
        lua_pushliteral(L, "max renderlists exceeded");
        return Enj_Lua_Error(L);
    }
    Enj_RenderListInit_OpenGL(rl, allocrn);

    Enj_RenderNode_OpenGL *newrn = Enj_RenderListAppend_OpenGL(parent);
    if(!newrn) {
        Enj_Free(allocrl, rl);
        lua_pushliteral(L, "max render nodes exceeded");
        return Enj_Lua_Error(L);
    }
    newrn->data = rl;
    newrn->ctx = lua_touserdata(L, lua_upvalueindex(1));
    newrn->allocdata = allocrl;
    newrn->onfreedata = Enj_RenderList_OnFree_OpenGL;
    newrn->onrender = Enj_RenderList_OnRender_OpenGL;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = parent;
    lrn->rn = newrn;

    lua_getfield(L, tmpidx+2, "renderlist");
    lua_setmetatable(L, tmpidx+5);

    rl->scalex = 1;
    rl->scaley = 1;
    rl->angle = 0;
    rl->translatex = 0;
    rl->translatey = 0;

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

static int luagetrenderlistscalex(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    lua_pushnumber(L, rl->scalex);
    return 1;
}
static int luasetrenderlistscalex(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->scalex = (float)v;
    return 0;
}

static int luagetrenderlistscaley(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    lua_pushnumber(L, rl->scaley);
    return 1;
}
static int luasetrenderlistscaley(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->scaley = (float)v;
    return 0;
}

static int luagetrenderlisttranslatex(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    lua_pushnumber(L, rl->translatex);
    return 1;
}
static int luasetrenderlisttranslatex(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->translatex = (float)v;
    return 0;
}

static int luagetrenderlistangle(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    lua_pushnumber(L, rl->angle);
    return 1;
}
static int luasetrenderlistangle(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->angle = (float)v;
    return 0;
}

static int luagetrenderlisttranslatey(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    lua_pushnumber(L, rl->translatey);
    return 1;
}
static int luasetrenderlisttranslatey(lua_State *L){
    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);
    Enj_RenderNode_OpenGL *rn = lrn->rn;

    if(!rn){
        lua_pushliteral(L, "render node already destroyed");
        return Enj_Lua_Error(L);
    }

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)rn->data;

    int isnum;
    lua_Number v = lua_tonumberx(L, 2, &isnum);
    if(!isnum){
        lua_pushliteral(L, "assigned incompatible type");
        return Enj_Lua_Error(L);
    }

    rl->translatey = (float)v;
    return 0;
}


Enj_RenderList_OpenGL * bindroot_renderlist_OpenGL(
    lua_State *L,
    Enj_Renderer_OpenGL *rend,
    Enj_Allocator *allocrenderlist,
    Enj_Allocator *allocrendernode
)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, 1, "render");

    luarendernode_OpenGL *lrn = (luarendernode_OpenGL *)
        lua_newuserdatauv(L, sizeof(luarendernode_OpenGL), 2);

    Enj_RenderList_OpenGL *rl = (Enj_RenderList_OpenGL *)
        Enj_Alloc(allocrenderlist, sizeof(Enj_RenderList_OpenGL));
    if(!rl) {
        return NULL;
    }
    Enj_RenderListInit_OpenGL(rl, allocrendernode);

    Enj_RenderNode_OpenGL *newrn = (Enj_RenderNode_OpenGL *)
        Enj_Alloc(allocrendernode, sizeof(Enj_RenderNode_OpenGL));
    if(!newrn) {
        Enj_Free(allocrenderlist, rl);
        return NULL;
    }
    newrn->data = rl;
    newrn->ctx = rend;
    newrn->allocdata = allocrenderlist;
    newrn->onfreedata = Enj_RenderList_OnFree_OpenGL;
    newrn->onrender = Enj_RenderList_OnRender_OpenGL;
    newrn->priority = 0;
    newrn->active = 1;

    lrn->parent = NULL;
    lrn->rn = newrn;

    lua_getfield(L, 2, "renderlist");
    lua_setmetatable(L, 3);

    rl->scalex = 1;
    rl->scaley = 1;
    rl->angle = 0;
    rl->translatex = 0;
    rl->translatey = 0;

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

    luarendernode_OpenGL *lrn =
        (luarendernode_OpenGL *)lua_touserdata(L, 1);

    if(!lrn->parent){
        lua_pushliteral(L, "cannot destroy root node");
        return Enj_Lua_Error(L);
    }

    Enj_RenderNode_OpenGL *rn = lrn->rn;

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


    Enj_RenderList_OpenGL *parent = lrn->parent;

    (*rn->onfreedata)(rn->data, rn->ctx, rn->allocdata);
    Enj_RenderListRemove_OpenGL(parent, rn);

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

void bindrenderlist_OpenGL(
    lua_State *L,
    Enj_Renderer_OpenGL *rend,
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
    lua_createtable(L, 0, 7);
    lua_pushcfunction(L, luagetrenderlisttranslatex);
    lua_setfield(L, 5, "x");
    lua_pushcfunction(L, luagetrenderlisttranslatey);
    lua_setfield(L, 5, "y");
    lua_pushcfunction(L, luagetrenderlistscalex);
    lua_setfield(L, 5, "scale_x");
    lua_pushcfunction(L, luagetrenderlistscaley);
    lua_setfield(L, 5, "scale_y");
    lua_pushcfunction(L, luagetrenderlistangle);
    lua_setfield(L, 5, "angle");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodePriority);
    lua_setfield(L, 5, "priority");
    lua_pushcfunction(L, Enj_Lua_GetRenderNodeActive);
    lua_setfield(L, 5, "active");
    lua_pushcclosure(L, Enj_Lua_GetDispatch, 2);

    lua_setfield(L, 3, "__index");

    //sets
    lua_createtable(L, 0, 7);
    lua_pushcfunction(L, luasetrenderlisttranslatex);
    lua_setfield(L, 4, "x");
    lua_pushcfunction(L, luasetrenderlisttranslatey);
    lua_setfield(L, 4, "y");
    lua_pushcfunction(L, luasetrenderlistscalex);
    lua_setfield(L, 4, "scale_x");
    lua_pushcfunction(L, luasetrenderlistscaley);
    lua_setfield(L, 4, "scale_y");
    lua_pushcfunction(L, luasetrenderlistangle);
    lua_setfield(L, 4, "angle");
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
