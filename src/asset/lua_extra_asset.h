#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;


//Used with Lua objects that are assets
int Enj_Lua_GetAssetDispatch(lua_State *L);
int Enj_Lua_SetAssetDispatch(lua_State *L);

int Enj_Lua_CreateAsset(lua_State *L);

#ifdef __cplusplus
}
#endif
