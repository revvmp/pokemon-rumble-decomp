#ifndef FA_PF_SETVOL_H
#define FA_PF_SETVOL_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_setvol(char drive, const char* name);

#ifdef __cplusplus
}
#endif

#endif
