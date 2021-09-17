#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct luafont{
    int height;
}luafont;

typedef struct luafontchar{
    short xoffset;
    short yoffset;
    short advance;
} luafontchar;

#ifdef __cplusplus
}
#endif
