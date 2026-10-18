#include <revolution/fa/pf_driver.h>
#include <revolution/fa/pf_clib.h>

static inline s32 ConvertDriverResult(PF_VOLUME* p_vol, s32 result)
{
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

void PFDRV_LoadVolumeLabelFromBuf(PF_VOLUME* p_vol, const u8* buf)
{
    pf_memcpy(p_vol->label, buf, 11);
    p_vol->label[11] = 0;
}

void PFDRV_StoreVolumeLabelToBuf(u8* p_buf, PF_VOLUME* p_vol)
{
    pf_memcpy(p_buf, p_vol->label, 11);
    p_buf[11] = 0;
}

s32 PFDRV_StoreVolumeLabelToBPB(PF_VOLUME* p_vol, const s8* label)
{
    s32 result;
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 num_success;

    result = PFCACHE_AllocateDataPage(p_vol, PF_SECTOR_INVALID, &p_page, &is_hit);
    if (result != 0) {
        return result;
    }
    result = ConvertDriverResult(p_vol, pdm_part_logical_read(p_vol->p_part, p_page->p_buf, 0, 1,
                                                            p_vol->bpb.bytes_per_sector, &num_success));
    if (result != 0) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        return result;
    }
    if (num_success != 1) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        return 17;
    }
    if (p_vol->bpb.fat_type == PDM_FAT_32) {
        pf_memcpy(p_page->p_buf + 71, label, 11);
    } else {
        pf_memcpy(p_page->p_buf + 43, label, 11);
    }
    result = ConvertDriverResult(p_vol, pdm_part_logical_write(p_vol->p_part, p_page->p_buf, 0, 1,
                                                             p_vol->bpb.bytes_per_sector, &num_success));
    PFCACHE_FreeDataPage(p_vol, p_page);
    if (result != 0) {
        return result;
    }
    if (num_success != 1) {
        return 17;
    }
    pf_memcpy(p_vol->label, label, 11);
    p_vol->label[11] = 0;
    return 0;
}

void PFDRV_MediaEject(PDM_PARTITION* p_part)
{
    PF_VOLUME* p_vol;
    u32 i;

    p_vol = pf_vol_set.volumes;
    for (i = 0; i < PDM_MAX_PARTITION; p_vol++, i++) {
        if (p_vol->p_part == p_part) {
            if (p_vol->flags & 8) {
                p_vol->flags |= 0x20;
            }
            p_vol->flags &= ~0xCu;
        }
    }
}

void PFDRV_MediaInsert(PDM_PARTITION* p_part)
{
    PF_VOLUME* p_vol;
    u32 i;

    p_vol = pf_vol_set.volumes;
    for (i = 0; i < PDM_MAX_PARTITION; p_vol++, i++) {
        if (p_vol->p_part == p_part) {
            p_vol->flags |= 0x14;
        }
    }
}

u32 PFDRV_IsInserted(PF_VOLUME* p_vol)
{
    return (p_vol->flags >> 2) & 1;
}

u32 PFDRV_IsMountRequested(PF_VOLUME* p_vol)
{
    return (p_vol->flags >> 4) & 1;
}

void PFDRV_ClearMountRequested(PF_VOLUME* p_vol)
{
    p_vol->flags &= ~0x10u;
}

u32 PFDRV_IsUnmountRequested(PF_VOLUME* p_vol)
{
    return (p_vol->flags >> 5) & 1;
}

void PFDRV_ClearUnmountRequested(PF_VOLUME* p_vol)
{
    p_vol->flags &= ~0x20u;
}

