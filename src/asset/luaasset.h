#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;
typedef struct luaasset{
    int (*onunload)(lua_State *);
    int (*oncanunload)(lua_State *);

    void *ctx;
    void *data;

    size_t refcount;
    //bit 0 - ready
    //bit 1 - error
    //bit 2 - load can pass
    unsigned char flag;
}luaasset;

void luafinishpreloadasset(lua_State *L, luaasset *la, int code);

#ifdef __cplusplus
}
#endif
