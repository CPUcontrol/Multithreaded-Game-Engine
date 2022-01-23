#pragma once
#include <SDL_mixer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Music{
    Mix_Music *music;
}Enj_Music;

#ifdef __cplusplus
}
#endif
