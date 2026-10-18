#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_fat12.h>
#include <revolution/fa/pf_fat16.h>
#include <revolution/fa/pf_fat32.h>
#include <revolution/fa/pf_sector.h>

static const struct {
    u32 bad;
    u32 eoc1;
    u32 eoc2;
    u32 fat0_mask;
    u32 fat1;
} fat_special_values[3] = {
    {0xFF7, 0xFF8, 0xFFF, 0xF00, 0xFFF},
    {0xFFF7, 0xFFF8, 0xFFFF, 0xFF00, 0xFFFF},
    {0xFFFFFF7, 0xFFFFFF8, 0xFFFFFFF, 0xFFFFF00, 0xFFFFFFF},
};

static inline s32 PFFAT_ReadFATEntry(PF_VOLUME* p_vol, u32 cluster, u32* p_value)
{
    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_12:
        return PFFAT12_ReadFATEntry(p_vol, cluster, p_value);
    case PDM_FAT_16:
        return PFFAT16_ReadFATEntry(p_vol, cluster, p_value);
    case PDM_FAT_32:
        return PFFAT32_ReadFATEntry(p_vol, cluster, p_value);
    default:
        *p_value = 0xFFFFFFFF;
        return 15;
    }
}

static inline s32 PFFAT_ReadFATEntryPage(PF_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page)
{
    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_12:
        return PFFAT12_ReadFATEntryWithBuf(p_vol, cluster, p_value, p_page);
    case PDM_FAT_16:
        return PFFAT16_ReadFATEntryWithBuf(p_vol, cluster, p_value, p_page);
    case PDM_FAT_32:
        return PFFAT32_ReadFATEntryWithBuf(p_vol, cluster, p_value, p_page);
    default:
        return 15;
    }
}

static inline s32 PFFAT_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value)
{
    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_12:
        if (value > 0xFFF) {
            return 16;
        }
        return PFFAT12_WriteFATEntry(p_vol, cluster, value);
    case PDM_FAT_16:
        if (value > 0xFFFF) {
            return 16;
        }
        return PFFAT16_WriteFATEntry(p_vol, cluster, value);
    case PDM_FAT_32:
        if (value > 0xFFFFFFF) {
            return 16;
        }
        return PFFAT32_WriteFATEntry(p_vol, cluster, value);
    default:
        return 15;
    }
}

static inline s32 PFFAT_WriteFATEntryPage(PF_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page)
{
    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_12:
        if (value > 0xFFF) {
            return 16;
        }
        return PFFAT12_WriteFATEntryWithBuf(p_vol, cluster, value, p_page);
    case PDM_FAT_16:
        if (value > 0xFFFF) {
            return 16;
        }
        return PFFAT16_WriteFATEntryWithBuf(p_vol, cluster, value, p_page);
    case PDM_FAT_32:
        if (value > 0xFFFFFFF) {
            return 16;
        }
        return PFFAT32_WriteFATEntryWithBuf(p_vol, cluster, value, p_page);
    default:
        return 15;
    }
}

s32 PFFAT_ReadFATSector(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 cluster)
{
    s32 err;
    u32 offset;
    u32 current_fat;
    s32 result;

    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_12:
        offset = (u16)(cluster + (cluster >> 1));
        break;
    case PDM_FAT_16:
        offset = cluster << 1;
        break;
    case PDM_FAT_32:
        offset = cluster << 2;
        break;
    default:
        return 15;
    }
    p_page->sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    current_fat = (p_vol->bpb.ext_flags & 0x80) ? (p_vol->bpb.ext_flags & 7) : 1;
    do {
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, p_page->sector, 0, p_vol->bpb.bytes_per_sector);
        if (err == 4096 && p_vol->p_callback != NULL) {
            result = p_vol->p_callback(p_vol->last_driver_error);
            if (result == 0) {
                continue;
            }
            if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                current_fat++;
                p_page->sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            return err;
        }
    } while (err != 0);
    return err;
}

