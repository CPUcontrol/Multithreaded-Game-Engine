#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;
typedef struct Enj_KeyboardList Enj_KeyboardList;

void bindkeyboard(lua_State *L, Enj_KeyboardList *sl);

#ifdef __cplusplus
}
#endif
