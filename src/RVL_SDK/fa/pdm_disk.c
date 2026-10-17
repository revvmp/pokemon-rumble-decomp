#include <revolution/fa/pdm.h>
#include <revolution/fa/pf_clib.h>

static PDM_DISK_TBL pdm_disk_tbl[PDM_MAX_DISK];

static s32 pdm_disk_convert_lsector_to_block(PDM_DISK* p_disk, u32 lsector, u32 num_lsector, u16 bps, u32* p_block,
                                             u32* p_num_block)
{
    s32 result;
    u32 ratio;

    *p_block = lsector;
    result = PDM_OK;
    *p_num_block = num_lsector;

    if (bps > 0x200) {
        ratio = bps >> 9;
        if (ratio == 2) {
            *p_block <<= 1;
        } else if (ratio == 4) {
            *p_block <<= 2;
        } else if (ratio == 8) {
            *p_block <<= 3;
        }

        if (ratio == 2) {
            *p_num_block <<= 1;
        } else if (ratio == 4) {
            *p_num_block <<= 2;
        } else if (ratio == 8) {
            *p_num_block <<= 3;
        }
    } else if (bps < 0x200) {
        result = PDM_ERR_INVALID_BPB;
    }
    return result;
}

static s32 pdm_disk_convert_block_to_lsector(PDM_DISK* p_disk, u32 block, u32 num_block, u16 bps, u32* p_lsector,
                                             u32* p_num_lsector)
{
    s32 result;
    u32 ratio;

    *p_lsector = block;
    result = PDM_OK;
    *p_num_lsector = num_block;

    if (bps > 0x200) {
        ratio = bps >> 9;
        if (ratio == 2) {
            *p_lsector >>= 1;
        } else if (ratio == 4) {
            *p_lsector >>= 2;
        } else if (ratio == 8) {
            *p_lsector >>= 3;
        }

        if (ratio == 2) {
            *p_num_lsector >>= 1;
        } else if (ratio == 4) {
            *p_num_lsector >>= 2;
        } else if (ratio == 8) {
            *p_num_lsector >>= 3;
        }
    } else if (bps < 0x200) {
        result = PDM_ERR_INVALID_BPB;
    }
    return result;
}

s32 pdm_disk_init_disk_set(void)
{
    pf_memset(&pdm_disk_set, 0, sizeof(PDM_DISK_SET));
    return PDM_OK;
}

s32 pdm_disk_open_disk(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk)
{
    PDM_DISK* p_disk;
    PDM_DISK_TBL* p_disk_tbl;
    u16 disk_no;

    *pp_disk = NULL;
    if (pdm_disk_set.num_allocated_disk >= PDM_MAX_DISK) {
        return PDM_ERR_TOO_MANY_DISKS;
    }

    for (disk_no = 0; disk_no < PDM_MAX_DISK; disk_no++) {
        if (!(pdm_disk_set.disk[disk_no].status & PDM_DISK_STATUS_OPEN)) {
            break;
        }
    }
    if (disk_no >= PDM_MAX_DISK) {
        return PDM_ERR_TOO_MANY_DISKS;
    }

    p_disk_tbl = &pdm_disk_tbl[disk_no];
    p_init_disk_tbl->p_func(p_disk_tbl, p_init_disk_tbl->ui_ext);

    p_disk = &pdm_disk_set.disk[disk_no];
    p_disk->disk_tbl.p_func = p_disk_tbl->p_func;
    p_disk->disk_tbl.ui_ext = p_disk_tbl->ui_ext;
    p_disk->status |= PDM_DISK_STATUS_OPEN;
    pdm_disk_set.num_allocated_disk++;
    *pp_disk = p_disk;
    return PDM_OK;
}

s32 pdm_disk_close_disk(PDM_DISK* p_disk)
{
    if (p_disk->open_disk_cnt != 0) {
        return PDM_ERR_DISK_OPENED;
    }
    if (p_disk->mount_cnt != 0) {
        return PDM_ERR_DISK_MOUNTED;
    }

    p_disk->status &= ~PDM_DISK_STATUS_OPEN;
    pdm_disk_set.num_allocated_disk--;
    return PDM_OK;
}

s32 pdm_disk_set_disk(PDM_DISK* p_disk, struct PDM_PARTITION* p_part)
{
    p_disk->p_cur_part = p_part;
    return PDM_OK;
}

s32 pdm_disk_get_media_information(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info)
{
    *p_disk_info = p_disk->disk_info;
    return PDM_OK;
}

s32 pdm_disk_get_lba_size(PDM_DISK* p_disk, u16* p_lba_size)
{
    *p_lba_size = p_disk->disk_info.bytes_per_sector;
    return PDM_OK;
}

