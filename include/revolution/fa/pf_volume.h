#ifndef FA_PF_VOLUME_H
#define FA_PF_VOLUME_H

#include <revolution/fa/pdm.h>
#include <revolution/fa/pf_cache.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_VOLUME {
    PDM_BPB bpb;
    u8 files[0x15EC];
    PF_SECTOR_CACHE cache;
    s8 label[12];
    u8 state[0x2904];
} PF_VOLUME;

s32 PFDRV_lread(PF_VOLUME* p_vol, u8* buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFDRV_lwrite(PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sector, u32* p_num_success);

#ifdef __cplusplus
}
#endif

#endif
