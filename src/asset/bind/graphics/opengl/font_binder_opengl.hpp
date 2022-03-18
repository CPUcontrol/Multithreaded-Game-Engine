#pragma once

class texture_binder_OpenGL;
class glyph_binder_OpenGL;

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <string>

class font_binder_OpenGL{
private:
public:
    texture_binder_OpenGL &texturebinder;
    glyph_binder_OpenGL &glyphbinder;
    multi_dispatch &dispatch;
    std::string &basepath;

    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    font_binder_OpenGL(
        multi_dispatch &md,
        std::string &base,
        lua_State *L,
        texture_binder_OpenGL &tb,
        glyph_binder_OpenGL &gb
    );
    ~font_binder_OpenGL();

    font_binder_OpenGL(const font_binder_OpenGL &o) = delete;
    font_binder_OpenGL & operator=(const font_binder_OpenGL &o) = delete;

    font_binder_OpenGL(font_binder_OpenGL &&o) = delete;
    font_binder_OpenGL & operator=(font_binder_OpenGL &&o) = delete;


    operator bool(){return true;}
};
