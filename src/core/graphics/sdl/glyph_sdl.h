#pragma once
#include <SDL_rect.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Texture_SDL Enj_Texture_SDL;

typedef struct Enj_Glyph_SDL{
    SDL_Rect rect;
    Enj_Texture_SDL *texture;
}Enj_Glyph_SDL;

#ifdef __cplusplus
}
#endif
