#ifndef FA_PF_GETDEV_H
#define FA_PF_GETDEV_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_devinf(s8 drive, PF_DEV_INF* p_info);

#ifdef __cplusplus
}
#endif

#endif
