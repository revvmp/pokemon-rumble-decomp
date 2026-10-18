#ifndef FA_PF_FSEEK_H
#define FA_PF_FSEEK_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_fseek(PF_FILE* p_file, s32 offset, int origin);

#ifdef __cplusplus
}
#endif

#endif
