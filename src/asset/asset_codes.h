#pragma once

#define ASSET_OK 0
#define ASSET_ERROR_MEMORY 1
#define ASSET_ERROR_POOL 2
#define ASSET_ERROR_FILE 3
#define ASSET_ERROR_LIBRARY 4
#define ASSET_ERROR_PARAM 5
#define ASSET_ERROR_BADARGS 6

#ifdef __cplusplus
extern "C" {
#endif

const char * getasseterrormsg(int code);

#ifdef __cplusplus
}
#endif
