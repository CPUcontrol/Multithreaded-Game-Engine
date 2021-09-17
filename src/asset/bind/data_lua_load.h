#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_DataOnPreload(lua_State *L);
int Enj_Lua_DataOnUnload(lua_State *L);
int Enj_Lua_DataOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
