#include <condition_variable>
#include <filesystem>
#include <mutex>

#include <stdlib.h>

#include <SDL_mixer.h>
#include <lua.hpp>

#include "../../../core/allocator.h"
#include "../../../core/act_lua.h"
#include "../../../core/instream.h"
#include "../../../core/sound.h"

#include "../../util/multi_dispatch.hpp"

#include "../../luaasset.h"

#include "sound_binder.hpp"
#include "../sound_lua_load.h"

int Enj_Lua_SoundOnPreload(lua_State *L){
    if(!lua_isstring(L, 2)) return 0;

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    sound_binder *ctx = (sound_binder *)la->ctx;
    Enj_Sound *e = (Enj_Sound *)Enj_Alloc(&ctx->alloc, sizeof(Enj_Sound));
    if(!e) return 0;


    {
        std::lock_guard lock(ctx->dispatch.wq.mtx);

        ctx->dispatch.wq.q.push([la, e, path = ctx->basepath.generic_string() + lua_tostring(L, 2), ctx](){
            Enj_Instream ifile;

            if(Enj_InitInstreamFromFile(&ifile, path.c_str())){

                std::lock_guard lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, e, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, 1);
                    Enj_Free(&ctx->alloc, e);
                });

                return;
            }

            unsigned long fsize = 0;
            unsigned long sz = 1<<10;
            void *buffer = NULL;
            void *newbuffer = malloc(sz);

            while(true){
                if(!newbuffer){
                    free(buffer);
                    Enj_FreeInstream(&ifile);

                    std::lock_guard lock(ctx->dispatch.mq.mtx);
                    ctx->dispatch.mq.q.push([la, e, ctx](){
                        luafinishpreloadasset(ctx->Lmain, la, 1);
                        Enj_Free(&ctx->alloc, e);
                    });

                    return;
                }
                buffer = newbuffer;

                size_t bytesread = Enj_ReadBytes(
                    &ifile, (char *)buffer + fsize, sz-fsize);

                fsize += (unsigned long)bytesread;
                if(fsize < sz){
                    break;
                }
                else if(sz == 1<<31){
                    free(buffer);
                    Enj_FreeInstream(&ifile);

                    std::lock_guard lock(ctx->dispatch.mq.mtx);
                    ctx->dispatch.mq.q.push([la, e, ctx](){
                        luafinishpreloadasset(ctx->Lmain, la, 1);
                        Enj_Free(&ctx->alloc, e);
                    });

                    return;
                }

                newbuffer = realloc(buffer, sz<<=1);
            }

            Enj_FreeInstream(&ifile);


            std::lock_guard lock(ctx->dispatch.mq.mtx);
            ctx->dispatch.mq.q.push([la, e, buffer, ctx, fsize]() {

                e->chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(buffer, fsize), 1);
                free(buffer);

                //if failure, chunk is null, so signal 1, otherwise success 0
                if(!e->chunk){
                    luafinishpreloadasset(ctx->Lmain, la, 1);
                    Enj_Free(&ctx->alloc, e);
                }
                else{
                    luafinishpreloadasset(ctx->Lmain, la, 0);
                }

            });

        });
        ctx->dispatch.cv.notify_one();
    }

    la->data = e;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_SoundOnUnload(lua_State *L){
    luaasset *la = (luaasset *)lua_touserdata(L, 1);
    Enj_Sound *e = (Enj_Sound *)la->data;
    sound_binder *ctx = (sound_binder *)la->ctx;

    Mix_FreeChunk(e->chunk);
    Enj_Free(&ctx->alloc, e);
    return 0;
}
int Enj_Lua_SoundOnCanUnload(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
