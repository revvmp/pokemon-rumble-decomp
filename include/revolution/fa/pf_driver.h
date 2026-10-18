#ifndef FA_PF_DRIVER_H
#define FA_PF_DRIVER_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

void PFDRV_MediaEject(PDM_PARTITION* p_part);
void PFDRV_MediaInsert(PDM_PARTITION* p_part);

#ifdef __cplusplus
}
#endif

#endif
