#ifndef FA_PF_FSTAT_H
#define FA_PF_FSTAT_H

#include <revolution/fa/pf_dir.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_fstat(const char* path, PF_STAT* p_stat);

#ifdef __cplusplus
}
#endif

#endif
