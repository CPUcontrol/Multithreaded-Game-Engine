#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_GetRenderNodePriority(lua_State *L);
int Enj_Lua_SetRenderNodePriority(lua_State *L);

int Enj_Lua_GetRenderNodeActive(lua_State *L);
int Enj_Lua_SetRenderNodeActive(lua_State *L);

#ifdef __cplusplus
}
#endif
