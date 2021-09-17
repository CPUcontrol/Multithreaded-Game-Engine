#pragma once

class texture_binder;
class glyph_binder;

typedef struct SDL_Renderer SDL_Renderer;
typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <filesystem>

class font_binder{
private:
public:
    texture_binder &texturebinder;
    glyph_binder &glyphbinder;
    multi_dispatch &dispatch;
    std::filesystem::path &basepath;

    SDL_Renderer *rend;
    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    font_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L,
        texture_binder &tb,
        glyph_binder &gb
    );
    ~font_binder();

    font_binder(const font_binder &o) = delete;
    font_binder & operator=(const font_binder &o) = delete;

    font_binder(font_binder &&o) = delete;
    font_binder & operator=(font_binder &&o) = delete;


    operator bool(){return true;}
};
