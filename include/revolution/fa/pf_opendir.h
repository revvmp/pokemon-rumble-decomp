#ifndef FA_PF_OPENDIR_H
#define FA_PF_OPENDIR_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

PF_DIR* pf2_opendir(const char* path);

#ifdef __cplusplus
}
#endif

#endif
