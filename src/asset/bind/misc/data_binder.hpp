#pragma once

typedef struct lua_State lua_State;

typedef struct multi_dispatch multi_dispatch;
#include <string>

class data_binder{
private:
public:
    multi_dispatch &dispatch;
    std::string &basepath;

    //Call those signals using the main lua state, instead of the
    //lua state belonging to a non-main thread passed to the C function
    lua_State *Lmain;


    data_binder(
        multi_dispatch &md,
        std::string &base,
        lua_State *L
    );
    ~data_binder();

    data_binder(const data_binder &o) = delete;
    data_binder & operator=(const data_binder &o) = delete;

    data_binder(data_binder &&o) = delete;
    data_binder & operator=(data_binder &&o) = delete;


    operator bool(){return true;}
};
