#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

void bindsound(lua_State *L, void *ctx,
    int (*onpreload)(lua_State *),
    int (*onunload)(lua_State *),
    int (*oncanunload)(lua_State *)
    );

#ifdef __cplusplus
}
#endif
