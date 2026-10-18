#ifndef FA_PF_SEEKDIR_H
#define FA_PF_SEEKDIR_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_seekdir(PF_DIR* p_dir, u32 offset);

#ifdef __cplusplus
}
#endif

#endif
