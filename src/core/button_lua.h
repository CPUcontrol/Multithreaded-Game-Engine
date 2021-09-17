#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;
typedef struct Enj_ButtonList Enj_ButtonList;

void bindbutton(lua_State *L, Enj_ButtonList *sl);

#ifdef __cplusplus
}
#endif
