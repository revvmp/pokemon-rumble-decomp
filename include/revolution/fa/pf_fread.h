#ifndef FA_PF_FREAD_H
#define FA_PF_FREAD_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 pf2_fread(void* buffer, u32 size, u32 count, PF_FILE* p_file);

#ifdef __cplusplus
}
#endif

#endif
