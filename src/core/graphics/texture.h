#pragma once

#define APP_ENUM_BACKEND_SDL 0
#define APP_ENUM_BACKEND_OPENGL 1

#ifndef APP_BACKEND
#define APP_BACKEND APP_ENUM_BACKEND_SDL
#endif

#if APP_BACKEND == APP_ENUM_BACKEND_OPENGL

#include "opengl/texture_opengl.h"
typedef Enj_Texture_OpenGL Enj_Texture;

#else

#include "sdl/texture_sdl.h"
typedef Enj_Texture_SDL Enj_Texture;

#endif