s32 PFFAT_SearchForNumFreeClusters(PF_VOLUME* p_vol, u32 start_cluster, u32 end_cluster, u32 num_cluster, u32* p_start_free_cluster,
                                   u32* p_last_free_cluster)
{
    u32 upper_bound_cluster;
    u32 fat_entry;
    u32 success_num;
    u32 save_start_cluster;
    u32 save_success_num;
    s32 result;
    u32 search_flg;
    u32 temp_start_cluster;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;

    search_flg = 0;
    *p_start_free_cluster = 0xFFFFFFFF;
    *p_last_free_cluster = 0xFFFFFFFF;
    success_num = 0;
    save_start_cluster = 0xFFFFFFFF;
    save_success_num = 0;
    upper_bound_cluster = p_vol->bpb.num_clusters + 2;
    if (start_cluster < 2 || start_cluster >= upper_bound_cluster) {
        start_cluster = 2;
    }
    if (end_cluster < 2 || end_cluster >= upper_bound_cluster) {
        end_cluster = upper_bound_cluster - 1;
    }
    temp_start_cluster = start_cluster;

    result = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (result != 0) {
        return result;
    }
    result = PFFAT_ReadFATSector(p_vol, p_page, start_cluster);
    if (result != 0) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        return result;
    }
    p_page->option = 0;

    while (start_cluster <= end_cluster) {
        if (upper_bound_cluster <= start_cluster) {
            start_cluster = 2;
        }
        result = PFFAT_ReadFATEntryPage(p_vol, start_cluster, &fat_entry, p_page);
        if (result != 0) {
            return result;
        }
        if (fat_entry == 0) {
            if (*p_start_free_cluster == 0xFFFFFFFF) {
                *p_start_free_cluster = start_cluster;
            }
            if (++success_num >= num_cluster) {
                *p_last_free_cluster = start_cluster;
                PFCACHE_FreeDataPage(p_vol, p_page);
                return 0;
            }
        } else {
            if (save_success_num < success_num) {
                save_start_cluster = *p_start_free_cluster;
                save_success_num = success_num;
            }
            *p_start_free_cluster = 0xFFFFFFFF;
            success_num = 0;
        }
        start_cluster++;
        if (!search_flg && !success_num && start_cluster > 2 && start_cluster == end_cluster) {
            end_cluster = temp_start_cluster;
            start_cluster = 2;
            search_flg = 1;
        }
    }
    if (save_success_num < success_num) {
        save_start_cluster = *p_start_free_cluster;
        save_success_num = success_num;
    }
    if (save_success_num != 0) {
        *p_start_free_cluster = save_start_cluster;
        *p_last_free_cluster = *p_start_free_cluster + save_success_num - 1;
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}

