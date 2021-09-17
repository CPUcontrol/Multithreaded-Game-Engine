#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;


void updateluaact(lua_State *L);

//signals waiting threads that are waiting for a userdata signal
void luasignaluserdata(lua_State *L, void *userdata, int code);

//put created thread into list of threads and acts
void threadstart(lua_State *L, const char *script);

void initbuiltins(lua_State *L);

#ifdef __cplusplus
}
#endif
