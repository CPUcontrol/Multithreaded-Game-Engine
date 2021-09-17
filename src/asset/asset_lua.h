#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;
//Initializes asset functions in lua
void bindasset(lua_State *L);

#ifdef __cplusplus
}
#endif
