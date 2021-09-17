#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

int Enj_Lua_GlyphOnPreload(lua_State *L);
int Enj_Lua_GlyphOnUnload(lua_State *L);
int Enj_Lua_GlyphOnCanUnload(lua_State *L);

#ifdef __cplusplus
}
#endif
