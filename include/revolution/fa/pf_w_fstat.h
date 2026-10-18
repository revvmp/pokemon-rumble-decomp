#ifndef FA_PF_W_FSTAT_H
#define FA_PF_W_FSTAT_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_w_fstat(const u16* path, PF_STAT* p_stat);

#ifdef __cplusplus
}
#endif

#endif