s32 PFFAT_ClearCluster(PF_VOLUME* p_vol, u32 cluster)
{
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 success_size;
    u32 sector;
    u32 first_sector;
    u32 i;
    s32 err;

    err = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (err != 0) {
        return err;
    }
    first_sector = p_vol->bpb.first_data_sector + ((cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
    for (i = 0; i < p_vol->bpb.sectors_per_cluster; i++) {
        sector = first_sector + i;
        err = PFSEC_ReadData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1);
        if (err != 0 || success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
        pf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        err = PFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1);
        if (err != 0 || success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}

static inline s32 PFFAT_UpdateClusterLink(PF_FFD* p_ffd, u32 cluster, u32 chain_index)
{
    if (p_ffd->cluster_link.max_count > p_ffd->cluster_link.position && p_ffd->cluster_link.max_count != 0) {
        if (chain_index == p_ffd->cluster_link.position * (p_ffd->cluster_link.interval + 1)) {
            p_ffd->cluster_link.interval_offset = 0;
            p_ffd->cluster_link.buffer[p_ffd->cluster_link.position] = cluster;
            p_ffd->cluster_link.position++;
        } else {
            p_ffd->cluster_link.interval_offset++;
        }
        p_ffd->cluster_link.save_index = chain_index;
    }
    return 0;
}

s32 PFFAT_FindClusterLink(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* p_is_found)
{
    u32 current_cluster;
    u32 next_cluster;
    u32 position;
    u32 offset;
    s32 result;

    next_cluster = 0xFFFFFFFF;
    *p_is_found = 0;
    if (p_ffd->cluster_link.position == 0) {
        return 0;
    }
    if (p_ffd->cluster_link.save_index >= chain_index) {
        position = chain_index / (p_ffd->cluster_link.interval + 1);
        offset = chain_index % (p_ffd->cluster_link.interval + 1);
        if (offset == 0) {
            *p_cluster = p_ffd->cluster_link.buffer[position];
            *p_is_found = 1;
        } else {
            current_cluster = p_ffd->cluster_link.buffer[position];
            for (; offset != 0; offset--) {
                result = PFFAT_ReadFATEntry(p_ffd->p_vol, current_cluster, &next_cluster);
                if (result != 0) {
                    return result;
                }
                if (next_cluster == 0) {
                    return 13;
                }
                current_cluster = next_cluster;
            }
            if (next_cluster == 0) {
                return 13;
            }
            if (next_cluster == fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
                return 0;
            }
            *p_cluster = next_cluster;
            *p_is_found = 1;
        }
    } else {
        return 0;
    }
    return 0;
}

s32 PFFAT_FindClusterLinkPage(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* p_is_found, PF_CACHE_PAGE* p_page)
{
    u32 current_cluster;
    u32 next_cluster;
    u32 position;
    u32 offset;
    s32 result;

    next_cluster = 0xFFFFFFFF;
    *p_is_found = 0;
    if (p_ffd->cluster_link.position == 0) {
        return 0;
    }
    if (p_ffd->cluster_link.save_index >= chain_index) {
        position = chain_index / (p_ffd->cluster_link.interval + 1);
        offset = chain_index % (p_ffd->cluster_link.interval + 1);
        if (offset == 0) {
            *p_cluster = p_ffd->cluster_link.buffer[position];
            *p_is_found = 1;
        } else {
            current_cluster = p_ffd->cluster_link.buffer[position];
            for (; offset != 0; offset--) {
                result = PFFAT_ReadFATEntryPage(p_ffd->p_vol, current_cluster, &next_cluster, p_page);
                if (result != 0) {
                    return result;
                }
                if (next_cluster == 0) {
                    return 13;
                }
                current_cluster = next_cluster;
            }
            if (next_cluster == 0) {
                return 13;
            }
            if (next_cluster == fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
                return 0;
            }
            *p_cluster = next_cluster;
            *p_is_found = 1;
        }
    } else {
        return 0;
    }
    return 0;
}

s32 PFFAT_ReadCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_check_link)
{
    s32 err;

    *p_next_cluster = 0xFFFFFFFF;
    if (p_ffd->cluster_link.buffer != NULL && *p_check_link == 1) {
        err = PFFAT_FindClusterLink(p_ffd, chain_index, p_next_cluster, p_check_link);
        if (err != 0) {
            return err;
        }
    }
    if (p_ffd->cluster_link.buffer == NULL || *p_check_link == 0) {
        err = PFFAT_ReadFATEntry(p_ffd->p_vol, cluster, p_next_cluster);
        if (err != 0) {
            return err;
        }
        if (*p_next_cluster == 0) {
            return 13;
        }
        if (*p_next_cluster == 0xFFFFFFFF) {
            return 6;
        }
        if (p_ffd->cluster_link.buffer != NULL && *p_next_cluster != fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
            err = PFFAT_UpdateClusterLink(p_ffd, *p_next_cluster, chain_index);
            if (err != 0) {
                return err;
            }
        }
    }
    return 0;
}

s32 PFFAT_ReadClusterPage(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_check_link, PF_CACHE_PAGE* p_page)
{
    s32 err;

    *p_next_cluster = 0xFFFFFFFF;
    if (p_ffd->cluster_link.buffer != NULL && *p_check_link == 1) {
        err = PFFAT_FindClusterLinkPage(p_ffd, chain_index, p_next_cluster, p_check_link, p_page);
        if (err != 0) {
            return err;
        }
    }
    if (p_ffd->cluster_link.buffer == NULL || *p_check_link == 0) {
        err = PFFAT_ReadFATEntryPage(p_ffd->p_vol, cluster, p_next_cluster, p_page);
        if (err != 0) {
            return err;
        }
        if (*p_next_cluster == 0) {
            return 13;
        }
        if (*p_next_cluster == 0xFFFFFFFF) {
            return 6;
        }
        if (p_ffd->cluster_link.buffer != NULL && *p_next_cluster != fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
            err = PFFAT_UpdateClusterLink(p_ffd, *p_next_cluster, chain_index);
            if (err != 0) {
                return err;
            }
        }
    }
    return 0;
}

static inline s32 PFFAT_ClearClusterLink(PF_FFD* p_ffd, u32 chain_index)
{
    u32 position;

    position = chain_index / (p_ffd->cluster_link.interval + 1);
    if (p_ffd->cluster_link.max_count >= position) {
        if (chain_index % (p_ffd->cluster_link.interval + 1) == 0) {
            p_ffd->cluster_link.buffer[position] = 0;
        }
        if (p_ffd->cluster_link.save_index >= chain_index) {
            p_ffd->cluster_link.position = (chain_index - 1) / (p_ffd->cluster_link.interval + 1);
            p_ffd->cluster_link.interval_offset = (chain_index - 1) % (p_ffd->cluster_link.interval + 1);
            p_ffd->cluster_link.save_index = chain_index - 1;
        }
    }
    return 0;
}

s32 PFFAT_WriteCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_link)
{
    s32 err;
    u32 access_cluster;

    err = PFFAT_WriteFATEntry(p_ffd->p_vol, cluster, next_cluster);
    if (err != 0) {
        return err;
    }
    if (p_ffd->cluster_link.buffer != NULL && use_link == 1) {
        if (next_cluster == 0) {
            PFFAT_ClearClusterLink(p_ffd, chain_index);
        } else if (cluster != fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
            err = PFFAT_FindClusterLink(p_ffd, chain_index, &access_cluster, &use_link);
            if (err != 0) {
                return err;
            }
            if (use_link == 0) {
                err = PFFAT_UpdateClusterLink(p_ffd, cluster, chain_index);
                if (err != 0) {
                    return err;
                }
            }
        }
    }
    return 0;
}

s32 PFFAT_WriteClusterPage(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_link, PF_CACHE_PAGE* p_page)
{
    s32 err;
    u32 access_cluster;

    err = PFFAT_WriteFATEntryPage(p_ffd->p_vol, cluster, next_cluster, p_page);
    if (err != 0) {
        return err;
    }
    if (p_ffd->cluster_link.buffer != NULL && use_link == 1) {
        if (next_cluster == 0) {
            PFFAT_ClearClusterLink(p_ffd, chain_index);
        } else if (cluster != fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2) {
            err = PFFAT_FindClusterLinkPage(p_ffd, chain_index, &access_cluster, &use_link, p_page);
            if (err != 0) {
                return err;
            }
            if (use_link == 0) {
                err = PFFAT_UpdateClusterLink(p_ffd, cluster, chain_index);
                if (err != 0) {
                    return err;
                }
            }
        }
    }
    return 0;
}

static inline s32 PFFAT_UpdateFATEntry(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page)
{
    u16 i;
    s32 err;
    s32 err2;
    u32 sector;

    err = 0;
    sector = p_page->sector;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        err2 = PFSEC_WriteFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
        if (err2 != 0 && err == 0) {
            err = err2;
        }
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return err;
}

s32 PFFAT_DoAllocateChain(PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated, u32 is_clear)
{
    PF_VOLUME* p_vol;
    u32 eoc2;
    u32 first_allocated_cluster;
    u32 last_allocated_cluster;
    u32 start_cluster;
    s32 result;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;

    p_vol = p_ffd->p_vol;
    eoc2 = fat_special_values[p_vol->bpb.fat_type].eoc2;
    *p_chain_start = 0xFFFFFFFF;
    *p_last_allocated = 0xFFFFFFFF;
    last_allocated_cluster = 0xFFFFFFFF;
    result = PFFAT_SearchForNumFreeClusters(p_vol, p_vol->last_free_cluster, 0xFFFFFFFF, chain_len, &first_allocated_cluster,
                                            &last_allocated_cluster);
    if (result != 0) {
        return result;
    }
    if (first_allocated_cluster == 0xFFFFFFFF) {
        return 0;
    }
    if (is_clear != 0) {
        result = PFFAT_ClearCluster(p_vol, first_allocated_cluster);
        if (result != 0) {
            return result;
        }
    }
    result = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (result != 0) {
        return result;
    }
    result = PFFAT_ReadFATSector(p_vol, p_page, first_allocated_cluster);
    if (result != 0) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        return result;
    }
    p_page->option = 0;

    for (start_cluster = first_allocated_cluster; start_cluster < last_allocated_cluster; start_cluster++) {
        result = PFFAT_WriteClusterPage(p_ffd, start_cluster, chain_index, start_cluster + 1, 1, p_page);
        if (result != 0) {
            PFCACHE_FreeDataPage(p_vol, p_page);
            return result;
        }
        if ((p_vol->fsi_flag & PF_FSI_FLAG_FREE_COUNT_VALID) && p_vol->num_free_clusters != 0xFFFFFFFF && p_vol->num_free_clusters != 0 &&
            (p_vol->bpb.fat_type != PDM_FAT_32 || last_allocated_cluster != p_vol->bpb.root_dir_cluster))
        {
            p_vol->num_free_clusters--;
        }
        if (*p_chain_start == 0xFFFFFFFF) {
            *p_chain_start = start_cluster;
        }
        *p_last_allocated = start_cluster;
        chain_index++;
    }
    if (last_allocated_cluster != 0xFFFFFFFF) {
        result = PFFAT_WriteClusterPage(p_ffd, last_allocated_cluster, chain_index, eoc2, 1, p_page);
        if (result != 0) {
            PFCACHE_FreeDataPage(p_vol, p_page);
            return result;
        }
        if (*p_chain_start == 0xFFFFFFFF) {
            *p_chain_start = start_cluster;
        }
        *p_last_allocated = start_cluster;
        if (p_vol->num_free_clusters != 0xFFFFFFFF && p_vol->num_free_clusters != 0 &&
            (p_vol->bpb.fat_type != PDM_FAT_32 || last_allocated_cluster != p_vol->bpb.root_dir_cluster))
        {
            p_vol->num_free_clusters--;
        }
    }
    p_vol->last_free_cluster = last_allocated_cluster + 1;
    p_ffd->last_cluster.num_last_cluster = last_allocated_cluster;
    p_ffd->last_cluster.max_chain_index = chain_index;
    *p_last_allocated = last_allocated_cluster;
    result = PFFAT_UpdateFATEntry(p_vol, p_page);
    if (result != 0) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        return result;
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    if (p_vol->bpb.fat_type == PDM_FAT_32 && (p_vol->fsi_flag & 2)) {
        if (PFFAT_RefreshFSINFO(p_vol) != 0) {
            p_vol->num_free_clusters = 0xFFFFFFFF;
            p_vol->fsi_flag &= ~PF_FSI_FLAG_FREE_COUNT_VALID;
        }
    }
    return 0;
}

