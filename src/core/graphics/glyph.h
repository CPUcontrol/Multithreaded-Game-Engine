#pragma once

#define APP_ENUM_BACKEND_SDL 0
#define APP_ENUM_BACKEND_OPENGL 1

#ifndef APP_BACKEND
#define APP_BACKEND APP_ENUM_BACKEND_SDL
#endif

#if APP_BACKEND == APP_ENUM_BACKEND_OPENGL

#include "opengl/glyph_opengl.h"
typedef Enj_Glyph_OpenGL Enj_Glyph;

#else

#include "sdl/glyph_sdl.h"
typedef Enj_Glyph_SDL Enj_Glyph;

#endif
