#include <condition_variable>
#include <filesystem>
#include <mutex>

#include <stdlib.h>

#include <SDL_render.h>
#include <lua.hpp>

#include "../../../../core/allocator.h"
#include "../../../../core/act_lua.h"
#include "../../../../core/instream.h"
#include "../../../../core/pngload.h"
#include "../../../../core/graphics/sdl/texture_sdl.h"

#include "../../../util/multi_dispatch.hpp"

#include "../../../asset_codes.h"
#include "../../../luaasset.h"

#include "texture_binder_sdl.hpp"
#include "../../texture_lua_load.h"

int Enj_Lua_TextureOnPreload(lua_State *L){
    if(!lua_isstring(L, 2)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    texture_binder_SDL *ctx = (texture_binder_SDL *)la->ctx;
    Enj_Texture_SDL *e = (Enj_Texture_SDL *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Texture_SDL));
    if(!e) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_POOL);
        return 2;
    }


    {
        std::lock_guard lock(ctx->dispatch.wq.mtx);

        ctx->dispatch.wq.q.push([la, e, path = ctx->basepath.generic_string() + lua_tostring(L, 2), ctx](){
            Enj_PNGLoader png;
            Enj_Instream ifile;

            if(Enj_InitInstreamFromFile(&ifile, path.c_str())){

                std::lock_guard lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }
            if(Enj_OpenPNG(&png, &ifile)){
                Enj_FreeInstream(&ifile);

                std::lock_guard lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }
            //Max dimension size is 2^16-1
            if((png.width > (1<<16)-1) | (png.height > (1<<16)-1)){
                Enj_ClosePNG(&png);
                Enj_FreeInstream(&ifile);

                std::lock_guard lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_PARAM);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }

            size_t imgsz = png.width*png.height*4;
            void *imgbuf = malloc(imgsz);
            if(!imgbuf){
                Enj_ClosePNG(&png);
                Enj_FreeInstream(&ifile);

                std::lock_guard lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }

            Enj_ReadPNG(&png, imgbuf, imgsz, 0);
            unsigned short width = png.width;
            unsigned short height = png.height;
            Enj_ClosePNG(&png);
            Enj_FreeInstream(&ifile);


            std::lock_guard lock(ctx->dispatch.mq.mtx);
            ctx->dispatch.mq.q.push([la, e, imgbuf, ctx,
                width, height]() {

                e->tx = SDL_CreateTexture(ctx->rend,
                    SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                    width, height);
                e->width = width;
                e->height = height;

                SDL_UpdateTexture(e->tx, NULL, imgbuf, width*4);
                SDL_SetTextureBlendMode(e->tx, SDL_BLENDMODE_BLEND);


                free(imgbuf);


                luafinishpreloadasset(ctx->Lmain, la, ASSET_OK);
            });

        });
        ctx->dispatch.cv.notify_one();
    }

    la->data = e;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_TextureOnUnload(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Texture_SDL *e = (Enj_Texture_SDL *)la->data;
    texture_binder_SDL *ctx = (texture_binder_SDL *)la->ctx;

    SDL_DestroyTexture(e->tx);
    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_TextureOnCanUnload(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
