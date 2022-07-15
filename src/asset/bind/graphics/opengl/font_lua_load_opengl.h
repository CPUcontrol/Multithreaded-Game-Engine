#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_FontOnPreload_OpenGL(lua_State *L);
int Enj_Lua_FontOnUnload_OpenGL(lua_State *L);
int Enj_Lua_FontOnCanUnload_OpenGL(lua_State *L);

#ifdef __cplusplus
}
#endif
