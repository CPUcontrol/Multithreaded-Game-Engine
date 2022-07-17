#include "data_binder.hpp"


data_binder::data_binder(
        multi_dispatch &md,
        const char *base,
        lua_State *L
): dispatch(md), basepath(base), Lmain(L)
{

}

data_binder::~data_binder(){

}
