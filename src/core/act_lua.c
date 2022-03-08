#include "act_lua.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "act.h"
#include "lua_extra.h"

typedef struct delay_state{
    unsigned int duration;
    unsigned int ticks;
}delay_state;

static void delayonenter(void *s, void *p){
    delay_state *st = (delay_state *)s;

    st->ticks = 0;
}
static void delayonexit(void *s, void *p){

}
static int delayonupdate(void *s, void *p){
    delay_state *st = (delay_state *)s;

    int res = st->ticks < st->duration;
    st->ticks += res;
    return res;
}
//Adds already-encased act to list
static void appendactlist(lua_State *L, int list){
    int tmpidx = lua_gettop(L);
    lua_pushvalue(L, list);
    lua_pushvalue(L, tmpidx);
    if(lua_geti(L, tmpidx+1, 2)==LUA_TNIL){
        //List empty
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+1, 1);
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+1, 2);
    }
    else{
        //Tail present, list not empty
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+3, 2);
        lua_seti(L, tmpidx+2, 1);
        lua_seti(L, tmpidx+1, 2);
    }

    lua_settop(L, tmpidx-1);

}
//Node contents
//1: prev, 2: next, 3: act/thread, 4: argc for thread
//Encases a pushed act into node, adds it to list
static void addactlist(lua_State *L, int list){
    int tmpidx = lua_gettop(L);
    lua_pushvalue(L, list);

    lua_createtable(L, 4, 0);
    //Init argc to 0
    lua_pushinteger(L, 0);
    lua_seti(L, tmpidx+2, 4);

    lua_pushvalue(L, tmpidx);
    lua_seti(L, tmpidx+2, 3);
    if(lua_geti(L, tmpidx+1, 2)==LUA_TNIL){
        //List empty
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+1, 1);
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+1, 2);
    }
    else{
        //Tail present, list not empty
        lua_pushvalue(L, tmpidx+2);
        lua_seti(L, tmpidx+3, 2);
        lua_seti(L, tmpidx+2, 1);
        lua_seti(L, tmpidx+1, 2);
    }

    lua_settop(L, tmpidx-1);

}
//Removes node, must push node onto state, not just act/thread!
static void removeactlist(lua_State *L, int list){
    int tmpidx = lua_gettop(L);

    //update prev node
    if(lua_geti(L, tmpidx, 1) == LUA_TNIL){
        lua_geti(L, tmpidx, 2);
        lua_seti(L, list, 1);
    }
    else{
        lua_geti(L, tmpidx, 2);
        lua_seti(L, tmpidx+1, 2);
    }
    //update next node
    if(lua_geti(L, tmpidx, 2) == LUA_TNIL){
        lua_geti(L, tmpidx, 1);
        lua_seti(L, list, 2);
    }
    else{
        lua_geti(L, tmpidx, 1);
        lua_seti(L, tmpidx+2, 1);
    }
    lua_settop(L, tmpidx-1);
}

static void concatlists(lua_State *L, int liststart, int listend){
    int tmpidx = lua_gettop(L);
    //Head of end list
    if(lua_geti(L, listend, 1) == LUA_TNIL){
        //Do nothing with empty end list
    }
    //Tail of first list
    else if(lua_geti(L, liststart, 2) == LUA_TNIL){
        lua_geti(L, listend, 1);
        lua_seti(L, liststart, 1);
        lua_geti(L, listend, 2);
        lua_seti(L, liststart, 2);

        lua_pushnil(L);
        lua_seti(L, listend, 1);
        lua_pushnil(L);
        lua_seti(L, listend, 2);
    }
    else{
        lua_pushvalue(L, tmpidx+1);
        lua_seti(L, tmpidx+2, 2);
        lua_seti(L, tmpidx+1, 1);

        lua_geti(L, listend, 2);
        lua_seti(L, liststart, 2);
    }

    lua_settop(L, tmpidx);
}

