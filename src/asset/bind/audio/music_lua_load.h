#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_MusicOnPreload(lua_State *L);
int Enj_Lua_MusicOnUnload(lua_State *L);
int Enj_Lua_MusicOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
