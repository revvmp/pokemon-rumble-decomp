#include <revolution/fa/msc_drv.h>
#include <revolution/fa/puh_ker.h>
#include <revolution/fa/puh_ker_mem.h>
#include <revolution/fa/puh_msc.h>
#include <revolution/fa/puh_msc_blk.h>
#include <revolution/fa/puh_msc_cmd.h>
#include <string.h>

#define UHF_MSC_BLK_UNIT_MAX 8
#define UHF_MSC_BLK_UNIT_MEDIA_INSERTED 0x4
#define UHF_MSC_BLK_UNIT_REMOVABLE 0x1000

typedef struct UHFMscBlkUnit {
    PDM_DISK* p_disk;
    u32 reserved0[4];
    u32 total_sectors;
    u32 bytes_per_sector;
    u32 reserved1[2];
    u32 flags;
    u32 reserved2[2];
} UHFMscBlkUnit;

extern void* uhg_msc_memid;

static UHFMscBlkUnit st_uhs_msc_blk_unit[UHF_MSC_BLK_UNIT_MAX];
static u8* st_uhs_msc_blk_bounce_buffer;

s32 uhf_msc_blk_init(PDM_DISK* p_disk);
s32 uhf_msc_blk_finalize(PDM_DISK* p_disk);
s32 uhf_msc_blk_mount(PDM_DISK* p_disk);
s32 uhf_msc_blk_unmount(PDM_DISK* p_disk);
s32 uhf_msc_blk_format(PDM_DISK* p_disk, const u8* p_param);
s32 uhf_msc_blk_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success);
s32 uhf_msc_blk_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success);
s32 uhf_msc_blk_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info);
static s32 _uhf_msc_blk_send_message(UHFMscPacket* p_packet, u32 command);

static PDM_FUNCTBL st_uhs_msc_blk_functions = {
    uhf_msc_blk_init,
    uhf_msc_blk_finalize,
    uhf_msc_blk_mount,
    uhf_msc_blk_unmount,
    uhf_msc_blk_format,
    uhf_msc_blk_pread,
    uhf_msc_blk_pwrite,
    uhf_msc_blk_get_disk_info,
};

static UHFMscBlkUnit* _uhf_msc_blk_find_unit(PDM_DISK* p_disk) {
    UHFMscBlkUnit* p_unit = NULL;
    int i;

    for (i = 0; i < UHF_MSC_BLK_UNIT_MAX; i++) {
        if (st_uhs_msc_blk_unit[i].p_disk == p_disk) {
            p_unit = &st_uhs_msc_blk_unit[i];
            break;
        }
    }
    return p_unit;
}

s32 pfd_mscdrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension) {
    if (p_table == NULL) {
        return -1;
    }
    p_table->p_func = &st_uhs_msc_blk_functions;
    p_table->ui_ext = extension;
    return 0;
}

s32 uhf_msc_blk_init(PDM_DISK* p_disk) {
    UHFMscPacket packet = {0};

    packet.p_disk = p_disk;
    return _uhf_msc_blk_send_message(&packet, UHF_MSC_COMMAND_INIT);
}

s32 uhf_msc_blk_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info) {
    UHFMscBlkUnit* p_unit;

    if (p_disk == NULL || p_info == NULL) {
        return -1;
    }
    p_unit = _uhf_msc_blk_find_unit(p_disk);
    if (p_unit == NULL) {
        return -1;
    }
    p_info->total_sectors = p_unit->total_sectors;
    p_info->cylinders = 0;
    p_info->heads = 0;
    p_info->sectors_per_track = 0;
    p_info->bytes_per_sector = p_unit->bytes_per_sector;
    p_info->media_attr = 0;
    if (p_unit->flags & UHF_MSC_BLK_UNIT_REMOVABLE) {
        p_info->media_attr |= 1;
    }
    p_info->format_param = NULL;
    return 0;
}

s32 uhf_msc_blk_mount(PDM_DISK* p_disk) {
    UHFMscBlkUnit* p_unit;

    if (p_disk == NULL) {
        return -1;
    }
    p_unit = _uhf_msc_blk_find_unit(p_disk);
    if (p_unit == NULL) {
        return -1;
    }
    return ((p_unit->flags >> 2) & 1) - 1;
}

s32 uhf_msc_blk_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success) {
    u8 sense[3] = {0, 0, 0};
    u32 bytes_per_sector;
    UHFMscBlkUnit* p_unit;
    s32 result;
    u8* p_transfer;
    u16 num_block;
    u32 max_block;
    BOOL direct;

    *p_success = 0;
    if (p_disk == NULL || p_buf == NULL) {
        return -1;
    }
    if (count == 0) {
        return 0;
    }
    p_unit = _uhf_msc_blk_find_unit(p_disk);
    if (p_unit == NULL) {
        return -1;
    }
    if (!(p_unit->flags & UHF_MSC_BLK_UNIT_MEDIA_INSERTED)) {
        return -1;
    }
    bytes_per_sector = p_unit->bytes_per_sector;
    if (bytes_per_sector == 0) {
        return -1;
    }
    if (!uhf_ker_check_cacheable(p_buf)) {
        max_block = 0x200 / bytes_per_sector;
        if (max_block == 0) {
            return -1;
        }
        memset(st_uhs_msc_blk_bounce_buffer, 0, 0x200);
        p_transfer = st_uhs_msc_blk_bounce_buffer;
        direct = FALSE;
    } else {
        p_transfer = p_buf;
        max_block = 0x10000 / bytes_per_sector;
        direct = TRUE;
    }
    for (;;) {
        num_block = (u16)max_block;
        if (count <= max_block) {
            num_block = (u16)count;
        }
        result = usbh_msc_read10(p_unit, block, num_block, p_transfer, sense);
        if (result == 0) {
            if (!direct) {
                memcpy(p_buf, p_transfer, num_block * bytes_per_sector);
                p_buf += num_block * bytes_per_sector;
            } else {
                p_transfer += num_block * bytes_per_sector;
            }
            *p_success += num_block;
            count -= num_block;
            block += num_block;
            if (count == 0) {
                break;
            }
        } else if (result == -15) {
            if (__uhf_msc_cmd_check_sense(sense) == 34 && p_unit->p_disk != NULL) {
                pdm_disk_notify_media_insert(p_unit->p_disk);
            }
            result = -1;
            break;
        } else {
            result = -1;
            break;
        }
    }
    return result;
}