static int luawaitforthread(lua_State *L){
    if(!lua_isthread(L, 1)) {
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    //If thread is yielded, then wait for it
    if (lua_status(lua_tothread(L, 1)) == LUA_YIELD){
        lua_pushvalue(L, 1);
        return lua_yield(L, 1);
    }
    else return 0;
}

int Enj_Lua_WaitForSignal(lua_State *L){
    int atype = lua_type(L, 1);
    if (
        (atype == LUA_TTHREAD)
    |   (atype == LUA_TNUMBER)
    |   (atype == LUA_TNIL)
    |   (atype == LUA_TNONE)) {

        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    lua_pushvalue(L, 1);
    return lua_yield(L, 1);
}


#define DELAY_MAXTIME (1<<15)

static int luawaitdelay(lua_State *L){
    int isint;
    lua_Integer delaytime = lua_tointegerx(L, 1, &isint);
    if(!isint) {
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }
    if(delaytime == 0) return 0;
    if((delaytime > DELAY_MAXTIME) | (delaytime < 0)) {
        lua_pushliteral(L, "delay time out of range");
        return Enj_Lua_Error(L);
    }

    int tmpidx = lua_gettop(L);

    lua_getfield(L, LUA_REGISTRYINDEX, "delaytick");
    lua_pushinteger(L, (lua_tointeger(L, tmpidx+1)+delaytime)%DELAY_MAXTIME);
    return lua_yield(L, 1);
}

static int wakewaiters(lua_State *L){
    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "waitlist");
    lua_pushvalue(L, 1);

    if(lua_gettable(L, tmpidx+1)!=LUA_TNIL){
        //Clear waitlist table at signalled entry
        lua_pushvalue(L, 1);
        lua_pushnil(L);
        lua_settable(L, tmpidx+1);
        //Get tail at ind 3
        lua_geti(L, tmpidx+2, 1);
        //Disconnect ring
        lua_pushnil(L);
        lua_seti(L, tmpidx+2, 1);
        lua_pushnil(L);
        lua_seti(L, tmpidx+3, 2);

        //Add rest of args to waiting threads if signal has more args
        if(tmpidx-1 > 0){
            for(
                lua_pushvalue(L, tmpidx+2);

                !lua_isnil(L, tmpidx+4);

                lua_geti(L, tmpidx+4, 2),
                lua_copy(L, tmpidx+7, tmpidx+4),
                lua_settop(L, tmpidx+4)
            )
            {
                lua_geti(L, tmpidx+4, 3);
                lua_State *nL = lua_tothread(L, tmpidx+5);
                //Duplicate and transfer
                for(int i = 2; i <= tmpidx; i++){
                    lua_pushvalue(L, i);
                }
                lua_xmove(L, nL, tmpidx-1);
                //Set arg count in node by adding
                lua_geti(L, tmpidx+4, 4);
                lua_Integer n = lua_tointeger(L, tmpidx+6);
                lua_pushinteger(L, n+tmpidx-1);
                lua_seti(L, tmpidx+4, 4);

            }
            lua_pop(L, 1);
        }

        //Concat actlist at ind 4
        lua_getfield(L, LUA_REGISTRYINDEX, "actlist");

        if(lua_geti(L, tmpidx+4, 2) == LUA_TNIL){
            lua_pushvalue(L, tmpidx+2);
            lua_seti(L, tmpidx+4, 1);
            lua_pushvalue(L, tmpidx+3);
            lua_seti(L, tmpidx+4, 2);
        }
        else{
            //Concat list
            lua_pushvalue(L, tmpidx+5);
            lua_seti(L, tmpidx+2, 1);
            lua_pushvalue(L, tmpidx+2);
            lua_seti(L, tmpidx+5, 2);

            lua_pushvalue(L, tmpidx+3);
            lua_seti(L, tmpidx+4, 2);
        }
    }

    return 0;
}

int Enj_Lua_SignalThreads(lua_State *L){
    int atype = lua_type(L, 1);
    if (
        (atype == LUA_TTHREAD)
    |   (atype == LUA_TNUMBER)
    |   (atype == LUA_TNIL)
    |   (atype == LUA_TNONE)) {

        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    return wakewaiters(L);
}

static int luakillthread(lua_State *L){
    if(!lua_isthread(L, 1)) {
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    //If thread is yielded, then wait for it
    lua_State *nL = lua_tothread(L, 1);
    if(nL == L){
        lua_pushthread(L);
        return lua_yield(L, 1);
    }

    if (lua_status(nL) == LUA_YIELD){
        lua_resetthread(nL);
    }
    return 0;
}

//C function that initializes a timer struct in Lua VM
static int actcreatetimer(lua_State *L){
    int isnum;
    lua_Integer x = lua_tointegerx(L, 1, &isnum);
    if(!(isnum & (x >= 0))){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }
    lua_settop(L, 0);
    delay_state *st = (delay_state *)
        lua_newuserdatauv(L, sizeof(delay_state), 0);

    st->duration = (unsigned int)x;

    lua_getfield(L, LUA_REGISTRYINDEX, "actproto");
    lua_getfield(L, 2, "delay");

    lua_setmetatable(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int actdispatchcont(lua_State *L, int status, lua_KContext ctx);
//dispatches any action to main thread and suspends current Lua thread
//Actually perform the act inside, yielding if unfinished
static int actdispatch(lua_State *L){

    void *state = lua_touserdata(L, 1);

    lua_getmetatable(L, 1);
    lua_getfield(L, LUA_REGISTRYINDEX, "waitlist");

    lua_pushvalue(L, 1);
    if(lua_gettable(L, 3) != LUA_TNIL){
        lua_pushliteral(L, "act already in use");
        return Enj_Lua_Error(L);
    }
    else{
        lua_pop(L, 1);
    }
    lua_getfield(L, 2, "act");

    Enj_Act *act = (Enj_Act *)lua_touserdata(L, 4);
    Enj_ActEnter(act, state);

    /*
    lua_getfield(L, LUA_REGISTRYINDEX, "actlist");
    lua_pushvalue(L, 1);
    addactlist(L, 5);

    lua_pushvalue(L, 1);

    return lua_yield(L, 1);
    */
    lua_toclose(L, 1);
    if(Enj_ActUpdate(act, state))
        return lua_yieldk(L, 0, 0, actdispatchcont);
    else{
        return 0;
    }
}

static int actdispatchcont(lua_State *L, int status, lua_KContext ctx){
    Enj_Act *act = (Enj_Act *)lua_touserdata(L, 4);
    void *state = lua_touserdata(L, 1);

    if(Enj_ActUpdate(act, state))
        return lua_yieldk(L, 0, 0, actdispatchcont);
    else{
        return 0;
    }
}

//C closure used to create and start an act
static int initanddispatch(lua_State *L){
    lua_CFunction actinit = lua_tocfunction(L, lua_upvalueindex(1));
    (*actinit)(L);
    lua_copy(L, lua_gettop(L), 1);
    lua_settop(L, 1);
    return actdispatch(L);
}

void updateluaact(lua_State *L){
    //Wake up delayed functions
    lua_pushcfunction(L, &wakewaiters);
    lua_getfield(L, LUA_REGISTRYINDEX, "delaytick");
    lua_Integer delaytick = lua_tointeger(L, 2);
    lua_call(L, 1, 0);


    lua_getfield(L, LUA_REGISTRYINDEX, "actlist");

    for(
        lua_geti(L, 1, 1);

        !lua_isnil(L, 2);

        lua_copy(L, 3, 2),
        lua_settop(L, 2)
    ){
        //Push next node to ind 3
        lua_geti(L, 2, 2);

        int entrytype = lua_geti(L, 2, 3);
        if(entrytype == LUA_TTHREAD){
            lua_State *nL = lua_tothread(L, 4);
            int luacode = lua_status(nL);

            switch(luacode){
            case LUA_ERRRUN:
            case LUA_ERRMEM:
            case LUA_ERRERR:
            case LUA_ERRSYNTAX:
            case LUA_ERRFILE:
                if(lua_getfield(nL, LUA_REGISTRYINDEX, "logfunction")
                    == LUA_TFUNCTION){

                    lua_pushvalue(nL, lua_gettop(nL) - 1);
                    lua_pcall(nL, 1, 0, 0);
                }
            case LUA_OK:
                //Thread was terminated abruptly
                lua_pushvalue(L, 2);
                removeactlist(L, 1);
                continue;
            case LUA_YIELD:
                break;
            }

            lua_geti(L, 2, 4);
            int narg = (int)lua_tointeger(L, 5);
            int nres;

            luacode = lua_resume(nL, NULL, narg, &nres);
            //set narg back to zero in node
            lua_pushinteger(L, 0);
            lua_seti(L, 2, 4);

            switch(luacode){
            //If error, do error handling, then fall through to OK procedure
            case LUA_ERRRUN:
            case LUA_ERRMEM:
            case LUA_ERRERR:
            case LUA_ERRSYNTAX:
            case LUA_ERRFILE:
                if(lua_getfield(nL, LUA_REGISTRYINDEX, "logfunction")
                    == LUA_TFUNCTION){

                    lua_pushvalue(nL, lua_gettop(nL) - 1);
                    lua_pcall(nL, 1, 0, 0);
                }

                lua_resetthread(nL);
            case LUA_OK:
                lua_pushcfunction(L, &wakewaiters);
                lua_pushvalue(L, 4);
                lua_call(L, 1, 0);

                lua_pushvalue(L, 2);
                removeactlist(L, 1);
                continue;
            case LUA_YIELD:
                //If 1 yielded value, means the thread is waiting on another
                //If zero, thread is performing an act inside
                if(nres == 1){
                    lua_xmove(nL, L, nres);
                    lua_pushvalue(L, 2);
                    removeactlist(L, 1);

                    //Waiting on self equivalent to exitting
                    if(lua_compare(L, 4, 6, LUA_OPEQ)){
                        lua_resetthread(nL);
                        lua_pushcfunction(L, &wakewaiters);
                        lua_pushvalue(L, 4);
                        lua_call(L, 1, 0);

                        continue;
                    }
                    //Put thread in wait list
                    lua_getfield(L, LUA_REGISTRYINDEX, "waitlist");
                    lua_pushvalue(L, 6);
                    if(lua_gettable(L, 7) == LUA_TNIL){
                        //Put first value in cycle
                        lua_pushvalue(L, 2);
                        lua_seti(L, 2, 1);
                        lua_pushvalue(L, 2);
                        lua_seti(L, 2, 2);
                        //Add to waitlist
                        lua_pushvalue(L, 6);
                        lua_pushvalue(L, 2);
                        lua_settable(L, 7);
                    }
                    else{
                        //Existing waitlist

                        //Get former tail at ind 9
                        lua_geti(L, 8, 1);
                        //Update new tail's everything
                        lua_pushvalue(L, 9);
                        lua_seti(L, 2, 1);
                        lua_pushvalue(L, 8);
                        lua_seti(L, 2, 2);
                        //Update old tail's next
                        lua_pushvalue(L, 2);
                        lua_seti(L, 9, 2);
                        //Update head's prev
                        lua_pushvalue(L, 2);
                        lua_seti(L, 8, 1);
                    }
                }


                continue;
            }
        }
        else{
            void *state = lua_touserdata(L, 4);
            lua_getmetatable(L, 4);
            lua_getfield(L, 5, "act");
            Enj_Act *act = (Enj_Act *)lua_touserdata(L, 6);

            if(!Enj_ActUpdate(act, state)){
                Enj_ActExit(act, state);

                //Contact waiting acts
                lua_pushvalue(L, 4);

                lua_pushcfunction(L, &wakewaiters);
                lua_pushvalue(L, 4);
                lua_call(L, 1, 0);

                lua_pushvalue(L, 2);
                removeactlist(L, 1);
                continue;
            }
        }
    }

    //Increment delay tick
    lua_pushinteger(L, (delaytick+1)%DELAY_MAXTIME);
    lua_setfield(L, LUA_REGISTRYINDEX, "delaytick");

    lua_settop(L, 0);
}
//3 functions that wrap a thread

static int initcfunc(lua_State *L);
static int kcontinit(lua_State *L, int status, lua_KContext ctx);
static int kcontinit2(lua_State *L, int status, lua_KContext ctx);

static int initcfunc(lua_State *L){
    return lua_yieldk(L, 0, 0, kcontinit);
}
static int kcontinit(lua_State *L, int status, lua_KContext ctx){
    lua_callk(L, lua_gettop(L)-1, 0, 0, kcontinit2);
    return kcontinit2(L, LUA_OK, ctx);
}
static int kcontinit2(lua_State *L, int status, lua_KContext ctx){
    return 0;
}

//put created thread into list of threads and acts
void threadstart(lua_State *L, const char *script){
    lua_getfield(L, LUA_REGISTRYINDEX, "actlist");
    lua_State *nL = lua_newthread(L);
    //Add timer+thread to actlist
    addactlist(L, 1);

    //Put script inside new thread
    lua_pushcfunction(nL, initcfunc);
    lua_getglobal(nL, script);

    int nres;
    lua_resume(nL, NULL, 1, &nres);

    lua_settop(L, 0);
}


int Enj_Lua_StartThread(lua_State *L){
    if(!lua_isfunction(L, 1)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }


    int tmpidx = lua_gettop(L);
    lua_getfield(L, LUA_REGISTRYINDEX, "actlist");
    lua_State *nL = lua_newthread(L);
    //Add timer+thread to actlist
    addactlist(L, tmpidx+1);
    lua_pop(L, 1);

    lua_pushcfunction(nL, initcfunc);
    lua_xmove(L, nL, tmpidx);

    lua_pushthread(nL);
    lua_xmove(nL, L, 1);

    int nres;
    lua_resume(nL, L, tmpidx, &nres);

    return 1;
}

int Enj_Lua_ScheduleThread(lua_State *L){
    int atype = lua_type(L, 1);
    if (
        (atype == LUA_TTHREAD)
    |   (atype == LUA_TNUMBER)
    |   (atype == LUA_TNIL)
    |   (atype == LUA_TNONE)) {

        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }
    if(!lua_isfunction(L, 2)){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }


    int tmpidx = lua_gettop(L);

    //Add timer+thread to actlist
    lua_createtable(L, 4, 0);
    lua_State *nL = lua_newthread(L);
    lua_seti(L, tmpidx+1, 3);
    //Init argc to num args
    lua_pushinteger(L, tmpidx-1);
    lua_seti(L, tmpidx+1, 4);


    //Put thread in wait list
    lua_getfield(L, LUA_REGISTRYINDEX, "waitlist");
    lua_pushvalue(L, 1);
    if(lua_gettable(L, tmpidx+2) == LUA_TNIL){
        //Put first value in cycle
        lua_pushvalue(L, tmpidx+1);
        lua_seti(L, tmpidx+1, 1);
        lua_pushvalue(L, tmpidx+1);
        lua_seti(L, tmpidx+1, 2);
        //Add to waitlist
        lua_pushvalue(L, 1);
        lua_pushvalue(L, tmpidx+1);
        lua_settable(L, tmpidx+2);
    }
    else{
        //Existing waitlist

        //Get former tail at ind 4
        lua_geti(L, tmpidx+3, 1);
        //Update new tail's everything
        lua_pushvalue(L, tmpidx+4);
        lua_seti(L, tmpidx+1, 1);
        lua_pushvalue(L, tmpidx+3);
        lua_seti(L, tmpidx+1, 2);
        //Update old tail's next
        lua_pushvalue(L, tmpidx+1);
        lua_seti(L, tmpidx+4, 2);
        //Update head's prev
        lua_pushvalue(L, tmpidx+1);
        lua_seti(L, tmpidx+3, 1);
    }
    lua_settop(L, tmpidx);


    lua_pushcfunction(nL, initcfunc);
    lua_xmove(L, nL, tmpidx-1);

    lua_pushthread(nL);
    lua_xmove(nL, L, 1);

    int nres;
    lua_resume(nL, L, tmpidx-1, &nres);

    return 1;
}

static int actnew(lua_State *L){
    int tmpidx = lua_gettop(L);
    if(!tmpidx){
        lua_pushliteral(L, "bad arguments");
        return Enj_Lua_Error(L);
    }

    lua_getfield(L, LUA_REGISTRYINDEX, "actproto");
    lua_pushvalue(L, 1);
    if(lua_gettable(L, tmpidx+1) == LUA_TNIL){
        lua_pushliteral(L, "unable to find act");
        return Enj_Lua_Error(L);
    }

    lua_getfield(L, tmpidx+2, "new");
    lua_copy(L, tmpidx+3, 1);

    lua_settop(L, tmpidx);
    lua_call(L, tmpidx-1, 1);

    return 1;
}

static int closeact(lua_State *L){

    lua_getmetatable(L, 1);
    lua_getfield(L, 3, "act");

    void *state = lua_touserdata(L, 1);
    Enj_Act *act = (Enj_Act *)lua_touserdata(L, 4);

    Enj_ActExit(act, state);
    return 0;
}

static int thisthread(lua_State *L){
    lua_pushthread(L);
    return 1;
}

void luasignaluserdata(lua_State *L, void *userdata, int code){
    lua_pushcfunction(L, &Enj_Lua_SignalThreads);
    lua_pushlightuserdata(L, userdata);
    lua_pushinteger(L, (lua_Integer)code);
    lua_call(L, 2, 0);
}

void initbuiltins(lua_State *L){
    lua_createtable(L, 0, 8);

    /* TODO - RETHINK ACTS IN LUA ENGINE
    lua_createtable(L, 0, 5);
    lua_pushliteral(L, "delay");
    lua_setfield(L, 2 , "__metatable");
    Enj_Act *act = (Enj_Act *)lua_newuserdatauv(L, sizeof(Enj_Act), 0);

    act->onenter = delayonenter;
    act->onexit = delayonexit;
    act->onupdate = delayonupdate;
    act->proc = NULL;
    lua_setfield(L, 2, "act");

    lua_pushcfunction(L, &actdispatch);
    lua_setfield(L, 2, "__call");

    lua_pushcfunction(L, &closeact);
    lua_setfield(L, 2, "__close");

    lua_pushcfunction(L, &actcreatetimer);
    lua_setfield(L, 2, "new");

    lua_setfield(L, 1, "delay");

    //bind delay name to Lua
    lua_pushcfunction(L, &actcreatetimer);
    lua_pushcclosure(L, &initanddispatch, 1);
    lua_setglobal(L, "delay");
    */

    lua_setfield(L, LUA_REGISTRYINDEX, "actproto");
    //init actlist
    lua_createtable(L, 2, 0);
    lua_setfield(L, LUA_REGISTRYINDEX, "actlist");

    //init gameproto
    lua_createtable(L, 0, 8);
    lua_setfield(L, LUA_REGISTRYINDEX, "gameproto");
    lua_register(L, "run", Enj_Lua_StartThread);
    lua_register(L, "schedule", Enj_Lua_ScheduleThread);
    lua_register(L, "wait", luawaitforthread);
    lua_register(L, "kill", luakillthread);
    lua_register(L, "this_thread", thisthread);
    //lua_register(L, "make_act", actnew);

    lua_register(L, "wait_signal", Enj_Lua_WaitForSignal);
    lua_register(L, "signal", Enj_Lua_SignalThreads);

    lua_pushinteger(L, 0);
    lua_setfield(L, LUA_REGISTRYINDEX, "delaytick");
    lua_register(L, "delay", luawaitdelay);

    lua_createtable(L, 0, 8);
    lua_setfield(L, LUA_REGISTRYINDEX, "waitlist");

    lua_createtable(L, 0, 8);
    lua_setfield(L, LUA_REGISTRYINDEX, "strongreftable");
}
