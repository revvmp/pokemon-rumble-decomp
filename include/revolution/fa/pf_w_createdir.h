#ifndef FA_PF_W_CREATEDIR_H
#define FA_PF_W_CREATEDIR_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_w_createdir(const u16* path, s32 mode, PF_DTA* p_dta);

#ifdef __cplusplus
}
#endif

#endif
