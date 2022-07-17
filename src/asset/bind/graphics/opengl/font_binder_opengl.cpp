#include "font_binder_opengl.hpp"


font_binder_OpenGL::font_binder_OpenGL(
        multi_dispatch &md,
        const char *base,
        lua_State *L,
        texture_binder_OpenGL &tb,
        glyph_binder_OpenGL &gb
): dispatch(md), basepath(base), Lmain(L), texturebinder(tb), glyphbinder(gb)
{

}

font_binder_OpenGL::~font_binder_OpenGL(){

}
