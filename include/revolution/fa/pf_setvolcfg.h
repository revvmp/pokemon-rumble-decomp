#ifndef FA_PF_SETVOLCFG_H
#define FA_PF_SETVOLCFG_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_setvolcfg(char drive, PF_VOLUME_CONFIG* p_config);

#ifdef __cplusplus
}
#endif

#endif
