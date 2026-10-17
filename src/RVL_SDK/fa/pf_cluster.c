#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_cluster.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_sector.h>

static PF_VOLUME* PFCLUSTER_GetVolume(PF_FILE* p_file)
{
    if (p_file == NULL) {
        return NULL;
    }
    return p_file->p_sfd->dir_entry.p_vol;
}

void PFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector)
{
    PF_VOLUME* p_vol;

    if (p_file->cursor.position == 0) {
        p_file->p_sfd->ffd.last_access.cluster = 0;
        p_file->p_sfd->ffd.last_access.chain_index = 0;
    } else {
        p_vol = PFCLUSTER_GetVolume(p_file);

        if ((p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1)) == 0 &&
            (p_file->cursor.file_sector_index & (p_vol->bpb.sectors_per_cluster - 1)) == 0)
        {
            if (p_file->cursor.file_sector_index != 0) {
                p_file->p_sfd->ffd.last_access.chain_index =
                    (p_file->cursor.file_sector_index - 1) >> p_vol->bpb.log2_sectors_per_cluster;
                p_file->p_sfd->ffd.last_access.cluster =
                    ((sector - 1 - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
            }
        } else {
            p_file->p_sfd->ffd.last_access.chain_index =
                p_file->cursor.file_sector_index >> p_vol->bpb.log2_sectors_per_cluster;
            p_file->p_sfd->ffd.last_access.cluster =
                ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
        }
    }
}

s32 PFCLUSTER_AppendCluster(PF_FILE* p_file, u32 byte, u32* p_success)
{
    s32 result;
    PF_VOLUME* p_vol;
    u32 max_appendable_size;
    PF_CURSOR save_cursor;
    PF_FAT_HINT save_hint;
    u32 sector;
    u32 num_sector;

    *p_success = 0;
    if ((p_file->p_sfd->stat & PF_SFD_STAT_ALLOCATED) == 0 || (p_file->p_sfd->stat & PF_SFD_STAT_OPENED) == 0) {
        return 38;
    }
    p_vol = PFCLUSTER_GetVolume(p_file);

    save_cursor = p_file->cursor;
    save_hint = p_file->hint;
    p_file->p_sfd->ffd.p_hint = &p_file->hint;

    sector = 0xFFFFFFFF;
    if ((p_vol->fsi_flag & PF_FSI_FLAG_FREE_COUNT_VALID) != 0 && p_vol->num_free_clusters == 0) {
        return 6;
    }

    PFFILE_Cursor_MoveToClusterEnd(p_file, p_file->p_sfd->dir_entry.file_size + byte);
    if (p_file->cursor.position == 0xFFFFFFFF) {
        *p_success = 0;
        return 37;
    }

    result = PFFAT_GetSectorAllocated(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, byte, &sector);
    if (result != 0) {
        p_file->cursor = save_cursor;
        p_file->hint = save_hint;
        return result;
    }

    num_sector = 0;
    result = PFFAT_GetContinuousSector(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, byte, &sector, &num_sector);
    p_file->cursor = save_cursor;
    p_file->hint = save_hint;
    if (result != 0) {
        return result;
    }

    if (sector == 0xFFFFFFFF) {
        PFFILE_Cursor_MoveToEnd(p_file);
        return 28;
    }

    max_appendable_size = num_sector << p_vol->bpb.log2_bytes_per_sector;
    if (byte < max_appendable_size) {
        max_appendable_size = byte;
    }
    *p_success = max_appendable_size;
    return 0;
}

s32 PFCLUSTER_FreeUnusedCluster(PF_FILE* p_file)
{
    s32 err;
    PF_VOLUME* p_vol;
    u32 chain_index;
    u32 cluster_size;
    u32 file_end_cluster;
    u32 unused_cluster;
    u32 sig_eoc;

    if ((p_file->p_sfd->stat & PF_SFD_STAT_ALLOCATED) == 0 || (p_file->p_sfd->stat & PF_SFD_STAT_OPENED) == 0) {
        return 38;
    }
    p_vol = PFCLUSTER_GetVolume(p_file);
    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    chain_index = 0xFFFFFFFF;

    if (p_file->p_sfd->ffd.last_cluster.max_chain_index != 0) {
        cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
        if (p_file->p_sfd->ffd.last_cluster.max_chain_index + 1 ==
            (p_file->p_sfd->dir_entry.file_size >> (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster)) +
                ((p_file->p_sfd->dir_entry.file_size & (cluster_size - 1)) ? 1 : 0))
        {
            return 0;
        }
    }

    if (p_file->p_sfd->dir_entry.file_size != 0) {
        err = PFFAT_TraceClustersChain(&p_file->p_sfd->ffd, p_file->p_sfd->dir_entry.start_cluster,
                                       p_file->p_sfd->dir_entry.file_size, &file_end_cluster, &unused_cluster);
        if (err != 0) {
            return err;
        }
    } else {
        file_end_cluster = 0;
        unused_cluster = p_file->p_sfd->dir_entry.start_cluster;
    }

    if (unused_cluster != 0) {
        sig_eoc = PFFAT_GetValueOfEOC2(p_vol);
        if (unused_cluster != sig_eoc) {
            if (file_end_cluster != 0) {
                err = PFFAT_WriteValueToSpecifiedCluster(p_vol, file_end_cluster, sig_eoc);
                if (err != 0) {
                    return err;
                }
                p_file->p_sfd->ffd.last_cluster.num_last_cluster = file_end_cluster;
                cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
                p_file->p_sfd->ffd.last_cluster.max_chain_index =
                    (p_file->p_sfd->dir_entry.file_size >> (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster)) +
                    ((p_file->p_sfd->dir_entry.file_size & (cluster_size - 1)) ? 1 : 0) - 1;
                chain_index = p_file->p_sfd->ffd.last_cluster.max_chain_index + 1;
            } else {
                p_file->p_sfd->dir_entry.start_cluster = 0;
                *p_file->p_sfd->ffd.p_start_cluster = 0;
                p_file->p_sfd->ffd.last_cluster.num_last_cluster = 0;
                p_file->p_sfd->ffd.last_cluster.max_chain_index = 0;
                err = PFENT_UpdateSFNEntry(&p_file->p_sfd->dir_entry, 1);
                if (err != 0) {
                    return err;
                }
                if ((p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) != 0) {
                    err = PFCACHE_FlushDataCacheSpecific(p_vol, p_file);
                    if (err != 0) {
                        return err;
                    }
                }
            }
            err = PFFAT_FreeChain(&p_file->p_sfd->ffd, unused_cluster, chain_index, 0xFFFFFFFF);
            if (err != 0) {
                return err;
            }
        }
    }
    return 0;
}

s32 PFCLUSTER_GetAppendSize(PF_FILE* p_file, u32* p_size)
{
    PF_VOLUME* p_vol;
    u32 cluster_size;
    u32 file_cluster_size;
    u32 total_allocated_size;
    u32 file_end_cluster;
    u32 unused_cluster;
    u32 next_cluster;
    u32 num_append_cluster;
    u32 sig_eoc;
    s32 err;
    u32 remainder;

    *p_size = 0;
    if ((p_file->p_sfd->stat & PF_SFD_STAT_ALLOCATED) == 0 || (p_file->p_sfd->stat & PF_SFD_STAT_OPENED) == 0) {
        return 38;
    }
    p_vol = PFCLUSTER_GetVolume(p_file);
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    if (p_file->p_sfd->dir_entry.start_cluster != 0) {
        if (p_file->p_sfd->dir_entry.file_size % cluster_size) {
            remainder = cluster_size - p_file->p_sfd->dir_entry.file_size % cluster_size;
        } else {
            remainder = 0;
        }
        file_cluster_size = p_file->p_sfd->dir_entry.file_size + remainder;
        if (p_file->p_sfd->ffd.last_cluster.max_chain_index != 0) {
            total_allocated_size = (p_file->p_sfd->ffd.last_cluster.max_chain_index + 1) * cluster_size;
        } else {
            if (p_file->p_sfd->dir_entry.file_size != 0) {
                err = PFFAT_TraceClustersChain(&p_file->p_sfd->ffd, p_file->p_sfd->dir_entry.start_cluster,
                                               p_file->p_sfd->dir_entry.file_size, &file_end_cluster, &unused_cluster);
                if (err != 0) {
                    return err;
                }
            } else {
                unused_cluster = p_file->p_sfd->dir_entry.start_cluster;
            }
            sig_eoc = PFFAT_GetValueOfEOC2(p_vol);
            num_append_cluster = 0;
            while (unused_cluster != sig_eoc) {
                num_append_cluster++;
                err = PFFAT_ReadValueToSpecifiedCluster(p_vol, unused_cluster, &next_cluster);
                if (err != 0) {
                    return err;
                }
                unused_cluster = next_cluster;
            }
            total_allocated_size = file_cluster_size + num_append_cluster * cluster_size;
        }
        *p_size = total_allocated_size - file_cluster_size;
    }
    return 0;
}

s32 PFCLUSTER_CombineChain(PF_ENT_ITER* p_iter_dst, PF_ENT_ITER* p_iter_src, PF_DIR_ENT* p_ent_dst, PF_DIR_ENT* p_ent_src)
{
    s32 err;
    u32 cluster_size;
    u32 num_dst_cluster;
    u32 num_src_cluster;
    u32 num_dst_unused;
    u32 num_src_unused;
    u32 num_over_cluster;
    u32 dst_end_cluster;
    u32 dst_unused_cluster;
    u32 src_unused_cluster;
    u32 sig_eoc;
    PF_VOLUME* p_vol;
    u32 end_cluster;
    u32 next_cluster;
    u32 num_total_cluster;
    u32 max_cluster;

    num_dst_cluster = 0;
    num_src_cluster = 0;
    num_dst_unused = 0;
    num_src_unused = 0;
    dst_unused_cluster = 0;
    p_vol = p_ent_dst->p_vol;

    err = PFFAT_TraceClustersChain(&p_iter_dst->ffd, *p_iter_dst->ffd.p_start_cluster, p_ent_dst->file_size, &end_cluster,
                                   &next_cluster);
    if (err != 0) {
        return err;
    }
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    dst_end_cluster = end_cluster;
    if (p_ent_dst->file_size != 0) {
        num_dst_cluster = p_ent_dst->file_size / cluster_size;
        if (p_ent_dst->file_size % cluster_size) {
            num_dst_cluster++;
        }
    }

    sig_eoc = PFFAT_GetValueOfEOC2(p_vol);
    if (next_cluster != sig_eoc) {
        if (p_ent_dst->file_size != 0 && next_cluster != 0) {
            dst_unused_cluster = next_cluster;
        } else {
            dst_unused_cluster = *p_iter_dst->ffd.p_start_cluster;
        }
        err = PFFAT_TraceClustersChain(&p_iter_dst->ffd, dst_unused_cluster, 0xFFFFFFFF, &end_cluster, &next_cluster);
        if (err != 0) {
            return err;
        }
        if (*p_iter_dst->ffd.p_start_cluster != 0) {
            num_dst_unused = p_iter_dst->ffd.last_cluster.max_chain_index + 1 - num_dst_cluster;
        }
    }

    err = PFFAT_TraceClustersChain(&p_iter_src->ffd, *p_iter_src->ffd.p_start_cluster, p_ent_src->file_size, &end_cluster,
                                   &next_cluster);
    if (err != 0) {
        return err;
    }
    if (p_ent_src->file_size != 0) {
        num_src_cluster = p_ent_src->file_size / cluster_size;
        if (p_ent_src->file_size % cluster_size) {
            num_src_cluster++;
        }
    }

    if (next_cluster != sig_eoc) {
        if (p_ent_src->file_size != 0 && next_cluster != 0) {
            src_unused_cluster = next_cluster;
        } else {
            src_unused_cluster = *p_iter_src->ffd.p_start_cluster;
        }
        err = PFFAT_TraceClustersChain(&p_iter_src->ffd, src_unused_cluster, 0xFFFFFFFF, &end_cluster, &next_cluster);
        if (err != 0) {
            return err;
        }
        if (*p_iter_src->ffd.p_start_cluster != 0) {
            num_src_unused = p_iter_src->ffd.last_cluster.max_chain_index + 1 - num_src_cluster;
        }
        max_cluster = 0xFFFFFFFF / cluster_size + 1;
        num_total_cluster = num_src_unused + num_dst_cluster + num_src_cluster;
        if (max_cluster < num_total_cluster) {
            num_over_cluster = num_total_cluster - max_cluster;
            err = PFFAT_TraceClustersChain(
                &p_iter_src->ffd, src_unused_cluster,
                cluster_size * (p_iter_src->ffd.last_cluster.max_chain_index + 1 - num_over_cluster - num_src_cluster),
                &end_cluster, &next_cluster);
            if (err != 0) {
                return err;
            }
            err = PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, sig_eoc);
            if (err != 0) {
                return err;
            }
            err = PFFAT_FreeChain(&p_iter_src->ffd, next_cluster, 0xFFFFFFFF, num_over_cluster * cluster_size);
            if (err != 0) {
                return err;
            }
            if (p_ent_src->file_size == 0 && num_src_unused == num_over_cluster) {
                *p_iter_src->ffd.p_start_cluster = 0;
                p_ent_src->start_cluster = 0;
            }
        }
    }

    if (num_dst_unused != 0) {
        err = PFFAT_FreeChain(&p_iter_dst->ffd, dst_unused_cluster, 0xFFFFFFFF, num_dst_unused * cluster_size);
        if (err != 0) {
            return err;
        }
        if (p_ent_dst->file_size == 0) {
            *p_iter_dst->ffd.p_start_cluster = 0;
            p_ent_dst->start_cluster = 0;
        }
    }

    if (*p_iter_dst->ffd.p_start_cluster != 0 && *p_iter_src->ffd.p_start_cluster != 0) {
        err = PFFAT_WriteValueToSpecifiedCluster(p_vol, dst_end_cluster, *p_iter_src->ffd.p_start_cluster);
        if (err != 0) {
            return err;
        }
    } else if (*p_iter_dst->ffd.p_start_cluster == 0 && *p_iter_src->ffd.p_start_cluster != 0) {
        p_ent_dst->start_cluster = p_ent_src->start_cluster;
    }

    p_ent_dst->file_size = p_ent_src->file_size + num_dst_cluster * cluster_size;
    return 0;
}

