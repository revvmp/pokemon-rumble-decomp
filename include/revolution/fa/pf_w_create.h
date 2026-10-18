#ifndef FA_PF_W_CREATE_H
#define FA_PF_W_CREATE_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

PF_FILE* pf2_w_create(const u16* path, int mode);

#ifdef __cplusplus
}
#endif

#endif
