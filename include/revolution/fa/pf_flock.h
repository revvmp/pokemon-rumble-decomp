#ifndef FA_PF_FLOCK_H
#define FA_PF_FLOCK_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_flock(PF_FILE* p_file, u32 mode);

#ifdef __cplusplus
}
#endif

#endif
