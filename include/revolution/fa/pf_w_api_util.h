#ifndef FA_PF_W_API_UTIL_H
#define FA_PF_W_API_UTIL_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 PFWAPI_ParseOpenModeString(const u16* mode_str);

#ifdef __cplusplus
}
#endif

#endif
