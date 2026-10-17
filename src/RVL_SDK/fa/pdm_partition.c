#include <revolution/fa/pdm.h>

#define PDM_MBR_ENTRY(buf, i, off)                                                                                    \
    ((buf)[0x1C6 + (i) * 16 + (off)] + ((buf)[0x1C7 + (i) * 16 + (off)] << 8) +                                      \
     ((buf)[0x1C8 + (i) * 16 + (off)] << 16) + ((buf)[0x1C9 + (i) * 16 + (off)] << 24))

s32 pdm_part_get_start_sector(PDM_PARTITION* p_part)
{
    s32 err;
    u32 part_id;
    u32 part_cnt;
    u32 sector;
    u32 num_sector;
    u32 base_sector;
    u16 i;
    u16 j;
    u32 is_mbr;
    u32 num_success;
    u32 start_sectors[4];
    u32 num_sectors[4];
    u8 buf[512] ATTRIBUTE_ALIGN(32);

    part_cnt = 0;
    p_part->start_sector = 0;
    part_id = p_part->part_id;
    p_part->total_sector = p_part->p_disk->disk_info.total_sectors;

    err = pdm_disk_physical_read(p_part->p_disk, buf, 0, 1, 0x200, &num_success);
    if (err != PDM_OK) {
        return err;
    }

    pdm_mbr_check_master_boot_record(buf, p_part->p_disk->disk_info.total_sectors, &is_mbr);
    if (is_mbr) {
        start_sectors[0] = PDM_MBR_ENTRY(buf, 0, 0);
        start_sectors[1] = PDM_MBR_ENTRY(buf, 1, 0);
        start_sectors[2] = PDM_MBR_ENTRY(buf, 2, 0);
        start_sectors[3] = PDM_MBR_ENTRY(buf, 3, 0);
        num_sectors[0] = PDM_MBR_ENTRY(buf, 0, 4);
        num_sectors[1] = PDM_MBR_ENTRY(buf, 1, 4);
        num_sectors[2] = PDM_MBR_ENTRY(buf, 2, 4);
        num_sectors[3] = PDM_MBR_ENTRY(buf, 3, 4);

        part_cnt = 0;
        for (i = 0; i < 4; i++) {
            sector = start_sectors[i];
            num_sector = num_sectors[i];
            if (sector == 0 || num_sector == 0) {
                return PDM_ERR_PARTITION_NOT_FOUND;
            }

            err = pdm_disk_physical_read(p_part->p_disk, buf, sector, 1, 0x200, &num_success);
            if (err != PDM_OK) {
                return err;
            }

            pdm_mbr_check_master_boot_record(buf, p_part->p_disk->disk_info.total_sectors, &is_mbr);
            if (is_mbr) {
                base_sector = start_sectors[i];
                sector = base_sector;
                for (j = 0; j < 2; j++) {
                    if (j == 0) {
                        sector += PDM_MBR_ENTRY(buf, 0, 0);
                        num_sector = PDM_MBR_ENTRY(buf, 0, 4);
                        err = pdm_disk_physical_read(p_part->p_disk, buf, sector, 1, 0x200, &num_success);
                        if (err != PDM_OK) {
                            return err;
                        }
                        if (part_id == part_cnt) {
                            p_part->start_sector = sector;
                            p_part->total_sector = num_sector;
                            return PDM_OK;
                        }
                        part_cnt++;
                    } else {
                        sector = base_sector + PDM_MBR_ENTRY(buf, 1, 0);
                        num_sector = PDM_MBR_ENTRY(buf, 1, 4);
                        if (sector == 0 || num_sector == 0) {
                            break;
                        }
                        err = pdm_disk_physical_read(p_part->p_disk, buf, sector, 1, 0x200, &num_success);
                        if (err != PDM_OK) {
                            return err;
                        }
                        pdm_mbr_check_master_boot_record(buf, p_part->p_disk->disk_info.total_sectors, &is_mbr);
                        if (!is_mbr) {
                            break;
                        }
                        j = 0;
                    }
                }
            } else {
                if (part_id == part_cnt) {
                    p_part->start_sector = sector;
                    p_part->total_sector = num_sector;
                    break;
                }
                part_cnt++;
            }
        }
    } else {
        if (part_id >= 2) {
            return PDM_ERR_PARTITION_NOT_FOUND;
        }
        p_part->start_sector = part_cnt;
        p_part->total_sector = p_part->p_disk->disk_info.total_sectors;
    }
    return PDM_OK;
}

