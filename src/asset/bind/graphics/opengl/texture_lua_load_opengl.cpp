#include <condition_variable>
#include <mutex>
#include <string>

#include <stdlib.h>

#include <GL/gl3w.h>
#include <lua.hpp>

#include "../../../../core/allocator.h"
#include "../../../../core/act_lua.h"
#include "../../../../core/instream.h"
#include "../../../../core/pngload.h"
#include "../../../../core/graphics/opengl/texture_opengl.h"

#include "../../../util/multi_dispatch.hpp"

#include "../../../asset_codes.h"
#include "../../../luaasset.h"

#include "texture_binder_opengl.hpp"
#include "texture_lua_load_opengl.h"

int Enj_Lua_TextureOnPreload_OpenGL(lua_State *L){
    if (!lua_isstring(L, 2)) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    texture_binder_OpenGL *ctx = (texture_binder_OpenGL *)la->ctx;
    Enj_Texture_OpenGL *e = (Enj_Texture_OpenGL *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Texture_OpenGL));
    if (!e) {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_POOL);
        return 2;
    }


    {
        std::lock_guard<std::mutex> lock(ctx->dispatch.wq.mtx);

        ctx->dispatch.wq.q.push([la, e, path = ctx->basepath + lua_tostring(L, 2), ctx](){
            Enj_PNGLoader png;
            Enj_Instream ifile;

            if(Enj_InitInstreamFromFile(&ifile, path.c_str())){

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }
            if(Enj_OpenPNG(&png, &ifile)){
                Enj_FreeInstream(&ifile);

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
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

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
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

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
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


            std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
            ctx->dispatch.mq.q.push([la, e, imgbuf, ctx,
                width, height]() {

                glGenTextures(1, &e->id);
                glBindTexture(GL_TEXTURE_2D, e->id);
                glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
                glTexImage2D(
                    GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, imgbuf
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                e->width = width;
                e->height = height;

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
int Enj_Lua_TextureOnUnload_OpenGL(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Texture_OpenGL *e = (Enj_Texture_OpenGL *)la->data;
    texture_binder_OpenGL *ctx = (texture_binder_OpenGL *)la->ctx;

    glDeleteTextures(1, &e->id);
    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_TextureOnCanUnload_OpenGL(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
