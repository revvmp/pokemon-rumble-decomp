#ifndef FA_PF_FOPEN_H
#define FA_PF_FOPEN_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

PF_FILE* pf2_fopen(const char* path, const char* mode);

#ifdef __cplusplus
}
#endif

#endif
