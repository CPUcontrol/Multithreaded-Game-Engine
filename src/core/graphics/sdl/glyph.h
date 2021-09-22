#pragma once
#include <SDL_rect.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Texture Enj_Texture;

typedef struct Enj_Glyph{
    SDL_Rect rect;
    Enj_Texture *texture;
}Enj_Glyph;

#ifdef __cplusplus
}
#endif
