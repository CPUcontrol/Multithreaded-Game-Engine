#include <condition_variable>
#include <mutex>

#include <stdlib.h>

#include <GL/glcorearb.h>
#include <lua.hpp>

#include "../../../../core/allocator.h"
#include "../../../../core/act_lua.h"
#include "../../../../core/graphics/opengl/glyph_opengl.h"
#include "../../../../core/graphics/opengl/texture_opengl.h"

#include "../../../util/multi_dispatch.hpp"

#include "../../../asset_codes.h"
#include "../../../luaasset.h"

#include "glyph_binder_opengl.hpp"
#include "glyph_lua_load_opengl.h"

int Enj_Lua_GlyphOnPreload_OpenGL(lua_State *L){
    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_getfield(L, tmpidx+1, "asset");
    lua_getfield(L, tmpidx+2, "texture");
    lua_getmetatable(L, 2);

    if(!lua_compare(L, tmpidx+3, tmpidx+4, LUA_OPEQ)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    luaasset *la_texture = (luaasset *)lua_touserdata(L, 2);
    if(!(la_texture->flag & 1<<0)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }
    Enj_Texture_OpenGL *e_texture = (Enj_Texture_OpenGL *)la_texture->data;

    lua_Integer x;
    lua_Integer y;
    lua_Integer w;
    lua_Integer h;

    unsigned char frbits = 0;

    switch(tmpidx){
    case 2:
        x = 0;
        y = 0;
        w = e_texture->width;
        h = e_texture->height;
        break;
    case 9:
        if (!(
            lua_isboolean(L, 7) &&
            lua_isboolean(L, 8) &&
            lua_isboolean(L, 9)
        ))
        {
            lua_pushnil(L);
            lua_pushinteger(L, ASSET_ERROR_BADARGS);
            return 2;
        }

        frbits |= lua_toboolean(L, 7);
        frbits |= lua_toboolean(L, 8) << 1;
        frbits |= lua_toboolean(L, 9) << 2;

        //Fall through to get other args
    case 6:
        int isint;
        x = lua_tointegerx(L, 3, &isint);
        if(!isint) {
            lua_pushnil(L);
            lua_pushinteger(L, ASSET_ERROR_BADARGS);
            return 2;
        }
        y = lua_tointegerx(L, 4, &isint);
        if(!isint) {
            lua_pushnil(L);
            lua_pushinteger(L, ASSET_ERROR_BADARGS);
            return 2;
        }
        w = lua_tointegerx(L, 5, &isint);
        if(!isint) {
            lua_pushnil(L);
            lua_pushinteger(L, ASSET_ERROR_BADARGS);
            return 2;
        }
        h = lua_tointegerx(L, 6, &isint);
        if(!isint) {
            lua_pushnil(L);
            lua_pushinteger(L, ASSET_ERROR_BADARGS);
            return 2;
        }
        break;
    default:
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    if(
        (x < 0)
    |   (y < 0)
    |   (w <= 0)
    |   (h <= 0)
    |   (x >= e_texture->width)
    |   (y >= e_texture->height)
    |   (x+w > e_texture->width)
    |   (y+h > e_texture->height))
    {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    //Fillout the created asset
    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    glyph_binder_OpenGL *ctx = (glyph_binder_OpenGL *)la->ctx;
    Enj_Glyph_OpenGL *e = (Enj_Glyph_OpenGL *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Glyph_OpenGL));
    if(!e) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_POOL);
        return 2;
    }

    //Flip width and height if rotated
    e->width = (unsigned short)(frbits & 1<<2 ? h : w);
    e->height = (unsigned short)(frbits & 1<<2 ? w : h);

    e->texture = e_texture;
    e->u_ul = (GLfloat)x / e_texture->width;
    e->v_ul = (GLfloat)y / e_texture->height;
    e->u_dr = (GLfloat)(x+w) / e_texture->width;
    e->v_dr = (GLfloat)(y+h) / e_texture->height;

    //TODO: add glyph option for fliprotate setting
    unsigned char fr = 0<<0 | 1<<2 | 2<<4 | 3<<6;
    //flip horizontal
    fr = frbits & 1<<0 ?
        fr ^ (1<<0 | 1<<2 | 1<<4 | 1<<6) :
        fr;
    //flip vertical
    fr = frbits & 1<<1 ?
        fr >> 4 | fr << 4 :
        fr;
    //rotate 90 deg clockwise
    fr = frbits & 1<<2 ?
        (fr & 3<<0)<<2 | (fr & 3<<2)<<4 | (fr & 3<<6)>>2 | (fr & 3<<4)>>4 :
        fr;

    e->fliprotate = fr;

    ++la_texture->refcount;
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 1);

    {
        std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
        ctx->dispatch.mq.q.push([la, Lmain = ctx->Lmain](){
            luafinishpreloadasset(Lmain, la, ASSET_OK);
        });
    }

    la->data = e;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_GlyphOnUnload_OpenGL(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Glyph_OpenGL *e = (Enj_Glyph_OpenGL *)la->data;
    glyph_binder_OpenGL *ctx = (glyph_binder_OpenGL *)la->ctx;

    lua_getiuservalue(L, 1, 1);

    luaasset *la_texture = (luaasset *)lua_touserdata(L, 2);
    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);
    --la_texture->refcount;

    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_GlyphOnCanUnload_OpenGL(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
