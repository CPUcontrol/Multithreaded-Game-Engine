#pragma once

#define APP_ENUM_BACKEND_SDL 0
#define APP_ENUM_BACKEND_OPENGL 1

#ifndef APP_BACKEND
#define APP_BACKEND APP_ENUM_BACKEND_SDL
#endif

#if APP_BACKEND == APP_ENUM_BACKEND_OPENGL

#include "opengl/render_opengl.h"
typedef Enj_Renderer_OpenGL Enj_Renderer;

#else

#include "sdl/render_sdl.h"
typedef Enj_Renderer_SDL Enj_Renderer;

#endif
