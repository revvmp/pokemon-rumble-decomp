#ifndef FA_PF_SETCLSTLINK_H
#define FA_PF_SETCLSTLINK_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_setclstlink(char drive, u32 mode, PF_CLUSTER_LINK_CONFIG* p_info);

#ifdef __cplusplus
}
#endif

#endif
