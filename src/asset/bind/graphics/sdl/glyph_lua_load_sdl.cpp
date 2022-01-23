#include <condition_variable>
#include <mutex>

#include <stdlib.h>

#include <lua.hpp>

#include "../../../../core/allocator.h"
#include "../../../../core/act_lua.h"
#include "../../../../core/graphics/sdl/glyph_sdl.h"
#include "../../../../core/graphics/sdl/texture_sdl.h"

#include "../../../util/multi_dispatch.hpp"

#include "../../../asset_codes.h"
#include "../../../luaasset.h"

#include "glyph_binder_sdl.hpp"
#include "../../glyph_lua_load.h"

int Enj_Lua_GlyphOnPreload(lua_State *L){
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
    Enj_Texture_SDL *e_texture = (Enj_Texture_SDL *)la_texture->data;

    lua_Integer x;
    lua_Integer y;
    lua_Integer w;
    lua_Integer h;
    switch(tmpidx){
    case 2:
        x = 0;
        y = 0;
        w = e_texture->width;
        h = e_texture->height;
        break;
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

    glyph_binder_SDL *ctx = (glyph_binder_SDL *)la->ctx;
    Enj_Glyph_SDL *e = (Enj_Glyph_SDL *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Glyph_SDL));
    if(!e) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_POOL);
        return 2;
    }

    e->rect.x = (int)x;
    e->rect.y = (int)y;
    e->rect.w = (int)w;
    e->rect.h = (int)h;
    e->texture = e_texture;
    ++la_texture->refcount;
    lua_pushvalue(L, 2);
    lua_setiuservalue(L, 1, 1);

    {
        std::lock_guard lock(ctx->dispatch.mq.mtx);
        ctx->dispatch.mq.q.push([la, Lmain = ctx->Lmain](){
            luafinishpreloadasset(Lmain, la, ASSET_OK);
        });
    }

    la->data = e;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_GlyphOnUnload(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Glyph_SDL *e = (Enj_Glyph_SDL *)la->data;
    glyph_binder_SDL *ctx = (glyph_binder_SDL *)la->ctx;

    lua_getiuservalue(L, 1, 1);

    luaasset *la_texture = (luaasset *)lua_touserdata(L, 2);
    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);
    --la_texture->refcount;

    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_GlyphOnCanUnload(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
