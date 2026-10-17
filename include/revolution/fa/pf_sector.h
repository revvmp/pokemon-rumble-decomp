#ifndef FA_PF_SECTOR_H
#define FA_PF_SECTOR_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFSEC_ReadData(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 offset, u32 size, u32* p_success_size, u32 set_sig);
s32 PFSEC_WriteData(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 offset, u32 size, u32* p_success_size, u32 set_sig);

#ifdef __cplusplus
}
#endif

#endif
