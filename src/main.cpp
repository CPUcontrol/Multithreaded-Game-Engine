#include <stdio.h>
#include <string.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <thread>
#include <queue>
#include <tuple>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <lua.hpp>

#include <SDL.h>
#include <SDL_mixer.h>


#include "core/allocator.h"
#include "core/act_lua.h"

#include "core/instream.h"
#include "core/pngload.h"

#include "core/button.h"
#include "core/button_lua.h"
#include "core/keyboard.h"
#include "core/keyboard_lua.h"
#include "core/lua_extra.h"

#include "core/graphics/texture.h"
#include "core/graphics/glyph.h"

#include "asset/luaasset.h"
#include "asset/asset_lua.h"
#include "asset/lua_extra_asset.h"
#include "asset/graphics/texture_lua.h"
#include "asset/graphics/glyph_lua.h"
#include "asset/audio/sound_lua.h"
#include "asset/audio/music_lua.h"
#include "asset/graphics/font_lua.h"
#include "asset/misc/data_lua.h"
#include "asset/bind/texture_lua_load.h"
#include "asset/bind/glyph_lua_load.h"
#include "asset/bind/font_lua_load.h"
#include "asset/bind/sound_lua_load.h"
#include "asset/bind/music_lua_load.h"
#include "asset/bind/data_lua_load.h"

#include "asset/bind/audio/sound_binder.hpp"
#include "asset/bind/audio/music_binder.hpp"
#include "asset/bind/misc/data_binder.hpp"
#include "asset/util/multi_dispatch.hpp"

#include "render/render.h"
#include "render/render_lua.h"

#include "appconfig.h"

static int luagetmousecoords(lua_State *L){
    int x;
    int y;

    SDL_GetMouseState(&x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

static int luagetmousepressed(lua_State *L){
    lua_pushboolean(
        L,
        (SDL_GetMouseState(NULL, NULL) | SDL_BUTTON_LMASK) != 0
    );

    return 1;
}

static int luasetquit(lua_State *L){
    int *quit = (int *)lua_touserdata(L, lua_upvalueindex(1));
    *quit = 1;

    return 0;
}

static int lualoadpref(lua_State *L){
    lua_settop(L, 1);
    if(!lua_isstring(L, 1)){
        return 0;
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 1);
    lua_concat(L, 2);

    FILE *f = fopen(lua_tostring(L, 2), "rb");
    if(!f){
        lua_pushliteral(L, "");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t sz = (size_t)ftell(f);
    rewind(f);

    char *buf = (char *)malloc(sz);
    fread(buf, 1, sz, f);
    lua_pushlstring(L, buf, sz);

    free(buf);
    fclose(f);

    return 1;
}

static int luasavepref(lua_State *L){
    lua_settop(L, 2);
    if(!lua_isstring(L, 1) || !lua_isstring(L, 2)){
        return 0;
    }
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
    lua_pushvalue(L, 3);
    lua_pushliteral(L, "~");
    lua_concat(L, 2);

    const char *dstfile = lua_tostring(L, 3);
    const char *tmpfile = lua_tostring(L, 4);

    FILE *f = fopen(tmpfile, "wb");
    if(!f){
        return 0;
    }
    size_t sz;
    const char *data = lua_tolstring(L, 2, &sz);

    fwrite(data, 1, sz, f);
    fclose(f);

    //Replace old destination file with temp file
    remove(dstfile);
    rename(tmpfile, dstfile);

    return 0;
}

#define APP_ENUM_BACKEND_SDL 0
#define APP_ENUM_BACKEND_OPENGL 1

#ifndef APP_BACKEND
#define APP_BACKEND APP_ENUM_BACKEND_SDL
#endif

#if APP_BACKEND == APP_ENUM_BACKEND_OPENGL
#include <GL/gl3w.h>

#include "render/opengl/luarendernode_opengl.h"
#include "render/opengl/renderlist_opengl.h"
#include "render/opengl/renderlist_lua_opengl.h"
#include "render/opengl/sprite_opengl.h"
#include "render/opengl/sprite_lua_opengl.h"
#include "render/opengl/primrect_opengl.h"
#include "render/opengl/primrect_lua_opengl.h"
#include "asset/bind/graphics/opengl/texture_binder_opengl.hpp"
#include "asset/bind/graphics/opengl/glyph_binder_opengl.hpp"
#include "asset/bind/graphics/opengl/font_binder_opengl.hpp"

typedef struct maindata{
    std::filesystem::path basepath;
    std::filesystem::path prefpath;

    lua_State *L;

    SDL_Window *wind;
    Enj_Renderer_OpenGL *rend;

    void *buf;
    Enj_Allocator allo;
    Enj_PoolAllocatorData dat;

    void *bufpr;
    Enj_Allocator allopr;
    Enj_PoolAllocatorData datpr;

    void *bufb;
    Enj_Allocator allob;
    Enj_PoolAllocatorData datb;
    Enj_ButtonList buttons;

    void *bufk;
    Enj_Allocator allok;
    Enj_PoolAllocatorData datk;
    Enj_KeyboardList keyboards;

    void *bufrl;
    Enj_Allocator allorl;
    Enj_PoolAllocatorData datrl;

    void *bufrn;
    Enj_Allocator allorn;
    Enj_PoolAllocatorData datrn;

    Enj_RenderList_OpenGL *renderlist;

} maindata;




#define MAX_ASSETS (1<<12)

#define MAX_SPRITES 1024


int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx);
int luadofilebasepath(lua_State *L){
    lua_settop(L, 1);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 3, 2);
        lua_pop(L, 1);
    }

    lua_callk(L, 1, LUA_MULTRET, 0, luadofilebasepath_cont);
    return luadofilebasepath_cont(L, LUA_OK, 0);
}
int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx){
    return lua_gettop(L);
}

