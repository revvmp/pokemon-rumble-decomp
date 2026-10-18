#ifndef FA_PF_TELLDIR_H
#define FA_PF_TELLDIR_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_telldir(PF_DIR* p_dir, u32* p_offset);

#ifdef __cplusplus
}
#endif

#endif
