#include "font_binder_sdl.hpp"


font_binder_SDL::font_binder_SDL(
        multi_dispatch &md,
        std::filesystem::path &base,
        SDL_Renderer *rend,
        lua_State *L,
        texture_binder_SDL &tb,
        glyph_binder_SDL &gb
): dispatch(md), basepath(base), rend(rend), Lmain(L), texturebinder(tb), glyphbinder(gb)
{

}

font_binder_SDL::~font_binder_SDL(){

}
