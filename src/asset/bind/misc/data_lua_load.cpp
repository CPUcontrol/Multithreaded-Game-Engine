#include <condition_variable>
#include <mutex>
#include <string>

#include <stdlib.h>

#include <SDL_mixer.h>
#include <lua.hpp>

#include "../../../core/act_lua.h"
#include "../../../core/instream.h"

#include "../../util/multi_dispatch.hpp"

#include "../../asset_codes.h"
#include "../../luaasset.h"

#include "data_binder.hpp"
#include "data_lua_load.h"

int Enj_Lua_DataOnPreload(lua_State *L){
    if(!lua_isstring(L, 2))  {
        lua_pushnil(L);
        lua_pushinteger(L, ASSET_ERROR_BADARGS);
        return 2;
    }

    luaasset *la = (luaasset *)lua_touserdata(L, 1);

    data_binder *ctx = (data_binder *)la->ctx;

    {
        std::lock_guard<std::mutex> lock(ctx->dispatch.wq.mtx);

        ctx->dispatch.wq.q.push([la, path = ctx->basepath + lua_tostring(L, 2), ctx](){
            Enj_Instream ifile;

            if(Enj_InitInstreamFromFile(&ifile, path.c_str())){

                std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                ctx->dispatch.mq.q.push([la, ctx](){
                    luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_FILE);
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

                    std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                    ctx->dispatch.mq.q.push([la, ctx](){
                        luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_MEMORY);
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

                    std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
                    ctx->dispatch.mq.q.push([la, ctx](){
                        luafinishpreloadasset(ctx->Lmain, la, ASSET_ERROR_PARAM);
                    });

                    return;
                }

                newbuffer = realloc(buffer, sz<<=1);
            }

            Enj_FreeInstream(&ifile);


            std::lock_guard<std::mutex> lock(ctx->dispatch.mq.mtx);
            ctx->dispatch.mq.q.push([la, buffer, ctx, fsize]() {
                lua_getfield(ctx->Lmain, LUA_REGISTRYINDEX, "assetweaktable");
                lua_pushlightuserdata(ctx->Lmain, la->data);
                lua_gettable(ctx->Lmain, 1);

                //Put data in uservalue of asset
                lua_pushlstring(ctx->Lmain, (char *)buffer, fsize);
                lua_setiuservalue(ctx->Lmain, 2, 1);

                free(buffer);

                lua_settop(ctx->Lmain, 0);
                luafinishpreloadasset(ctx->Lmain, la, ASSET_OK);

            });

        });
        ctx->dispatch.cv.notify_one();
    }
    //Just need a unique id, the luaasset address itself will do nicely
    la->data = la;
    lua_pushvalue(L, 1);
    return 1;
}
int Enj_Lua_DataOnUnload(lua_State *L){\
    lua_pushnil(L);
    lua_setiuservalue(L, 1, 1);
    return 0;
}
int Enj_Lua_DataOnCanUnload(lua_State *L){
    lua_pushboolean(L, 1);
    return 1;
}
