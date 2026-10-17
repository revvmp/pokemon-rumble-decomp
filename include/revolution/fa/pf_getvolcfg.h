#ifndef FA_PF_GETVOLCFG_H
#define FA_PF_GETVOLCFG_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_getvolcfg(s8 drive, PF_VOLUME_CONFIG* config);

#ifdef __cplusplus
}
#endif

#endif
