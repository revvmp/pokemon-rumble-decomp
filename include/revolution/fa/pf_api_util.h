#ifndef FA_PF_API_UTIL_H
#define FA_PF_API_UTIL_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 PFAPI_ParseOpenModeString(const char* mode_str);
int PFAPI_convertError(int err);
int PFAPI_convertDriverError(int err);
int PFAPI_convertReturnValue(int err);
int PFAPI_convertReturnValue4feof(int err);
void* PFAPI_convertReturnValue2NULL(int err, void* p_stream);
int PFAPI_convertReturnValue4unmount(int err);

#ifdef __cplusplus
}
#endif

#endif