s32 PFCLUSTER_DivideChain(PF_ENT_ITER* p_iter, PF_ENT_ITER* p_iter_new, PF_DIR_ENT* p_ent, PF_DIR_ENT* p_ent_new, u32 size)
{
    u32 src_sector;
    u32 dst_sector;
    u32 cluster_size;
    u32 i;
    u32 prev_last_cluster;
    u32 alloc_size;
    u32 new_start_cluster;
    u32 remainder;
    PF_VOLUME* p_vol;
    u32 cut_next_cluster;
    u32 sig_eoc;
    s32 err;
    u32 end_cluster;
    u32 num_success;
    u32 start_cluster;
    u32 last_cluster;
    u32 next_cluster;
    PF_CACHE_PAGE* p_page;
    u32 is_hit;

    start_cluster = 0;
    p_vol = p_ent->p_vol;
    new_start_cluster = 0;
    prev_last_cluster = 0;
    remainder = 0;

    if (size == 0) {
        next_cluster = p_ent->start_cluster;
    } else {
        err = PFFAT_TraceClustersChain(&p_iter->ffd, *p_iter->ffd.p_start_cluster, size, &end_cluster, &next_cluster);
        if (err != 0) {
            return err;
        }
        cut_next_cluster = next_cluster;
        sig_eoc = PFFAT_GetValueOfEOC2(p_vol);
        err = PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, sig_eoc);
        if (err != 0) {
            return err;
        }

        if (size == p_ent->file_size) {
            if (next_cluster == sig_eoc) {
                next_cluster = 0;
            }
        } else {
            cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
            remainder = size % cluster_size;
            if (remainder != 0) {
                for (alloc_size = 0; alloc_size < cluster_size; alloc_size += num_success) {
                    err = PFFAT_AllocateContinuousClusters(&p_iter_new->ffd, cluster_size, &start_cluster, &last_cluster,
                                                           &num_success);
                    if (err != 0) {
                        PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                        return err;
                    }
                    if (num_success == 0) {
                        break;
                    }
                    if (alloc_size == 0) {
                        new_start_cluster = start_cluster;
                    } else {
                        err = PFFAT_WriteValueToSpecifiedCluster(p_vol, prev_last_cluster, start_cluster);
                        if (err != 0) {
                            PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                            PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                            return err;
                        }
                    }
                    prev_last_cluster = last_cluster;
                }

                if (cluster_size != alloc_size) {
                    if (alloc_size != 0) {
                        err = PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                        if (err != 0) {
                            PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                            return err;
                        }
                    }
                    PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                    PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                    return 6;
                }

                err = PFFAT_WriteValueToSpecifiedCluster(p_vol, prev_last_cluster, next_cluster);
                if (err != 0) {
                    PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                    PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                    return err;
                }
                next_cluster = new_start_cluster;

                src_sector = p_vol->bpb.first_data_sector + ((end_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
                dst_sector = p_vol->bpb.first_data_sector + ((start_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
                err = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
                if (err != 0) {
                    PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                    PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                    return err;
                }

                for (i = 0; i < p_vol->bpb.sectors_per_cluster; src_sector++, dst_sector++, i++) {
                    err = PFSEC_ReadData(p_vol, p_page->p_buf, src_sector, 0, p_vol->bpb.bytes_per_sector, &num_success, 0);
                    if (err != 0) {
                        PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                        PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                        return err;
                    }
                    if (num_success != p_vol->bpb.bytes_per_sector) {
                        PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                        PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                        return 17;
                    }
                    err = PFSEC_WriteData(p_vol, p_page->p_buf, dst_sector, 0, p_vol->bpb.bytes_per_sector, &num_success, 0);
                    if (err != 0) {
                        PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                        PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                        return err;
                    }
                    if (num_success != p_vol->bpb.bytes_per_sector) {
                        PFFAT_FreeChain(&p_iter_new->ffd, new_start_cluster, 0xFFFFFFFF, alloc_size);
                        PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, cut_next_cluster);
                        return 17;
                    }
                }
                PFCACHE_FreeDataPage(p_vol, p_page);
            }
        }
    }

    p_ent_new->start_cluster = next_cluster;
    p_ent_new->file_size = remainder + (p_ent->file_size - size);
    return 0;
}

s32 PFCLUSTER_InsertChain(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 chain_index, u32 num_cluster, u32* p_num_inserted)
{
    u32 size;
    u32 cluster_size;
    u32 alloc_size;
    u32 new_start_cluster;
    u32 prev_last_cluster;
    PF_VOLUME* p_vol;
    u32 num_file_cluster;
    u32 max_cluster;
    u32 remainder;
    u32 padding;
    s32 err;
    u32 end_cluster;
    u32 next_cluster;
    u32 num_success;
    u32 start_cluster;
    u32 last_cluster;

    p_vol = p_ent->p_vol;
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    new_start_cluster = 0;
    prev_last_cluster = 0;

    err = PFFAT_TraceClustersChain(&p_iter->ffd, *p_iter->ffd.p_start_cluster, chain_index * cluster_size, &end_cluster,
                                   &next_cluster);
    if (err != 0) {
        return err;
    }

    num_file_cluster = p_ent->file_size / cluster_size;
    if (p_ent->file_size % cluster_size) {
        num_file_cluster++;
    }
    num_file_cluster += num_cluster;
    max_cluster = 0xFFFFFFFF / cluster_size + 1;
    if (max_cluster < num_file_cluster) {
        num_cluster = num_file_cluster - max_cluster;
    }
    size = num_cluster * cluster_size;

    for (alloc_size = 0; alloc_size < size; alloc_size += num_success) {
        err = PFFAT_AllocateContinuousClusters(&p_iter->ffd, size, &start_cluster, &last_cluster, &num_success);
        if (err != 0) {
            return err;
        }
        if (num_success == 0) {
            if (alloc_size != 0) {
                break;
            }
            return 6;
        }
        if (alloc_size == 0) {
            new_start_cluster = start_cluster;
        } else {
            err = PFFAT_WriteValueToSpecifiedCluster(p_vol, prev_last_cluster, start_cluster);
            if (err != 0) {
                return err;
            }
        }
        prev_last_cluster = last_cluster;
    }

    if (next_cluster == 0) {
        next_cluster = p_ent->start_cluster != 0 ? p_ent->start_cluster : PFFAT_GetValueOfEOC2(p_vol);
    }
    err = PFFAT_WriteValueToSpecifiedCluster(p_vol, prev_last_cluster, next_cluster);
    if (err != 0) {
        return err;
    }

    if (chain_index != 0) {
        err = PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, new_start_cluster);
        if (err != 0) {
            return err;
        }
    } else {
        p_ent->start_cluster = new_start_cluster;
    }

    num_file_cluster = p_ent->file_size / cluster_size;
    remainder = p_ent->file_size % cluster_size;
    if (remainder) {
        num_file_cluster++;
    }
    padding = 0;
    if (chain_index == num_file_cluster && remainder != 0) {
        padding = cluster_size - remainder;
    }
    p_ent->file_size = p_ent->file_size + (alloc_size + padding);
    *p_num_inserted = alloc_size / cluster_size;
    return 0;
}

s32 PFCLUSTER_DeleteChain(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 chain_index, u32 num_cluster, u32* p_num_deleted)
{
    u32 cluster_size;
    PF_VOLUME* p_vol;
    u32 size;
    u32 offset;
    s32 err;
    u32 end_cluster;
    u32 next_cluster;
    u32 start_cluster;
    u32 last_cluster;

    p_vol = p_ent->p_vol;
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    offset = chain_index * cluster_size;
    err = PFFAT_TraceClustersChain(&p_iter->ffd, *p_iter->ffd.p_start_cluster, offset, &end_cluster, &start_cluster);
    if (err != 0) {
        return err;
    }
    if (chain_index == 0) {
        start_cluster = p_ent->start_cluster;
    }

    size = num_cluster * cluster_size;
    if (size > p_ent->file_size - offset) {
        size = p_ent->file_size - offset;
    }
    err = PFFAT_TraceClustersChain(&p_iter->ffd, start_cluster, size, &last_cluster, &next_cluster);
    if (err != 0) {
        return err;
    }

    if (chain_index == 0) {
        if (next_cluster == PFFAT_GetValueOfEOC2(p_vol)) {
            next_cluster = 0;
        }
        p_ent->start_cluster = next_cluster;
    }
    p_ent->file_size -= size;
    PFENT_getcurrentDateTimeForEnt(&p_ent->modify_date, &p_ent->modify_time);
    p_ent->access_date = p_ent->modify_date;
    err = PFENT_UpdateSFNEntry(p_ent, 1);
    if (err != 0) {
        return err;
    }
    if ((p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) != 0) {
        err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
        if (err != 0) {
            return err;
        }
    }

    err = PFFAT_FreeChain(&p_iter->ffd, start_cluster, 0xFFFFFFFF, size);
    if (err != 0) {
        return err;
    }
    if (chain_index != 0) {
        err = PFFAT_WriteValueToSpecifiedCluster(p_vol, end_cluster, next_cluster);
        if (err != 0) {
            return err;
        }
    }

    *p_num_deleted = size / cluster_size;
    if (size % cluster_size) {
        *p_num_deleted = size / cluster_size + 1;
    }
    return 0;
}