static inline s32 PFFAT_AllocateChain(PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated)
{
    s32 err;

    err = PFFAT_DoAllocateChain(p_ffd, chain_len, chain_index, p_chain_start, p_last_allocated, 0);
    if (err != 0) {
        if (*p_chain_start != 0xFFFFFFFF) {
            PFFAT_FreeChain(p_ffd, *p_chain_start, chain_index, 0xFFFFFFFF);
        }
        *p_chain_start = 0xFFFFFFFF;
        *p_last_allocated = 0xFFFFFFFF;
        return err;
    }
    return 0;
}

s32 PFFAT_GetClusterInChain(PF_FFD* p_ffd, u32 chain_index, u32 mode, u32 num_cluster, u32* p_locate_start, u32* p_locate_end)
{
    s32 err;
    u32 bad;
    u32 eoc1;
    u32 current_cluster;
    u32 next_cluster;
    u32 trace_cnt;
    u32 append_cnt;
    u32 search_index;
    u32* p_chain_start;
    u32 chain_start;
    u32 is_hit;
    u32 check_use;
    PF_CACHE_PAGE* p_page;

    bad = fat_special_values[p_ffd->p_vol->bpb.fat_type].bad;
    eoc1 = fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc1;
    *p_locate_end = 0xFFFFFFFF;
    *p_locate_start = 0xFFFFFFFF;
    if (p_ffd->last_access.chain_index != 0 && p_ffd->last_access.chain_index <= chain_index) {
        chain_start = p_ffd->last_access.cluster;
        p_chain_start = &chain_start;
        trace_cnt = chain_index - p_ffd->last_access.chain_index;
        search_index = p_ffd->last_access.chain_index + 1;
    } else {
        p_chain_start = p_ffd->p_start_cluster;
        trace_cnt = chain_index;
        search_index = 1;
    }
    current_cluster = *p_chain_start;
    next_cluster = *p_chain_start;
    if (mode == 2) {
        if (trace_cnt != 0 && num_cluster != 0) {
            trace_cnt = trace_cnt + num_cluster - 1;
        } else {
            trace_cnt = num_cluster;
        }
        append_cnt = trace_cnt;
        if (p_ffd->last_cluster.num_last_cluster != 0) {
            trace_cnt = 0;
            append_cnt = p_ffd->last_cluster.max_chain_index + num_cluster - chain_index;
            current_cluster = p_ffd->last_cluster.num_last_cluster;
            next_cluster = fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2;
            search_index = p_ffd->last_cluster.max_chain_index + 1;
        }
    } else {
        if (mode == 1 && current_cluster == 0) {
            trace_cnt++;
        }
        append_cnt = trace_cnt;
    }

    if (trace_cnt != 0) {
        err = PFCACHE_AllocateDataPage(p_ffd->p_vol, 0xFFFFFFFF, &p_page, &is_hit);
        if (err != 0) {
            return err;
        }
        err = PFFAT_ReadFATSector(p_ffd->p_vol, p_page, current_cluster);
        if (err != 0) {
            PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
            return err;
        }
        p_page->option = 0;
        check_use = 1;
        while (trace_cnt-- != 0 && current_cluster < bad) {
            if (current_cluster != 0) {
                err = PFFAT_ReadClusterPage(p_ffd, current_cluster, search_index, &next_cluster, &check_use, p_page);
                if (err != 0 && next_cluster != 0xFFFFFFFF) {
                    PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
                    return err;
                }
            } else {
                next_cluster = fat_special_values[p_ffd->p_vol->bpb.fat_type].eoc2;
                search_index--;
            }
            if ((next_cluster < 2 || next_cluster >= p_ffd->p_vol->bpb.num_clusters + 2) && next_cluster < eoc1) {
                PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
                return 14;
            }
            if (mode != 0 && next_cluster >= eoc1) {
                break;
            }
            current_cluster = next_cluster;
            search_index++;
        }
        append_cnt = trace_cnt;
        PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
    }

    if (mode != 0 && next_cluster >= eoc1) {
        *p_locate_start = 0;
        err = PFFAT_AllocateChain(p_ffd, ++append_cnt, search_index, p_locate_start, &next_cluster);
        if (err != 0) {
            return err;
        }
        if (next_cluster == 0xFFFFFFFF) {
            return 6;
        }
        if (*p_chain_start == 0) {
            *p_chain_start = *p_locate_start;
        } else {
            err = PFFAT_WriteCluster(p_ffd, current_cluster, search_index - 1, *p_locate_start, 1);
            if (err != 0) {
                return err;
            }
        }
        current_cluster = next_cluster;
    }
    *p_locate_end = current_cluster;
    return 0;
}

