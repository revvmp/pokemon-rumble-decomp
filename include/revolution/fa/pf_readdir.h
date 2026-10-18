#ifndef FA_PF_READDIR_H
#define FA_PF_READDIR_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_readdir(PF_DIR* p_dir, PF_DIRENT* p_entry);

#ifdef __cplusplus
}
#endif

#endif
