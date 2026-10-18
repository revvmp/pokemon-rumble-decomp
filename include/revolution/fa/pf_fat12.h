#ifndef FA_PF_FAT12_H
#define FA_PF_FAT12_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFAT12_ReadFATEntry(PF_VOLUME* p_vol, u16 cluster, u32* p_value);
s32 PFFAT12_WriteFATEntry(PF_VOLUME* p_vol, u16 cluster, u16 value);
s32 PFFAT12_WriteFATEntryWithBuf(PF_VOLUME* p_vol, u16 cluster, u16 value, PF_CACHE_PAGE* p_page);
s32 PFFAT12_ReadFATEntryWithBuf(PF_VOLUME* p_vol, u16 cluster, u32* p_value, PF_CACHE_PAGE* p_page);

#ifdef __cplusplus
}
#endif

#endif