s32 PFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector)
{
    PF_VOLUME* p_vol;
    u32 cluster;
    s32 err;
    u32 next_cluster;
    u32 check_use;

    p_vol = p_ffd->p_vol;
    next_cluster = 0xFFFFFFFF;
    cluster = initial_cluster;
    check_use = 1;
    while (next_cluster != 0) {
        err = PFFAT_ReadCluster(p_ffd, cluster, chain_index, &next_cluster, &check_use);
        if (err != 0) {
            return err;
        }
        if (cluster + 1 != next_cluster) {
            if (next_cluster == fat_special_values[p_vol->bpb.fat_type].eoc2) {
                p_ffd->last_cluster.num_last_cluster = cluster;
                p_ffd->last_cluster.max_chain_index = chain_index - 1;
            }
            break;
        }
        p_ffd->p_hint->chain_index++;
        p_ffd->p_hint->cluster = next_cluster;
        *p_num_sector += p_vol->bpb.sectors_per_cluster;
        if (*p_num_sector << p_vol->bpb.log2_bytes_per_sector >= size) {
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

s32 PFFAT_GetClusterAllocatedInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_clusters)
{
    PF_VOLUME* p_vol;
    u32 cluster;
    u32 total_size;
    s32 err;
    u32 next_cluster;
    u32 check_use;

    p_vol = p_ffd->p_vol;
    next_cluster = 0xFFFFFFFF;
    cluster = initial_cluster;
    check_use = 1;
    total_size = 0;
    while (next_cluster != 0) {
        total_size += p_vol->bpb.bytes_per_sector * p_vol->bpb.sectors_per_cluster;
        (*p_num_clusters)++;
        if (total_size >= size) {
            break;
        }
        err = PFFAT_ReadCluster(p_ffd, cluster, chain_index, &next_cluster, &check_use);
        if (err != 0) {
            return err;
        }
        if (next_cluster == fat_special_values[p_vol->bpb.fat_type].eoc2) {
            p_ffd->last_cluster.num_last_cluster = cluster;
            p_ffd->last_cluster.max_chain_index = chain_index - 1;
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

static inline s32 PFFAT_GetNumberOfCluster(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster)
{
    s32 err;
    u32 locate_start;
    u32 locate_end;

    *p_cluster = 0xFFFFFFFF;
    err = PFFAT_GetClusterInChain(p_ffd, chain_index, may_allocate ? 1 : 0, 0, &locate_start, &locate_end);
    if (err != 0) {
        return err;
    }
    if (locate_end >= 2 && locate_end < p_ffd->p_vol->bpb.num_clusters + 2) {
        *p_cluster = locate_end;
    }
    return 0;
}

s32 PFFAT_GetClusterSpecified(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster)
{
    PF_VOLUME* p_vol;
    PF_FAT_HINT* p_hint;
    u32 fat_type;
    s32 err;
    u32 is_found;

    p_vol = p_ffd->p_vol;
    fat_type = p_vol->bpb.fat_type;
    p_hint = p_ffd->p_hint;

    if (*p_ffd->p_start_cluster == 0 && may_allocate == 0) {
        *p_cluster = 0xFFFFFFFF;
        return 0;
    }
    if (*p_ffd->p_start_cluster == 1) {
        if (fat_type <= PDM_FAT_16) {
            *p_cluster = 0xFFFFFFFF;
            return 0;
        }
        *p_ffd->p_start_cluster = p_vol->bpb.root_dir_cluster;
    }
    if (p_ffd->cluster_link.buffer != NULL && may_allocate == 0) {
        err = PFFAT_FindClusterLink(p_ffd, chain_index, p_cluster, &is_found);
        if (err != 0) {
            return err;
        }
        if (is_found == 1) {
            p_hint->chain_stamp = p_ffd->chain_stamp;
            p_hint->chain_index = chain_index;
            p_hint->cluster = *p_cluster;
            return 0;
        }
    }
    err = PFFAT_GetNumberOfCluster(p_ffd, chain_index, may_allocate, p_cluster);
    if (err != 0) {
        return err;
    }
    if (*p_cluster == 0xFFFFFFFF) {
        return 0;
    }
    p_hint->chain_stamp = p_ffd->chain_stamp;
    p_hint->chain_index = chain_index;
    p_hint->cluster = *p_cluster;
    return 0;
}

static inline s32 PFFAT_GetNumberOfAllocatedCluster(PF_FFD* p_ffd, u32 chain_index, u32 num_cluster, u32* p_cluster)
{
    s32 err;
    u32 locate_start;
    u32 locate_end;

    *p_cluster = 0xFFFFFFFF;
    err = PFFAT_GetClusterInChain(p_ffd, chain_index, 2, num_cluster, &locate_start, &locate_end);
    if (err != 0) {
        return err;
    }
    if (locate_start >= 2 && locate_start < p_ffd->p_vol->bpb.num_clusters + 2) {
        *p_cluster = locate_start;
    }
    return 0;
}

s32 PFFAT_GetClusterAllocated(PF_FFD* p_ffd, u32 chain_index, u32 num_cluster, u32* p_cluster)
{
    PF_VOLUME* p_vol;
    PF_FAT_HINT* p_hint;
    u32 fat_type;
    s32 err;

    p_vol = p_ffd->p_vol;
    fat_type = p_vol->bpb.fat_type;
    p_hint = p_ffd->p_hint;
    if (*p_ffd->p_start_cluster == 1) {
        if (fat_type <= PDM_FAT_16) {
            *p_cluster = 0xFFFFFFFF;
            return 0;
        }
        *p_ffd->p_start_cluster = p_vol->bpb.root_dir_cluster;
    }
    err = PFFAT_GetNumberOfAllocatedCluster(p_ffd, chain_index, num_cluster, p_cluster);
    if (err != 0) {
        return err;
    }
    if (*p_cluster == 0xFFFFFFFF) {
        return 0;
    }
    p_hint->chain_stamp = p_ffd->chain_stamp;
    p_hint->chain_index = chain_index;
    p_hint->cluster = *p_cluster;
    return 0;
}

static inline s32 PFFAT_GetSectorInRootDirRegion(PF_VOLUME* p_vol, u32 sector_offset, u32* p_sector)
{
    if (sector_offset >= p_vol->bpb.num_root_dir_sectors) {
        *p_sector = 0xFFFFFFFF;
        return 0;
    }
    *p_sector = p_vol->bpb.first_root_dir_sector + sector_offset;
    return 0;
}

s32 PFFAT_GetSector(PF_FFD* p_ffd, u32 file_sector_index, u32 mode, u32 size, u32* p_sector)
{
    s32 err;
    PF_VOLUME* p_vol;
    u32 fat_type;
    u32 chain_index;
    u32 cluster;
    u32 num_divide;
    u32 num_remaind;
    u32 num_cluster;

    p_vol = p_ffd->p_vol;
    fat_type = p_vol->bpb.fat_type;
    if (*p_ffd->p_start_cluster == 1 && fat_type <= PDM_FAT_16) {
        err = PFFAT_GetSectorInRootDirRegion(p_vol, file_sector_index, p_sector);
        if (err != 0) {
            return err;
        }
        return 0;
    }
    chain_index = file_sector_index >> p_vol->bpb.log2_sectors_per_cluster;
    if (mode == 2) {
        num_divide = size / p_vol->bpb.bytes_per_sector;
        num_remaind = size % p_vol->bpb.bytes_per_sector != 0;
        num_cluster = (num_divide + num_remaind) / p_vol->bpb.sectors_per_cluster;
        num_cluster += (num_divide + num_remaind) % p_vol->bpb.sectors_per_cluster != 0;
        err = PFFAT_GetClusterAllocated(p_ffd, chain_index, num_cluster, &cluster);
        if (err != 0) {
            return err;
        }
    } else {
        err = PFFAT_GetClusterSpecified(p_ffd, chain_index, mode, &cluster);
        if (err != 0) {
            return err;
        }
    }
    if (cluster == 0xFFFFFFFF) {
        *p_sector = 0xFFFFFFFF;
        return 0;
    }
    *p_sector = (((cluster - 2) << p_vol->bpb.log2_sectors_per_cluster) + p_vol->bpb.first_data_sector) +
                (file_sector_index & (p_vol->bpb.sectors_per_cluster - 1));
    return 0;
}

s32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 file_sector_index, u32 may_allocate, u32* p_sector)
{
    s32 err;
    u32 mode;

    mode = may_allocate ? 1 : 0;
    err = PFFAT_GetSector(p_ffd, file_sector_index, mode, 0, p_sector);
    if (err != 0) {
        return err;
    }
    return 0;
}
