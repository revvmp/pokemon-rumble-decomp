#ifndef FA_PF_CREATEDIR_H
#define FA_PF_CREATEDIR_H

#include <revolution/types.h>
#include <revolution/fa/pf_dir.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_createdir(const char* path, s32 mode, PF_DTA* p_dta);

#ifdef __cplusplus
}
#endif

#endif
