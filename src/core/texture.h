#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_Texture SDL_Texture;

typedef struct Enj_Texture{
    SDL_Texture *tx;
    unsigned short width;
    unsigned short height;
}Enj_Texture;

#ifdef __cplusplus
}
#endif
