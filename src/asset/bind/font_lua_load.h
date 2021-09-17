#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_FontOnPreload(lua_State *L);
int Enj_Lua_FontOnUnload(lua_State *L);
int Enj_Lua_FontOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