int lualoadfilebasepath(lua_State *L){
    lua_settop(L, 3);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 5, 2);
        lua_pop(L, 1);
    }

    lua_call(L, 3, LUA_MULTRET);
    return lua_gettop(L);
}



int main(int argc, char **argv){
    int ecode = 0;

    maindata mdata;
    int isquit = 0;
    std::chrono::high_resolution_clock::time_point start;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    char *cwd = SDL_GetBasePath();
    mdata.basepath = std::filesystem::path(cwd);
    SDL_free(cwd);

    char *prefd = SDL_GetPrefPath(APP_COMPANY, APP_NAME);
    mdata.prefpath = std::filesystem::path(prefd);
    SDL_free(prefd);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetSwapInterval(1);

    mdata.wind = SDL_CreateWindow(
        APP_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        SDL_WINDOW_OPENGL
    );

    SDL_GL_CreateContext(mdata.wind);
    SDL_RaiseWindow(mdata.wind);

    gl3wInit();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mdata.rend = Enj_InitRenderer_OpenGL();

    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(mdata.wind);

    Mix_Init(0);
    //44100 Hz, default format, stereo, 2048 chunk size
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(16);



    mdata.L = luaL_newstate();

    luaL_requiref(mdata.L, "basic", luaopen_base, 0);
    luaL_requiref(mdata.L, "math", luaopen_math, 0);
    luaL_requiref(mdata.L, "string", luaopen_string, 0);
    luaL_requiref(mdata.L, "table", luaopen_table, 0);
    luaL_requiref(mdata.L, "utf8", luaopen_utf8, 0);
    lua_settop(mdata.L, 0);

    luaL_requiref(mdata.L, "package", luaopen_package, 0);
    //Set module search path
    lua_pushstring(mdata.L,
    (
        (mdata.basepath/"modules"/"?.lua").generic_string()
        + ';'
        + (mdata.basepath/"modules"/"?.lc").generic_string()
    ).c_str());
    lua_setfield(mdata.L, 1, "path");

    //Delete c library loader + all-in-one loader
    lua_getfield(mdata.L, 1, "searchers");
    lua_pushnil(mdata.L);
    lua_seti(mdata.L, 2, 4);
    lua_pushnil(mdata.L);
    lua_seti(mdata.L, 2, 3);

    //Delete loadlib function
    lua_pushnil(mdata.L);
    lua_setfield(mdata.L, 1, "loadlib");
    lua_settop(mdata.L, 0);


    //Make dofile, loadfile work with base path
    lua_getglobal(mdata.L, "dofile");
    lua_pushstring(mdata.L, mdata.basepath.generic_string().c_str());
    lua_pushcclosure(mdata.L, luadofilebasepath, 2);
    lua_setglobal(mdata.L, "dofile");

    lua_getglobal(mdata.L, "loadfile");
    lua_pushstring(mdata.L, mdata.basepath.generic_string().c_str());
    lua_pushcclosure(mdata.L, lualoadfilebasepath, 2);
    lua_setglobal(mdata.L, "loadfile");

    //Pref file functions with pref path
    lua_pushstring(mdata.L, mdata.prefpath.generic_string().c_str());
    lua_pushcclosure(mdata.L, lualoadpref, 1);
    lua_setglobal(mdata.L, "load_pref");

    lua_pushstring(mdata.L, mdata.prefpath.generic_string().c_str());
    lua_pushcclosure(mdata.L, luasavepref, 1);
    lua_setglobal(mdata.L, "save_pref");

    initbuiltins(mdata.L);

    //Make grid of renderlist
    mdata.bufrl = malloc(sizeof(Enj_RenderList_OpenGL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allorl, &mdata.datrl, mdata.bufrl, sizeof(Enj_RenderList_OpenGL)*MAX_SPRITES, sizeof(Enj_RenderList_OpenGL));
    mdata.bufrn = malloc(sizeof(Enj_RenderNode_OpenGL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allorn, &mdata.datrn, mdata.bufrn, sizeof(Enj_RenderNode_OpenGL)*MAX_SPRITES, sizeof(Enj_RenderNode_OpenGL));

    bindrender(mdata.L);

    //Make grid of sprites
    mdata.buf = malloc(sizeof(Enj_Sprite_OpenGL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allo, &mdata.dat, mdata.buf, sizeof(Enj_Sprite_OpenGL)*MAX_SPRITES, sizeof(Enj_Sprite_OpenGL));
    bindsprite_OpenGL(mdata.L, mdata.rend, &mdata.allo);
    //Make grid of rects
    mdata.bufpr = malloc(sizeof(Enj_PrimRect_OpenGL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allopr, &mdata.datpr, mdata.bufpr, sizeof(Enj_PrimRect_OpenGL)*MAX_SPRITES, sizeof(Enj_PrimRect_OpenGL));
    bindprimrect_OpenGL(mdata.L, mdata.rend, &mdata.allopr);

    bindrenderlist_OpenGL(mdata.L, mdata.rend,
        &mdata.allorl, &mdata.allorn, &mdata.allo, &mdata.allopr);

    //Init root render to renderlist
    mdata.renderlist = bindroot_renderlist_OpenGL(mdata.L, mdata.rend, &mdata.allorl, &mdata.allorn);

    //GUI button
    mdata.bufb = malloc(64*128);
    Enj_InitPoolAllocator(&mdata.allob, &mdata.datb, mdata.bufb, 64*128, sizeof(Enj_Button));
    Enj_InitButtonList(&mdata.buttons, &mdata.allob);
    bindbutton(mdata.L, &mdata.buttons);

    //GUI keyboard
    mdata.bufk = malloc(64*128);
    Enj_InitPoolAllocator(&mdata.allok, &mdata.datk, mdata.bufk, 64*128, sizeof(Enj_Keyboard));
    Enj_InitKeyboardList(&mdata.keyboards, &mdata.allok);
    bindkeyboard(mdata.L, &mdata.keyboards);


    multi_dispatch md;
    bool workeractive = true;

    std::thread worker([&md, &workeractive](){
        while(true){

            std::unique_lock lock(md.wq.mtx);

            while(md.wq.q.empty() & workeractive)
                md.cv.wait(lock);

            if(!workeractive) return;
            std::function<void()> f(std::move(md.wq.q.front()));
            md.wq.q.pop();

            lock.unlock();


            f();
        }
    });

    //Binding asset stuff
    bindasset(mdata.L);
    //Binding texture asset

    texture_binder_OpenGL texture_b(md, mdata.basepath, mdata.L);
    bindtexture(mdata.L, &texture_b,
        Enj_Lua_TextureOnPreload,
        Enj_Lua_TextureOnUnload,
        Enj_Lua_TextureOnCanUnload);

    glyph_binder_OpenGL glyph_b(md, mdata.L);
    bindglyph(mdata.L, &glyph_b,
        Enj_Lua_GlyphOnPreload,
        Enj_Lua_GlyphOnUnload,
        Enj_Lua_GlyphOnCanUnload);

    sound_binder sound_b(md, mdata.basepath, mdata.L);
    bindsound(mdata.L, &sound_b,
        Enj_Lua_SoundOnPreload,
        Enj_Lua_SoundOnUnload,
        Enj_Lua_SoundOnCanUnload);

    music_binder music_b(md, mdata.basepath, mdata.L);
    bindmusic(mdata.L, &music_b,
        Enj_Lua_MusicOnPreload,
        Enj_Lua_MusicOnUnload,
        Enj_Lua_MusicOnCanUnload);

    font_binder_OpenGL font_b(md, mdata.basepath, mdata.L, texture_b, glyph_b);
    bindfont(mdata.L, &font_b,
        Enj_Lua_FontOnPreload,
        Enj_Lua_FontOnUnload,
        Enj_Lua_FontOnCanUnload);

    data_binder data_b(md, mdata.basepath, mdata.L);
    binddata(mdata.L, &data_b,
        Enj_Lua_DataOnPreload,
        Enj_Lua_DataOnUnload,
        Enj_Lua_DataOnCanUnload);

    lua_register(mdata.L, "get_mousepos", luagetmousecoords);
    lua_register(mdata.L, "get_mousepressed", luagetmousepressed);

    lua_pushlightuserdata(mdata.L, &isquit);
    lua_pushcclosure(mdata.L, luasetquit, 1);
    lua_setglobal(mdata.L, "quit");


    bool windowminimized = false;

    //Start main lua script
    lua_pushcfunction(mdata.L, Enj_Lua_StartThread);
    if(luaL_loadfile(mdata.L, (mdata.basepath/"main.lua").generic_string().c_str())){
        printf("%s\n", lua_tostring(mdata.L, 2));
        lua_settop(mdata.L, 0);
    }
    else{
        lua_call(mdata.L, 1, 0);
    }


    /* Problem with SDL2 means IME doesn't show
    SDL_Rect textrect;
    textrect.x = 64;
    textrect.y = 64;
    textrect.w = 64;
    textrect.h = 16;

    SDL_SetTextInputRect(&textrect);
    */


    SDL_Event ev;

    while(!isquit){
        start = std::chrono::high_resolution_clock::now();

        if(Enj_GetKeyboardListSize(&mdata.keyboards)){
            for(
                Enj_Keyboard *kb = Enj_GetKeyboardListTail(&mdata.keyboards);
                kb;
                kb = kb->prev
            )
            {
                if(!kb->active) continue;

                if(kb->textmode && !SDL_IsTextInputActive()){
                    SDL_StartTextInput();
                }
                else if(!kb->textmode && SDL_IsTextInputActive()){
                    SDL_StopTextInput();
                }

                break;
            }
        }
        else if(SDL_IsTextInputActive()){
            SDL_StopTextInput();
        }

        while(SDL_PollEvent(&ev)){
            switch(ev.type){
            case SDL_QUIT:
                goto quit_app;
            case SDL_WINDOWEVENT:
                switch (ev.window.event){
                case SDL_WINDOWEVENT_MINIMIZED:
                    windowminimized = true;
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    windowminimized = false;
                    break;
                }
                break;
            case SDL_KEYDOWN:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->onpress){
                            (*itb->onpress)(
                                SDL_GetKeyName(ev.key.keysym.sym),
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_KEYUP:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->onunpress){
                            (*itb->onunpress)(
                                SDL_GetKeyName(ev.key.keysym.sym),
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_TEXTINPUT:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->oninput){
                            (*itb->oninput)(
                                ev.text.text,
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(ev.button.button == SDL_BUTTON_LEFT){
                    for(
                        Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        int xrel = ev.button.x - itb->rect.x;
                        int yrel = ev.button.y - itb->rect.y;
                        if(!( (xrel >= 0)
                            & (xrel < itb->rect.w)
                            & (yrel >= 0)
                            & (yrel < itb->rect.h)))
                            continue;

                        if(itb->pressing) continue;

                        if(itb->onpress){
                            (*itb->onpress)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);

                        }
                        itb->pressing = 1;

                        break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                //Different from mouse button down in that it does not
                //consider bounding boxes
                if(ev.button.button == SDL_BUTTON_LEFT){
                    for(
                        Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(!itb->pressing) continue;

                        if(itb->onunpress){
                            (*itb->onunpress)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);
                        }
                        itb->pressing = 0;
                    }
                }
                break;
            case SDL_MOUSEMOTION:{

                char hoverfound = 0;

                for(
                    Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    int xrel = ev.button.x - itb->rect.x;
                    int yrel = ev.button.y - itb->rect.y;
                    if( !hoverfound
                        & (xrel >= 0)
                        & (xrel < itb->rect.w)
                        & (yrel >= 0)
                        & (yrel < itb->rect.h)){

                        if(!itb->hovering){
                            itb->hovering = 1;
                            if(itb->onhover){
                                (*itb->onhover)(
                                    ev.button.x,
                                    ev.button.y,
                                    itb->data);

                            }
                        }

                        hoverfound = 1;
                    }
                    else{
                        if(itb->hovering){
                            itb->hovering = 0;
                            if(itb->onunhover){
                                (*itb->onunhover)(
                                    ev.button.x,
                                    ev.button.y,
                                    itb->data);

                            }
                        }
                    }
                }
                }
                break;
            default:
                break;
            }
        }

        updateluaact(mdata.L);

        if(!windowminimized){
            glClear(GL_COLOR_BUFFER_BIT);

            float idmat[4] = {1,0,0,1};
            float idt[2] = {0,0};

            Enj_RendererBegin_OpenGL(mdata.rend);
            Enj_RenderList_OnRender_OpenGL(mdata.renderlist, mdata.rend, idmat, idt);
            Enj_RendererFlush_OpenGL(mdata.rend);

            SDL_GL_SwapWindow(mdata.wind);
        }


        do {
            std::unique_lock lock(md.mq.mtx);
            if(!md.mq.q.empty()){
                std::function<void()> f(std::move(md.mq.q.front()));
                md.mq.q.pop();

                lock.unlock();

                f();
                continue;
            }
            lock.unlock();

            if(std::chrono::high_resolution_clock::now() - start
            < std::chrono::duration<double>(1./60. - 0.002)){
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

        }while(std::chrono::high_resolution_clock::now() - start
        < std::chrono::duration<double>(1./60.));
    }

quit_app:
    {
        std::lock_guard lock(md.wq.mtx);
        workeractive = false;
    }
    md.cv.notify_one();
    worker.join();

    for(Enj_ListNode *it = mdata.renderlist->list.head; it; it = it->next){
        Enj_RenderNode_OpenGL *node = (Enj_RenderNode_OpenGL *)
            ((char *)it - offsetof(Enj_RenderNode_OpenGL, listnode));
        (*node->onfreedata)(node->data, node->ctx, node->allocdata);
    }
    Enj_RenderListFree_OpenGL(mdata.renderlist);

    free(mdata.bufrn);
    free(mdata.bufrl);

    Enj_FreeKeyboardList(&mdata.keyboards);
    free(mdata.bufk);

    Enj_FreeButtonList(&mdata.buttons);
    free(mdata.bufb);

    free(mdata.bufpr);
    free(mdata.buf);

    Mix_CloseAudio();
    Mix_Quit();

    lua_close(mdata.L);

    SDL_DestroyWindow(mdata.wind);

    SDL_Quit();
    return ecode;
}


#else

#include "render/sdl/luarendernode_sdl.h"
#include "render/sdl/renderlist_sdl.h"
#include "render/sdl/renderlist_lua_sdl.h"
#include "render/sdl/sprite_sdl.h"
#include "render/sdl/sprite_lua_sdl.h"
#include "render/sdl/primrect_sdl.h"
#include "render/sdl/primrect_lua_sdl.h"
#include "asset/bind/graphics/sdl/texture_binder_sdl.hpp"
#include "asset/bind/graphics/sdl/glyph_binder_sdl.hpp"
#include "asset/bind/graphics/sdl/font_binder_sdl.hpp"

typedef struct maindata{
    std::filesystem::path basepath;
    std::filesystem::path prefpath;

    lua_State *L;

    SDL_Window *wind;
    SDL_Renderer *rend;

    void *buf;
    Enj_Allocator allo;
    Enj_PoolAllocatorData dat;

    void *bufpr;
    Enj_Allocator allopr;
    Enj_PoolAllocatorData datpr;

    void *bufb;
    Enj_Allocator allob;
    Enj_PoolAllocatorData datb;
    Enj_ButtonList buttons;

    void *bufk;
    Enj_Allocator allok;
    Enj_PoolAllocatorData datk;
    Enj_KeyboardList keyboards;

    void *bufrl;
    Enj_Allocator allorl;
    Enj_PoolAllocatorData datrl;

    void *bufrn;
    Enj_Allocator allorn;
    Enj_PoolAllocatorData datrn;

    Enj_RenderList_SDL *renderlist;

} maindata;




#define MAX_ASSETS (1<<12)

#define MAX_SPRITES 1024


int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx);
int luadofilebasepath(lua_State *L){
    lua_settop(L, 1);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 3, 2);
        lua_pop(L, 1);
    }

    lua_callk(L, 1, LUA_MULTRET, 0, luadofilebasepath_cont);
    return luadofilebasepath_cont(L, LUA_OK, 0);
}
int luadofilebasepath_cont(lua_State *L, int status, lua_KContext ctx){
    return lua_gettop(L);
}

int lualoadfilebasepath(lua_State *L){
    lua_settop(L, 3);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_rotate(L, 1, 1);

    if(lua_isstring(L, 2)){
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_pushvalue(L, 2);
        lua_concat(L, 2);
        lua_copy(L, 5, 2);
        lua_pop(L, 1);
    }

    lua_call(L, 3, LUA_MULTRET);
    return lua_gettop(L);
}



int main(int argc, char **argv){
    int ecode = 0;

    maindata mdata;
    int isquit = 0;
    std::chrono::high_resolution_clock::time_point start;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    char *cwd = SDL_GetBasePath();
    mdata.basepath = std::filesystem::path(cwd);
    SDL_free(cwd);

    char *prefd = SDL_GetPrefPath(APP_COMPANY, APP_NAME);
    mdata.prefpath = std::filesystem::path(prefd);
    SDL_free(prefd);

    mdata.wind = SDL_CreateWindow(
        APP_NAME,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        0
    );
    SDL_RaiseWindow(mdata.wind);


    mdata.rend = SDL_CreateRenderer(
        mdata.wind,
        -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    );
    SDL_SetRenderDrawColor(mdata.rend,
                0,
                0,
                0,
                255);
    SDL_RenderClear(mdata.rend);
    SDL_RenderPresent(mdata.rend);

    SDL_RendererInfo info;
    SDL_GetRendererInfo(mdata.rend, &info);


    Mix_Init(0);
    //44100 Hz, default format, stereo, 2048 chunk size
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(16);



    mdata.L = luaL_newstate();

    luaL_requiref(mdata.L, "basic", luaopen_base, 0);
    luaL_requiref(mdata.L, "math", luaopen_math, 0);
    luaL_requiref(mdata.L, "string", luaopen_string, 0);
    luaL_requiref(mdata.L, "table", luaopen_table, 0);
    luaL_requiref(mdata.L, "utf8", luaopen_utf8, 0);
    lua_settop(mdata.L, 0);

    luaL_requiref(mdata.L, "package", luaopen_package, 0);
    //Set module search path
    lua_pushstring(mdata.L,
    (
        (mdata.basepath/"modules"/"?.lua").generic_string()
        + ';'
        + (mdata.basepath/"modules"/"?.lc").generic_string()
    ).c_str());
    lua_setfield(mdata.L, 1, "path");

    //Delete c library loader + all-in-one loader
    lua_getfield(mdata.L, 1, "searchers");
    lua_pushnil(mdata.L);
    lua_seti(mdata.L, 2, 4);
    lua_pushnil(mdata.L);
    lua_seti(mdata.L, 2, 3);

    //Delete loadlib function
    lua_pushnil(mdata.L);
    lua_setfield(mdata.L, 1, "loadlib");
    lua_settop(mdata.L, 0);


    //Make dofile, loadfile work with base path
    lua_getglobal(mdata.L, "dofile");
    lua_pushstring(mdata.L, mdata.basepath.generic_string().c_str());
    lua_pushcclosure(mdata.L, luadofilebasepath, 2);
    lua_setglobal(mdata.L, "dofile");

    lua_getglobal(mdata.L, "loadfile");
    lua_pushstring(mdata.L, mdata.basepath.generic_string().c_str());
    lua_pushcclosure(mdata.L, lualoadfilebasepath, 2);
    lua_setglobal(mdata.L, "loadfile");

    //Pref file functions with pref path
    lua_pushstring(mdata.L, mdata.prefpath.generic_string().c_str());
    lua_pushcclosure(mdata.L, lualoadpref, 1);
    lua_setglobal(mdata.L, "load_pref");

    lua_pushstring(mdata.L, mdata.prefpath.generic_string().c_str());
    lua_pushcclosure(mdata.L, luasavepref, 1);
    lua_setglobal(mdata.L, "save_pref");

    initbuiltins(mdata.L);

    //Make grid of renderlist
    mdata.bufrl = malloc(sizeof(Enj_RenderList_SDL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allorl, &mdata.datrl, mdata.bufrl, sizeof(Enj_RenderList_SDL)*MAX_SPRITES, sizeof(Enj_RenderList_SDL));
    mdata.bufrn = malloc(sizeof(Enj_RenderNode_SDL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allorn, &mdata.datrn, mdata.bufrn, sizeof(Enj_RenderNode_SDL)*MAX_SPRITES, sizeof(Enj_RenderNode_SDL));

    bindrender(mdata.L);

    //Make grid of sprites
    mdata.buf = malloc(sizeof(Enj_Sprite_SDL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allo, &mdata.dat, mdata.buf, sizeof(Enj_Sprite_SDL)*MAX_SPRITES, sizeof(Enj_Sprite_SDL));
    bindsprite_SDL(mdata.L, mdata.rend, &mdata.allo);
    //Make grid of rects
    mdata.bufpr = malloc(sizeof(Enj_PrimRect_SDL)*MAX_SPRITES);
    Enj_InitPoolAllocator(&mdata.allopr, &mdata.datpr, mdata.bufpr, sizeof(Enj_PrimRect_SDL)*MAX_SPRITES, sizeof(Enj_PrimRect_SDL));
    bindprimrect_SDL(mdata.L, mdata.rend, &mdata.allopr);

    bindrenderlist_SDL(mdata.L, mdata.rend,
        &mdata.allorl, &mdata.allorn, &mdata.allo, &mdata.allopr);

    //Init root render to renderlist
    mdata.renderlist = bindroot_renderlist_SDL(mdata.L, mdata.rend, &mdata.allorl, &mdata.allorn);

    //GUI button
    mdata.bufb = malloc(64*128);
    Enj_InitPoolAllocator(&mdata.allob, &mdata.datb, mdata.bufb, 64*128, sizeof(Enj_Button));
    Enj_InitButtonList(&mdata.buttons, &mdata.allob);
    bindbutton(mdata.L, &mdata.buttons);

    //GUI keyboard
    mdata.bufk = malloc(64*128);
    Enj_InitPoolAllocator(&mdata.allok, &mdata.datk, mdata.bufk, 64*128, sizeof(Enj_Keyboard));
    Enj_InitKeyboardList(&mdata.keyboards, &mdata.allok);
    bindkeyboard(mdata.L, &mdata.keyboards);


    multi_dispatch md;
    bool workeractive = true;

    std::thread worker([&md, &workeractive](){
        while(true){

            std::unique_lock lock(md.wq.mtx);

            while(md.wq.q.empty() & workeractive)
                md.cv.wait(lock);

            if(!workeractive) return;
            std::function<void()> f(std::move(md.wq.q.front()));
            md.wq.q.pop();

            lock.unlock();


            f();
        }
    });

    //Binding asset stuff
    bindasset(mdata.L);
    //Binding texture asset

    texture_binder_SDL texture_b(md, mdata.basepath, mdata.rend, mdata.L);
    bindtexture(mdata.L, &texture_b,
        Enj_Lua_TextureOnPreload,
        Enj_Lua_TextureOnUnload,
        Enj_Lua_TextureOnCanUnload);

    glyph_binder_SDL glyph_b(md, mdata.L);
    bindglyph(mdata.L, &glyph_b,
        Enj_Lua_GlyphOnPreload,
        Enj_Lua_GlyphOnUnload,
        Enj_Lua_GlyphOnCanUnload);

    sound_binder sound_b(md, mdata.basepath, mdata.L);
    bindsound(mdata.L, &sound_b,
        Enj_Lua_SoundOnPreload,
        Enj_Lua_SoundOnUnload,
        Enj_Lua_SoundOnCanUnload);

    music_binder music_b(md, mdata.basepath, mdata.L);
    bindmusic(mdata.L, &music_b,
        Enj_Lua_MusicOnPreload,
        Enj_Lua_MusicOnUnload,
        Enj_Lua_MusicOnCanUnload);

    font_binder_SDL font_b(md, mdata.basepath, mdata.rend, mdata.L, texture_b, glyph_b);
    bindfont(mdata.L, &font_b,
        Enj_Lua_FontOnPreload,
        Enj_Lua_FontOnUnload,
        Enj_Lua_FontOnCanUnload);

    data_binder data_b(md, mdata.basepath, mdata.L);
    binddata(mdata.L, &data_b,
        Enj_Lua_DataOnPreload,
        Enj_Lua_DataOnUnload,
        Enj_Lua_DataOnCanUnload);

    lua_register(mdata.L, "get_mousepos", luagetmousecoords);
    lua_register(mdata.L, "get_mousepressed", luagetmousepressed);

    lua_pushlightuserdata(mdata.L, &isquit);
    lua_pushcclosure(mdata.L, luasetquit, 1);
    lua_setglobal(mdata.L, "quit");


    bool windowminimized = false;

    //Start main lua script
    lua_pushcfunction(mdata.L, Enj_Lua_StartThread);
    if(luaL_loadfile(mdata.L, (mdata.basepath/"main.lua").generic_string().c_str())){
        printf("%s\n", lua_tostring(mdata.L, 2));
        lua_settop(mdata.L, 0);
    }
    else{
        lua_call(mdata.L, 1, 0);
    }


    /* Problem with SDL2 means IME doesn't show
    SDL_Rect textrect;
    textrect.x = 64;
    textrect.y = 64;
    textrect.w = 64;
    textrect.h = 16;

    SDL_SetTextInputRect(&textrect);
    */


    SDL_Event ev;

    while(!isquit){
        start = std::chrono::high_resolution_clock::now();

        if(Enj_GetKeyboardListSize(&mdata.keyboards)){
            for(
                Enj_Keyboard *kb = Enj_GetKeyboardListTail(&mdata.keyboards);
                kb;
                kb = kb->prev
            )
            {
                if(!kb->active) continue;

                if(kb->textmode && !SDL_IsTextInputActive()){
                    SDL_StartTextInput();
                }
                else if(!kb->textmode && SDL_IsTextInputActive()){
                    SDL_StopTextInput();
                }

                break;
            }
        }
        else if(SDL_IsTextInputActive()){
            SDL_StopTextInput();
        }

        while(SDL_PollEvent(&ev)){
            switch(ev.type){
            case SDL_QUIT:
                goto quit_app;
            case SDL_WINDOWEVENT:
                switch (ev.window.event){
                case SDL_WINDOWEVENT_MINIMIZED:
                    windowminimized = true;
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    windowminimized = false;
                    break;
                }
                break;
            case SDL_KEYDOWN:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->onpress){
                            (*itb->onpress)(
                                SDL_GetKeyName(ev.key.keysym.sym),
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_KEYUP:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->onunpress){
                            (*itb->onunpress)(
                                SDL_GetKeyName(ev.key.keysym.sym),
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_TEXTINPUT:{
                    for(
                        Enj_Keyboard *itb = Enj_GetKeyboardListTail(&mdata.keyboards);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(itb->oninput){
                            (*itb->oninput)(
                                ev.text.text,
                                itb->data);
                        }

                        break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(ev.button.button == SDL_BUTTON_LEFT){
                    for(
                        Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        int xrel = ev.button.x - itb->rect.x;
                        int yrel = ev.button.y - itb->rect.y;
                        if(!( (xrel >= 0)
                            & (xrel < itb->rect.w)
                            & (yrel >= 0)
                            & (yrel < itb->rect.h)))
                            continue;

                        if(itb->pressing) continue;

                        if(itb->onpress){
                            (*itb->onpress)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);

                        }
                        itb->pressing = 1;

                        break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                //Different from mouse button down in that it does not
                //consider bounding boxes
                if(ev.button.button == SDL_BUTTON_LEFT){
                    for(
                        Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                        itb;
                        itb = itb->prev
                    )
                    {
                        if(!itb->active) continue;

                        if(!itb->pressing) continue;

                        if(itb->onunpress){
                            (*itb->onunpress)(
                                ev.button.x,
                                ev.button.y,
                                itb->data);
                        }
                        itb->pressing = 0;
                    }
                }
                break;
            case SDL_MOUSEMOTION:{

                char hoverfound = 0;

                for(
                    Enj_Button *itb = Enj_GetButtonListTail(&mdata.buttons);
                    itb;
                    itb = itb->prev
                )
                {
                    if(!itb->active) continue;

                    int xrel = ev.button.x - itb->rect.x;
                    int yrel = ev.button.y - itb->rect.y;
                    if( !hoverfound
                        & (xrel >= 0)
                        & (xrel < itb->rect.w)
                        & (yrel >= 0)
                        & (yrel < itb->rect.h)){

                        if(!itb->hovering){
                            itb->hovering = 1;
                            if(itb->onhover){
                                (*itb->onhover)(
                                    ev.button.x,
                                    ev.button.y,
                                    itb->data);

                            }
                        }

                        hoverfound = 1;
                    }
                    else{
                        if(itb->hovering){
                            itb->hovering = 0;
                            if(itb->onunhover){
                                (*itb->onunhover)(
                                    ev.button.x,
                                    ev.button.y,
                                    itb->data);

                            }
                        }
                    }
                }
                }
                break;
            default:
                break;
            }
        }

        updateluaact(mdata.L);

        if(!windowminimized){
            SDL_SetRenderDrawColor(mdata.rend,
                0,
                0,
                0,
                255);
            SDL_RenderClear(mdata.rend);

            Enj_RenderList_OnRender_SDL(mdata.renderlist, mdata.rend, 0, 0);

            SDL_RenderPresent(mdata.rend);
        }


        do {
            std::unique_lock lock(md.mq.mtx);
            if(!md.mq.q.empty()){
                std::function<void()> f(std::move(md.mq.q.front()));
                md.mq.q.pop();

                lock.unlock();

                f();
                continue;
            }
            lock.unlock();

            if(std::chrono::high_resolution_clock::now() - start
            < std::chrono::duration<double>(1./60. - 0.002)){
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

        }while(std::chrono::high_resolution_clock::now() - start
        < std::chrono::duration<double>(1./60.));
    }

quit_app:
    {
        std::lock_guard lock(md.wq.mtx);
        workeractive = false;
    }
    md.cv.notify_one();
    worker.join();

    for(Enj_ListNode *it = mdata.renderlist->list.head; it; it = it->next){
        Enj_RenderNode_SDL *node = (Enj_RenderNode_SDL *)
            ((char *)it - offsetof(Enj_RenderNode_SDL, listnode));
        (*node->onfreedata)(node->data, node->ctx, node->allocdata);
    }
    Enj_RenderListFree_SDL(mdata.renderlist);

    free(mdata.bufrn);
    free(mdata.bufrl);

    Enj_FreeKeyboardList(&mdata.keyboards);
    free(mdata.bufk);

    Enj_FreeButtonList(&mdata.buttons);
    free(mdata.bufb);

    free(mdata.bufpr);
    free(mdata.buf);

    Mix_CloseAudio();
    Mix_Quit();

    lua_close(mdata.L);

    SDL_DestroyRenderer(mdata.rend);
    SDL_DestroyWindow(mdata.wind);

    SDL_Quit();
    return ecode;
}

#endif
