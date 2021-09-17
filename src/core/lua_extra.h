#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lua_State lua_State;

//Takes the string on top the lua stack and formats it with metadata
int Enj_Lua_Error(lua_State *L);

//Used to get members of a user-set Lua object
int Enj_Lua_GetDispatch(lua_State *L);
//Used to set members of a user-set Lua object
int Enj_Lua_SetDispatch(lua_State *L);


//Functions provided by act_lua runtime
int Enj_Lua_StartThread(lua_State *L);
//First argument is signal, second is function, rest are args + signal args
int Enj_Lua_ScheduleThread(lua_State *L);
//This function yields. use callk to properly resume
int Enj_Lua_WaitForSignal(lua_State *L);
//Signals threads waiting on a signal to resume
int Enj_Lua_SignalThreads(lua_State *L);

#ifdef __cplusplus
}
#endif