s32 PFDRV_init(PF_VOLUME* p_vol)
{
    s32 result;
    PDM_PART_CALLBACK callback;

    callback.insert = PFDRV_MediaInsert;
    callback.eject = PFDRV_MediaEject;
    result = pdm_part_set_callback(p_vol->p_part, &callback);
    if (result != PDM_OK) {
        return -1;
    }
    result = pdm_part_initialize(p_vol->p_part);
    if (result != PDM_OK) {
        pdm_part_clear_callback(p_vol->p_part);
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 PFDRV_finalize(PF_VOLUME* p_vol)
{
    s32 result;

    result = pdm_part_clear_callback(p_vol->p_part);
    if (result != PDM_OK) {
        return -1;
    }
    result = pdm_part_finalize(p_vol->p_part);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 PFDRV_mount(PF_VOLUME* p_vol)
{
    s32 result;
    PDM_DISK_INFO disk_info;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;
    u32 num_success;

    result = pdm_part_mount(p_vol->p_part);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    result = pdm_part_get_media_information(p_vol->p_part, &disk_info);
    if (result != PDM_OK) {
        pdm_part_unmount(p_vol->p_part, 0);
        return -1;
    }
    if (disk_info.media_attr & PDM_MEDIA_ATTR_WRITE_PROTECT) {
        p_vol->flags |= 2;
    } else {
        p_vol->flags &= ~2u;
    }
    if (disk_info.media_attr & 2) {
        p_vol->format_param = disk_info.format_param;
        p_vol->flags |= 0x40;
    }
    if (disk_info.media_attr & 8) {
        p_vol->flags |= 0x80;
    }
    PFCACHE_AllocateDataPage(p_vol, PF_SECTOR_INVALID, &p_page, &is_hit);
    result = pdm_part_logical_read(p_vol->p_part, p_page->buffer, 0, 1,
                                    512, &num_success);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            result = 0x1000;
        } else {
            result = -1;
        }
        PFCACHE_FreeDataPage(p_vol, p_page);
        pdm_part_unmount(p_vol->p_part, 0);
        return result;
    }
    result = pdm_bpb_get_bpb_information(&p_vol->bpb, p_page->buffer);
    if (result != PDM_OK) {
        PFCACHE_FreeDataPage(p_vol, p_page);
        pdm_part_unmount(p_vol->p_part, 0);
        return 7;
    }
    p_vol->num_free_clusters = 0xFFFFFFFF;
    p_vol->last_free_cluster = 0xFFFFFFFF;
    if (p_vol->bpb.fat_type == PDM_FAT_32) {
        result = pdm_part_logical_read(p_vol->p_part, p_page->buffer,
                                        p_vol->bpb.fs_info_sector, 1, 512,
                                        &num_success);
        if (result != PDM_OK) {
            if (result == PDM_ERR_DRIVER) {
                pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
                p_vol->last_driver_error = pf_vol_set.last_driver_error;
                result = 0x1000;
            } else {
                result = -1;
            }
            PFCACHE_FreeDataPage(p_vol, p_page);
            pdm_part_unmount(p_vol->p_part, 0);
            return result;
        }
        result = pdm_bpb_get_fsinfo_information(&p_vol->bpb, p_page->buffer,
                                                &p_vol->num_free_clusters, &p_vol->last_free_cluster);
        if (result != PDM_OK) {
            PFCACHE_FreeDataPage(p_vol, p_page);
            pdm_part_unmount(p_vol->p_part, 0);
            return -1;
        }
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}

s32 PFDRV_unmount(PF_VOLUME* p_vol, u32 mode)
{
    s32 result;

    result = pdm_part_unmount(p_vol->p_part, mode == 1);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 PFDRV_format(PF_VOLUME* p_vol, const u8* param)
{
    s32 result;

    result = pdm_part_format(p_vol->p_part, param);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 PFDRV_lread(PF_VOLUME* p_vol, u8* buf, u32 sector, u32 num_sectors, u32* p_num_success)
{
    s32 result;

    result = pdm_part_logical_read(p_vol->p_part, buf, sector, num_sectors,
                                   p_vol->bpb.bytes_per_sector, p_num_success);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 PFDRV_lwrite(PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sectors, u32* p_num_success)
{
    s32 result;

    result = pdm_part_logical_write(p_vol->p_part, buf, sector, num_sectors,
                                    p_vol->bpb.bytes_per_sector, p_num_success);
    if (result != PDM_OK) {
        if (result == PDM_ERR_DRIVER) {
            pf_vol_set.last_driver_error = pdm_part_get_driver_error_code(p_vol->p_part);
            p_vol->last_driver_error = pf_vol_set.last_driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}
