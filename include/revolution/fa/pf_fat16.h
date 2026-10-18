#ifndef FA_PF_FAT16_H
#define FA_PF_FAT16_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFAT16_ReadFATEntry(PF_VOLUME* p_vol, u32 cluster, u32* p_value);
s32 PFFAT16_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value);
s32 PFFAT16_WriteFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page);
s32 PFFAT16_ReadFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page);

#ifdef __cplusplus
}
#endif

#endif
