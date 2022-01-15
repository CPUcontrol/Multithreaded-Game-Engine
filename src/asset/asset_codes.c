#include "asset_codes.h"

static const char *strings[6] = {
    "",
    "out of memory",
    "no free assets in pool",
    "problem loading file",
    "external library error",
    "asset too large"
};

const char * getasseterrormsg(int code){
    if((code < 0) | (code >= 6)) return "unknown error";

    return strings[code];
}
