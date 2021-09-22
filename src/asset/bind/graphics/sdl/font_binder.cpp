#include "font_binder.hpp"


font_binder::font_binder(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L,
        texture_binder &tb,
        glyph_binder &gb
): dispatch(md), basepath(base), rend(rend), Lmain(L), texturebinder(tb), glyphbinder(gb)
{

}

font_binder::~font_binder(){

}