u32 pdm_part_chg_ltop(PDM_PARTITION* p_part, u32 lsector, u16 lbps)
{
    u32 start_sector;
    u16 lba_size;
    u16 p_delta;
    u16 l_delta;

    pdm_disk_get_lba_size(p_part->p_disk, &lba_size);
    if (lbps == lba_size) {
        return lsector + p_part->start_sector;
    }

    p_delta = lba_size / 512;
    start_sector = p_part->start_sector;
    if (p_delta == 2) {
        start_sector <<= 1;
    } else if (p_delta == 4) {
        start_sector <<= 2;
    } else if (p_delta == 8) {
        start_sector <<= 3;
    }

    l_delta = lbps / 512;
    if (l_delta == 2) {
        start_sector >>= 1;
    } else if (l_delta == 4) {
        start_sector >>= 2;
    } else if (l_delta == 8) {
        start_sector >>= 3;
    }
    return lsector + start_sector;
}

s32 pdm_part_open_partition(PDM_DISK* p_disk, u32 part_id, PDM_PARTITION** pp_part)
{
    s32 err;
    u16 part_no;

    *pp_part = NULL;
    if (pdm_disk_set.num_partition >= PDM_MAX_PARTITION) {
        return PDM_ERR_TOO_MANY_PARTITIONS;
    }

    for (part_no = 0; part_no < PDM_MAX_PARTITION; part_no++) {
        if (!(pdm_disk_set.partition[part_no].status & PDM_PART_STATUS_OPEN)) {
            break;
        }
    }
    if (part_no == PDM_MAX_PARTITION) {
        return PDM_ERR_TOO_MANY_PARTITIONS;
    }

    err = pdm_disk_set_disk(p_disk, &pdm_disk_set.partition[part_no]);
    if (err != PDM_OK) {
        return err;
    }

    pdm_disk_set.num_partition++;
    pdm_disk_set.partition[part_no].status |= PDM_PART_STATUS_OPEN;
    pdm_disk_set.partition[part_no].p_disk = p_disk;
    pdm_disk_set.partition[part_no].part_id = part_id;
    *pp_part = &pdm_disk_set.partition[part_no];
    return PDM_OK;
}

s32 pdm_part_close_partition(PDM_PARTITION* p_part)
{
    p_part->status &= ~PDM_PART_STATUS_OPEN;
    pdm_disk_set.num_partition--;
    return PDM_OK;
}

u32 pdm_part_is_initialized(PDM_PARTITION* p_part)
{
    return (p_part->status & PDM_PART_STATUS_INITIALIZED) >> 2;
}

void pdm_part_set_driver_error_code(PDM_PARTITION* p_part, s32 error_code)
{
    p_part->driver_last_error = error_code;
}

s32 pdm_part_get_driver_error_code(PDM_PARTITION* p_part)
{
    return p_part->driver_last_error;
}

s32 pdm_part_get_media_information(PDM_PARTITION* p_part, PDM_DISK_INFO* p_disk_info)
{
    s32 err;

    err = pdm_disk_get_media_information(p_part->p_disk, p_disk_info);
    if (err != PDM_OK) {
        return err;
    }
    return PDM_OK;
}

s32 pdm_part_initialize(PDM_PARTITION* p_part)
{
    s32 err;

    if (!(p_part->status & PDM_PART_STATUS_INITIALIZED)) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != PDM_OK) {
            return err;
        }
        err = pdm_disk_initialize(p_part->p_disk);
        if (err != PDM_OK) {
            return err;
        }
    } else {
        return PDM_ERR_INITIALIZED;
    }
    p_part->status |= PDM_PART_STATUS_INITIALIZED;
    return PDM_OK;
}

s32 pdm_part_mount(PDM_PARTITION* p_part)
{
    s32 err;

    if (!(p_part->status & PDM_PART_STATUS_MOUNTED)) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != PDM_OK) {
            return err;
        }
        err = pdm_disk_mount(p_part->p_disk);
        if (err != PDM_OK) {
            return err;
        }
        err = pdm_part_get_start_sector(p_part);
        if (err != PDM_OK) {
            pdm_disk_unmount(p_part->p_disk, 0);
            return err;
        }
    } else {
        return PDM_ERR_MOUNTED;
    }
    p_part->status = (p_part->status & ~PDM_PART_STATUS_MEDIA_CHANGED) | PDM_PART_STATUS_MOUNTED;
    return PDM_OK;
}

s32 pdm_part_format(PDM_PARTITION* p_part, const u8* param)
{
    s32 err;

    err = pdm_disk_set_disk(p_part->p_disk, p_part);
    if (err != PDM_OK) {
        return err;
    }
    err = pdm_disk_format(p_part->p_disk, param);
    if (err != PDM_OK) {
        return err;
    }
    return PDM_OK;
}

