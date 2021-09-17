#pragma once
#include <SDL_mixer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Enj_Sound{
    Mix_Chunk *chunk;
}Enj_Sound;

#ifdef __cplusplus
}
#endif
