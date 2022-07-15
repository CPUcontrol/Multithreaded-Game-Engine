#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_SoundOnPreload(lua_State *L);
int Enj_Lua_SoundOnUnload(lua_State *L);
int Enj_Lua_SoundOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