s32 pdm_part_logical_read(PDM_PARTITION* p_part, u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success)
{
    s32 err;
    u32 psector;

    if ((p_part->status & PDM_PART_STATUS_MOUNTED) != PDM_PART_STATUS_MOUNTED ||
        (p_part->status & PDM_PART_STATUS_MEDIA_CHANGED) == PDM_PART_STATUS_MEDIA_CHANGED) {
        return PDM_ERR_ACCESS;
    }

    err = pdm_disk_set_disk(p_part->p_disk, p_part);
    if (err != PDM_OK) {
        return err;
    }

    if (p_part->total_sector <= lsector) {
        return PDM_ERR_OUT_OF_RANGE;
    }
    if (p_part->total_sector < lsector + num_sector) {
        num_sector -= lsector + num_sector - p_part->total_sector;
    }

    psector = pdm_part_chg_ltop(p_part, lsector, bps);
    err = pdm_disk_physical_read(p_part->p_disk, buf, psector, num_sector, bps, p_num_success);
    if (err != PDM_OK) {
        return err;
    }
    return PDM_OK;
}

s32 pdm_part_logical_write(PDM_PARTITION* p_part, const u8* buf, u32 lsector, u32 num_sector, u16 bps,
                           u32* p_num_success)
{
    s32 err;
    u32 psector;

    if ((p_part->status & PDM_PART_STATUS_MOUNTED) != PDM_PART_STATUS_MOUNTED ||
        (p_part->status & PDM_PART_STATUS_MEDIA_CHANGED) == PDM_PART_STATUS_MEDIA_CHANGED) {
        return PDM_ERR_ACCESS;
    }

    err = pdm_disk_set_disk(p_part->p_disk, p_part);
    if (err != PDM_OK) {
        return err;
    }

    if (p_part->total_sector <= lsector) {
        return PDM_ERR_OUT_OF_RANGE;
    }
    if (p_part->total_sector < lsector + num_sector) {
        num_sector -= lsector + num_sector - p_part->total_sector;
    }

    psector = pdm_part_chg_ltop(p_part, lsector, bps);
    err = pdm_disk_physical_write(p_part->p_disk, buf, psector, num_sector, bps, p_num_success);
    if (err != PDM_OK) {
        return err;
    }
    return PDM_OK;
}

s32 pdm_part_unmount(PDM_PARTITION* p_part, u32 mode)
{
    s32 err;
    s32 result = PDM_OK;

    if (p_part->status & PDM_PART_STATUS_MOUNTED) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != PDM_OK) {
            return err;
        }
        err = pdm_disk_unmount(p_part->p_disk, mode);
        if (err != PDM_OK) {
            result = err;
        }
    } else {
        result = PDM_ERR_NOT_MOUNTED;
    }

    if (result == PDM_OK || mode == 1) {
        p_part->status &= ~PDM_PART_STATUS_MEDIA_CHANGED;
        p_part->status &= ~PDM_PART_STATUS_MOUNTED;
    }
    return result;
}

s32 pdm_part_finalize(PDM_PARTITION* p_part)
{
    s32 err;

    if (p_part->status & PDM_PART_STATUS_INITIALIZED) {
        err = pdm_disk_set_disk(p_part->p_disk, p_part);
        if (err != PDM_OK) {
            return err;
        }
        err = pdm_disk_finalize(p_part->p_disk);
        if (err != PDM_OK) {
            return err;
        }
    } else {
        return PDM_ERR_NOT_OPENED;
    }
    p_part->status &= ~PDM_PART_STATUS_INITIALIZED;
    return PDM_OK;
}

s32 pdm_part_set_callback(PDM_PARTITION* p_part, PDM_PART_CALLBACK* p_callback)
{
    p_part->callback.insert = p_callback->insert;
    p_part->callback.eject = p_callback->eject;
    return PDM_OK;
}

s32 pdm_part_clear_callback(PDM_PARTITION* p_part)
{
    p_part->callback.insert = NULL;
    p_part->callback.eject = NULL;
    return PDM_OK;
}

void pdm_part_set_change_media_state(PDM_DISK* p_disk, u32 event)
{
    u16 part_no;
    PDM_PARTITION* p_part;

    for (part_no = 0; part_no < PDM_MAX_PARTITION; part_no++) {
        p_part = &pdm_disk_set.partition[part_no];
        if ((pdm_disk_set.partition[part_no].status & PDM_PART_STATUS_OPEN) &&
            pdm_disk_set.partition[part_no].p_disk == p_disk) {
            p_part->status |= PDM_PART_STATUS_MEDIA_CHANGED;
            if (event == 0 && pdm_disk_set.partition[part_no].callback.eject != NULL) {
                pdm_disk_set.partition[part_no].callback.eject(p_part);
            } else if (pdm_disk_set.partition[part_no].callback.insert != NULL) {
                pdm_disk_set.partition[part_no].callback.insert(p_part);
            }
        }
    }
}
