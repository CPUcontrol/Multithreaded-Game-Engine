#pragma once

class texture_binder_SDL;
class glyph_binder_SDL;

typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <filesystem>

class font_binder_SDL{
private:
public:
    texture_binder_SDL &texturebinder;
    glyph_binder_SDL &glyphbinder;
    multi_dispatch &dispatch;
    std::filesystem::path &basepath;

    SDL_Renderer *rend;
    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    font_binder_SDL(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L,
        texture_binder_SDL &tb,
        glyph_binder_SDL &gb
    );
    ~font_binder_SDL();

    font_binder_SDL(const font_binder_SDL &o) = delete;
    font_binder_SDL & operator=(const font_binder_SDL &o) = delete;

    font_binder_SDL(font_binder_SDL &&o) = delete;
    font_binder_SDL & operator=(font_binder_SDL &&o) = delete;


    operator bool(){return true;}
};
