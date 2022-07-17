#include <stdio.h>
#include <string.h>

#include <chrono>
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

#include "asset/luaasset.h"
#include "asset/asset_lua.h"
#include "asset/lua_extra_asset.h"
#include "asset/graphics/texture_lua.h"
#include "asset/graphics/glyph_lua.h"
#include "asset/audio/sound_lua.h"
#include "asset/audio/music_lua.h"
#include "asset/graphics/font_lua.h"
#include "asset/misc/data_lua.h"
#include "asset/bind/audio/sound_lua_load.h"
#include "asset/bind/audio/music_lua_load.h"
#include "asset/bind/misc/data_lua_load.h"

#include "asset/bind/audio/sound_binder.hpp"
#include "asset/bind/audio/music_binder.hpp"
#include "asset/bind/misc/data_binder.hpp"
#include "asset/util/multi_dispatch.hpp"

#include "render/render.h"
#include "render/render_lua.h"

#include "input/input.h"

#include "appconfig.h"
#include "embed_lua.h"

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
        SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK
    );

    return 1;
}

static int luagetfullscreen(lua_State *L){
    SDL_Window *window = (SDL_Window *)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushboolean(L, SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);

    return 1;
}

static int luasetfullscreen(lua_State *L){
    if(!lua_isboolean(L, 1)){
        return 0;
    }

    SDL_Window *window = (SDL_Window *)lua_touserdata(L, lua_upvalueindex(1));
    int fulltowindowed =
        ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0)
        & !lua_toboolean(L, 1);
    SDL_SetWindowFullscreen(
        window,
        lua_toboolean(L, 1) ? SDL_WINDOW_FULLSCREEN : 0
    );

    if (fulltowindowed)
        SDL_SetWindowPosition(
            window,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED
        );

    return 0;
}

static int luagetresolution(lua_State *L){
    SDL_Window *window = (SDL_Window *)lua_touserdata(L, lua_upvalueindex(1));
    if(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN){
        SDL_DisplayMode mode;
        SDL_GetWindowDisplayMode(window, &mode);

        lua_pushinteger(L, mode.w);
        lua_pushinteger(L, mode.h);
    }
    else{
        int w;
        int h;
        SDL_GetWindowSize(window, &w, &h);

        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
    }

    return 2;
}

static int luasetquit(lua_State *L){
    int *quit = (int *)lua_touserdata(L, lua_upvalueindex(1));
    *quit = 1;

    return 0;
}


#define MAX_SPRITES (1<<14)

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
#include "asset/bind/graphics/opengl/texture_lua_load_opengl.h"
#include "asset/bind/graphics/opengl/glyph_lua_load_opengl.h"
#include "asset/bind/graphics/opengl/font_lua_load_opengl.h"

static int luasetresolution(lua_State *L){
    if(!lua_isinteger(L, 1) | !lua_isinteger(L, 2)){
        return 0;
    }

    SDL_Window *window = (SDL_Window *)lua_touserdata(L, lua_upvalueindex(1));

    SDL_DisplayMode mode;
    SDL_GetWindowDisplayMode(window, &mode);

    int w = (int)lua_tointeger(L, 1);
    int h = (int)lua_tointeger(L, 2);
    mode.w = w;
    mode.h = h;

    SDL_SetWindowDisplayMode(window, &mode);

    SDL_SetWindowSize(window, w, h);

    Enj_Renderer_OpenGL *render =
        (Enj_Renderer_OpenGL *)lua_touserdata(L, lua_upvalueindex(2));

    Enj_RendererSetResolution_OpenGL(
        render,
        (unsigned int)w,
        (unsigned int)h
    );

    return 0;
}

typedef struct maindata{
    const char *basepath;
    const char *prefpath;

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


int main(int argc, char **argv){
    int ecode = 0;

    maindata mdata;
    int isquit = 0;
    std::chrono::steady_clock::time_point start;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    char *cwd = SDL_GetBasePath();
    mdata.basepath = cwd;

    char *prefd = SDL_GetPrefPath(APP_COMPANY, APP_NAME);
    mdata.prefpath = prefd;

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

    embed_lua(mdata.L, cwd, prefd);

    //Fullscreen and resolution functions
    lua_pushlightuserdata(mdata.L, mdata.wind);
    lua_pushcclosure(mdata.L, luagetfullscreen, 1);
    lua_setglobal(mdata.L, "get_fullscreen");

    lua_pushlightuserdata(mdata.L, mdata.wind);
    lua_pushcclosure(mdata.L, luasetfullscreen, 1);
    lua_setglobal(mdata.L, "set_fullscreen");

    lua_pushlightuserdata(mdata.L, mdata.wind);
    lua_pushcclosure(mdata.L, luagetresolution, 1);
    lua_setglobal(mdata.L, "get_resolution");

    lua_pushlightuserdata(mdata.L, mdata.wind);
    lua_pushlightuserdata(mdata.L, mdata.rend);
    lua_pushcclosure(mdata.L, luasetresolution, 2);
    lua_setglobal(mdata.L, "set_resolution");

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

            std::unique_lock<std::mutex> lock(md.wq.mtx);

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
        Enj_Lua_TextureOnPreload_OpenGL,
        Enj_Lua_TextureOnUnload_OpenGL,
        Enj_Lua_TextureOnCanUnload_OpenGL);

    glyph_binder_OpenGL glyph_b(md, mdata.L);
    bindglyph(mdata.L, &glyph_b,
        Enj_Lua_GlyphOnPreload_OpenGL,
        Enj_Lua_GlyphOnUnload_OpenGL,
        Enj_Lua_GlyphOnCanUnload_OpenGL);

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
        Enj_Lua_FontOnPreload_OpenGL,
        Enj_Lua_FontOnUnload_OpenGL,
        Enj_Lua_FontOnCanUnload_OpenGL);

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

    Enj_InputState input;
    Enj_InitInputState(&input);

    //Start main lua script
    lua_pushcfunction(mdata.L, Enj_Lua_StartThread);
    if(luaL_loadfile(mdata.L, (std::string(mdata.basepath) + "main.lua").c_str())){
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


    while(!isquit){
        start = std::chrono::steady_clock::now();

        if(Enj_HandleInput(&input, &mdata.buttons, &mdata.keyboards))
            break;

        updateluaact(mdata.L);

        if(!input.windowminimized){
            Enj_RendererVisit_OpenGL(mdata.rend, mdata.renderlist);
            SDL_GL_SwapWindow(mdata.wind);
        }


        do {
            std::unique_lock<std::mutex> lock(md.mq.mtx);
            if(!md.mq.q.empty()){
                std::function<void()> f(std::move(md.mq.q.front()));
                md.mq.q.pop();

                lock.unlock();

                f();
                continue;
            }
            lock.unlock();

            if(
                std::chrono::steady_clock::now() - start
                < std::chrono::duration<int, std::ratio<1, 60>>(1)
                    - std::chrono::milliseconds(2)
            )
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

        }while(std::chrono::steady_clock::now() - start
        < std::chrono::duration<int, std::ratio<1, 60>>(1));
    }

    {
        std::lock_guard<std::mutex> lock(md.wq.mtx);
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

    SDL_free(prefd);
    SDL_free(cwd);

    SDL_Quit();
    return ecode;
}
