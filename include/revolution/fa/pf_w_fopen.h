#ifndef FA_PF_W_FOPEN_H
#define FA_PF_W_FOPEN_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

PF_FILE* pf2_w_fopen(const u16* path, const u16* mode);

#ifdef __cplusplus
}
#endif

#endif
