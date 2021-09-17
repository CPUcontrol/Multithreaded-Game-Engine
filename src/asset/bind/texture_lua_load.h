#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_TextureOnPreload(lua_State *L);
int Enj_Lua_TextureOnUnload(lua_State *L);
int Enj_Lua_TextureOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