s32 pdm_disk_initialize(PDM_DISK* p_disk)
{
    s32 err;

    if (p_disk->open_disk_cnt == 0) {
        err = p_disk->disk_tbl.p_func->init(p_disk);
        if (err != PDM_OK) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
            return PDM_ERR_DRIVER;
        }
    }
    p_disk->open_disk_cnt++;
    return PDM_OK;
}

s32 pdm_disk_mount(PDM_DISK* p_disk)
{
    s32 err;

    if (p_disk->mount_cnt == 0) {
        err = p_disk->disk_tbl.p_func->mount(p_disk);
        if (err != PDM_OK) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
            return PDM_ERR_DRIVER;
        }

        err = p_disk->disk_tbl.p_func->get_disk_info(p_disk, &p_disk->disk_info);
        if (err != PDM_OK) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
            p_disk->disk_tbl.p_func->unmount(p_disk);
            return PDM_ERR_DRIVER;
        }

        if (p_disk->disk_info.media_attr & PDM_MEDIA_ATTR_WRITE_PROTECT) {
            p_disk->status |= PDM_DISK_STATUS_WRITE_PROTECT;
        } else {
            p_disk->status &= ~PDM_DISK_STATUS_WRITE_PROTECT;
        }
    }
    p_disk->mount_cnt++;
    return PDM_OK;
}

s32 pdm_disk_format(PDM_DISK* p_disk, const u8* param)
{
    s32 err;

    err = p_disk->disk_tbl.p_func->format(p_disk, param);
    if (err != PDM_OK) {
        pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
        return PDM_ERR_DRIVER;
    }
    return PDM_OK;
}

s32 pdm_disk_physical_read(PDM_DISK* p_disk, u8* buf, u32 lsector, u32 num_lsector, u16 bps, u32* p_num_success)
{
    s32 err;
    u32 block;
    u32 num_block;
    u32 lsector_dummy;

    err = pdm_disk_convert_lsector_to_block(p_disk, lsector, num_lsector, bps, &block, &num_block);
    if (err != PDM_OK) {
        return err;
    }

    err = p_disk->disk_tbl.p_func->physical_read(p_disk, buf, block, num_block, p_num_success);
    pdm_disk_convert_block_to_lsector(p_disk, block, *p_num_success, bps, &lsector_dummy, p_num_success);
    if (err != PDM_OK) {
        pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
        return PDM_ERR_DRIVER;
    }
    return PDM_OK;
}

s32 pdm_disk_physical_write(PDM_DISK* p_disk, const u8* buf, u32 lsector, u32 num_lsector, u16 bps,
                            u32* p_num_success)
{
    s32 err;
    u32 block;
    u32 num_block;
    u32 lsector_dummy;

    err = pdm_disk_convert_lsector_to_block(p_disk, lsector, num_lsector, bps, &block, &num_block);
    if (err != PDM_OK) {
        return err;
    }

    err = p_disk->disk_tbl.p_func->physical_write(p_disk, buf, block, num_block, p_num_success);
    pdm_disk_convert_block_to_lsector(p_disk, block, *p_num_success, bps, &lsector_dummy, p_num_success);
    if (err != PDM_OK) {
        pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
        return PDM_ERR_DRIVER;
    }
    return PDM_OK;
}

s32 pdm_disk_unmount(PDM_DISK* p_disk, u32 mode)
{
    s32 result = PDM_OK;

    if (p_disk->mount_cnt == 1) {
        result = p_disk->disk_tbl.p_func->unmount(p_disk);
        if (result != PDM_OK) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, result);
            result = PDM_ERR_DRIVER;
        }
    } else if (p_disk->mount_cnt == 0) {
        return PDM_ERR_NOT_MOUNTED;
    }

    if (result == PDM_OK || mode == 1) {
        p_disk->mount_cnt--;
    }
    return result;
}

s32 pdm_disk_finalize(PDM_DISK* p_disk)
{
    s32 err;

    if (p_disk->open_disk_cnt == 1) {
        err = p_disk->disk_tbl.p_func->finalize(p_disk);
        if (err != PDM_OK) {
            pdm_part_set_driver_error_code(p_disk->p_cur_part, err);
            return PDM_ERR_DRIVER;
        }
    } else if (p_disk->open_disk_cnt == 0) {
        return PDM_ERR_NOT_OPENED;
    }
    p_disk->open_disk_cnt--;
    return PDM_OK;
}

void pdm_disk_notify_media_eject(PDM_DISK* p_disk)
{
    p_disk->status &= ~PDM_DISK_STATUS_INSERTED;
    pdm_part_set_change_media_state(p_disk, 0);
}

void pdm_disk_notify_media_insert(PDM_DISK* p_disk)
{
    p_disk->status |= PDM_DISK_STATUS_INSERTED;
    pdm_part_set_change_media_state(p_disk, 1);
}
