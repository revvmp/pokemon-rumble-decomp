#ifndef FA_PF_FAT_H
#define FA_PF_FAT_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFAT_ReadFATSector(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 cluster);
s32 PFFAT_SearchForNumFreeClusters(PF_VOLUME* p_vol, u32 start_cluster, u32 end_cluster, u32 num_cluster, u32* p_start_free_cluster,
                                   u32* p_last_free_cluster);
s32 PFFAT_ClearCluster(PF_VOLUME* p_vol, u32 cluster);
s32 PFFAT_FindClusterLink(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* p_is_found);
s32 PFFAT_FindClusterLinkPage(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* p_is_found, PF_CACHE_PAGE* p_page);
s32 PFFAT_ReadCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_check_link);
s32 PFFAT_ReadClusterPage(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_check_link, PF_CACHE_PAGE* p_page);
s32 PFFAT_WriteCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_link);
s32 PFFAT_WriteClusterPage(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_link, PF_CACHE_PAGE* p_page);
s32 PFFAT_DoAllocateChain(PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated, u32 is_clear);
s32 PFFAT_GetClusterInChain(PF_FFD* p_ffd, u32 chain_index, u32 mode, u32 num_cluster, u32* p_locate_start, u32* p_locate_end);
s32 PFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector);
s32 PFFAT_GetClusterAllocatedInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_clusters);
s32 PFFAT_GetClusterSpecified(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster);
s32 PFFAT_GetClusterAllocated(PF_FFD* p_ffd, u32 chain_index, u32 num_cluster, u32* p_cluster);
s32 PFFAT_GetSector(PF_FFD* p_ffd, u32 file_sector_index, u32 mode, u32 size, u32* p_sector);
s32 PFFAT_InitFATRegion(PF_VOLUME* p_vol);
s32 PFFAT_RefreshFSINFO(PF_VOLUME* p_vol);
s32 PFFAT_CountFreeClusters(PF_VOLUME* p_vol, u32* p_count);
s32 PFFAT_FreeChain(PF_FFD* p_ffd, u32 start_cluster, u32 chain_index, u32 size);
s32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 file_sector_index, u32 may_allocate, u32* p_sector);
s32 PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector);
s32 PFFAT_GetContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector);
s32 PFFAT_TraceClustersChain(PF_FFD* p_ffd, u32 start_clst, u32 size, u32* p_target_clst, u32* p_next_clst);
s32 PFFAT_AllocateContinuousClusters(PF_FFD* p_ffd, u32 size, u32* p_start_cluster, u32* p_last_cluster, u32* p_size);
s32 PFFAT_WriteValueToSpecifiedCluster(PF_VOLUME* p_vol, u32 cluster, u32 value);
void PFFAT_InitHint(PF_FAT_HINT* p_hint);
s32 PFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster);
s32 PFFAT_FinalizeFFD(PF_FFD* p_ffd);
s32 PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster);
s32 PFFAT_ReadValueToSpecifiedCluster(PF_VOLUME* p_vol, u32 cluster, u32* value);
u32 PFFAT_GetValueOfEOC2(PF_VOLUME* p_vol);
s32 PFFAT_MakeRootDir(PF_VOLUME* p_vol);
s32 PFFAT_GetBeforeChain(PF_VOLUME* p_vol, u32 start_cluster, u32 cluster, u32* p_before_cluster);

#ifdef __cplusplus
}
#endif

#endif
