#ifndef FA_PF_FAT_H
#define FA_PF_FAT_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFAT_FreeChain(PF_FFD* p_ffd, u32 start_cluster, u32 chain_index, u32 size);
s32 PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector);
s32 PFFAT_GetContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector);
s32 PFFAT_TraceClustersChain(PF_FFD* p_ffd, u32 start_clst, u32 size, u32* p_target_clst, u32* p_next_clst);
s32 PFFAT_AllocateContinuousClusters(PF_FFD* p_ffd, u32 size, u32* p_start_cluster, u32* p_last_cluster, u32* p_size);
s32 PFFAT_WriteValueToSpecifiedCluster(PF_VOLUME* p_vol, u32 cluster, u32 value);
s32 PFFAT_ReadValueToSpecifiedCluster(PF_VOLUME* p_vol, u32 cluster, u32* value);
u32 PFFAT_GetValueOfEOC2(PF_VOLUME* p_vol);

#ifdef __cplusplus
}
#endif

#endif