s32 uhf_msc_blk_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success) {
    u8 sense[3] = {0, 0, 0};
    UHFMscBlkUnit* p_unit;
    BOOL direct;
    u8* p_transfer;
    u16 num_block;
    u32 bytes_per_sector;
    u32 max_block;
    s32 result;

    *p_success = 0;
    if (p_disk == NULL || p_buf == NULL) {
        return -1;
    }
    if (count == 0) {
        return 0;
    }
    p_unit = _uhf_msc_blk_find_unit(p_disk);
    if (p_unit == NULL) {
        return -1;
    }
    if (!(p_unit->flags & UHF_MSC_BLK_UNIT_MEDIA_INSERTED)) {
        return -1;
    }
    bytes_per_sector = p_unit->bytes_per_sector;
    if (bytes_per_sector == 0) {
        return -1;
    }
    if (!uhf_ker_check_cacheable(p_buf)) {
        max_block = 0x200 / bytes_per_sector;
        if (max_block == 0) {
            return -1;
        }
        p_transfer = st_uhs_msc_blk_bounce_buffer;
        direct = FALSE;
    } else {
        p_transfer = (u8*)p_buf;
        max_block = 0x10000 / bytes_per_sector;
        direct = TRUE;
    }
    for (;;) {
        num_block = (u16)max_block;
        if (count <= max_block) {
            num_block = (u16)count;
        }
        if (!direct) {
            memcpy(p_transfer, p_buf, num_block * bytes_per_sector);
        }
        result = usbh_msc_write10(p_unit, block, num_block, p_transfer, sense);
        if (result == 0) {
            if (!direct) {
                p_buf += num_block * bytes_per_sector;
            }
            if (direct) {
                p_transfer += num_block * bytes_per_sector;
            }
            *p_success += num_block;
            count -= num_block;
            block += num_block;
            if (count == 0) {
                break;
            }
        } else if (result == -15) {
            if (__uhf_msc_cmd_check_sense(sense) == 34 && p_unit->p_disk != NULL) {
                pdm_disk_notify_media_insert(p_unit->p_disk);
            }
            result = -1;
            break;
        } else {
            result = -1;
            break;
        }
    }
    return result;
}

s32 uhf_msc_blk_format(PDM_DISK* p_disk, const u8* p_param) {
    if (p_disk == NULL) {
        return -1;
    }
    return 0;
}

s32 uhf_msc_blk_finalize(PDM_DISK* p_disk) {
    UHFMscPacket packet = {0};

    packet.p_disk = p_disk;
    return _uhf_msc_blk_send_message(&packet, UHF_MSC_COMMAND_FINALIZE);
}

s32 uhf_msc_blk_unmount(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -1;
    }
    return 0;
}

s32 uhf_msc_is_media_insert(PDM_DISK* p_disk) {
    UHFMscBlkUnit* p_unit;

    p_unit = _uhf_msc_blk_find_unit(p_disk);
    if (p_unit == NULL) {
        return 0;
    }
    if (!(p_unit->flags & UHF_MSC_BLK_UNIT_MEDIA_INSERTED)) {
        return 0;
    }
    pdm_disk_notify_media_insert(p_disk);
    return 1;
}

static s32 _uhf_msc_blk_send_message(UHFMscPacket* p_packet, u32 command) {
    s32 result = 0;
    s32 semaphore;
    UHFMscMessage* p_message;
    int message_id;

    message_id = uhf_msc_get_message_id();
    p_message = (UHFMscMessage*)uhf_ker_get_memory_block(uhg_msc_memid, sizeof(UHFMscMessage), 4);
    if (p_message == NULL) {
        return -1;
    }
    semaphore = uhf_ker_create_sem(0, 0);
    if (semaphore <= 0) {
        uhf_ker_release_memory_block(uhg_msc_memid, p_message);
        return -1;
    }
    p_message->command = command;
    p_message->semaphore = semaphore;
    p_message->p_result = &result;
    p_message->packet = *p_packet;
    if (uhf_ker_send_message(message_id, p_message, 0) != 0) {
        uhf_ker_release_memory_block(uhg_msc_memid, p_message);
        uhf_ker_delete_sem(semaphore);
        return -1;
    }
    if (uhf_ker_get_sem(semaphore) != 0) {
        uhf_ker_delete_sem(semaphore);
        return -1;
    }
    if (uhf_ker_delete_sem(semaphore) != 0) {
        return -1;
    }
    return result;
}
