#ifndef FA_PF_W_FSFIRST_H
#define FA_PF_W_FSFIRST_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_w_fsfirst(const u16* path, u8 attributes, PF_DTA* p_dta);

#ifdef __cplusplus
}
#endif

#endif
