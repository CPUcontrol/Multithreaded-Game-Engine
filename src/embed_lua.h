#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

void embed_lua(lua_State *L, const char *basepath, const char *prefpath);

#ifdef __cplusplus
}
#endif
