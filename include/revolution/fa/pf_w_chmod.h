#ifndef FA_PF_W_CHMOD_H
#define FA_PF_W_CHMOD_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_w_chmod(const u16* path, int attr);

#ifdef __cplusplus
}
#endif

#endif
