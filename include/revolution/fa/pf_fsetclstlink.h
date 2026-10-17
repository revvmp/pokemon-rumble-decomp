#ifndef FA_PF_FSETCLSTLINK_H
#define FA_PF_FSETCLSTLINK_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pf2_fsetclstlink(PF_FILE* p_file, u32 mode, PF_CLUSTER_LINK_CONFIG* config);

#ifdef __cplusplus
}
#endif

#endif
