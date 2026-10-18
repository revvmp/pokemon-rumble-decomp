#ifndef FA_PF_W_CDELETE_H
#define FA_PF_W_CDELETE_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 pf2_w_cdelete(const u16* path, u32 offset, u32 num_clusters);

#ifdef __cplusplus
}
#endif

#endif
