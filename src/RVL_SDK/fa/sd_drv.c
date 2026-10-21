#include <revolution/os/OSTime.h>
s32 pfd_sddrv_store_fat32_fsi_buf(u8* p_buffer);
#include <revolution/fa/sd_drv.h>
#include <revolution/fa/pfd_cmn.h>
#include <revolution/fa/isd.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/os.h>

#define SD_DRIVER_INITIALIZED 1
#define SD_DRIVER_MOUNTED 2
#define SD_DRIVER_ISD_READY 4

#define SD_MOUNT_RETRIES 5

typedef struct PFD_SD_DRIVER {
    u32 status;
    u32 sector_size;
    ISD_HANDLE* p_handle;
    PDM_DISK* p_disk;
    s32 inserted;
    s32 media_changed;
    char drive;
} PFD_SD_DRIVER;

static volatile PFD_SD_DRIVER sSdDriver;
static ISD_HANDLE sSdHandle;
static u8 sSdSectorBuffer[512] ATTRIBUTE_ALIGN(32);
static u32 sSdIntrEvent ATTRIBUTE_ALIGN(32);
static PFD_SD_MEDIA_CALLBACK sSdEjectCallback;
static PFD_SD_MEDIA_CALLBACK sSdInsertCallback;

static s32 pfd_st_inter_callback(s32 event, void* p_context);
static s32 pfd_st_removal_callback(s32 event, void* p_context);
s32 pfd_sddrv_physical_read(u32 count, u8* p_buf, u32 block, u32 sector_size, u32* p_success);
s32 pfd_sddrv_physical_write(u32 count, const u8* p_buf, u32 block, u32 sector_size, u32* p_success);
s32 pfd_sddrv_full_format(void);

s32 pfd_sddrv_get_total_sectors(u32* p_total, u16* p_sector_size);
static const PDM_FUNCTBL sSdFunctions = {pfd_sddrv_init, pfd_sddrv_finalize, pfd_sddrv_mount, pfd_sddrv_unmount, pfd_sddrv_format, pfd_sddrv_pread, pfd_sddrv_pwrite, pfd_sddrv_get_disk_info};

typedef struct SD_FORMAT_GEOMETRY {
    u32 alignmentSectors;
    u32 heads;
    u32 sectorsPerTrack;
    u8 sectorsPerCluster;
} SD_FORMAT_GEOMETRY;

typedef struct SD_FORMAT_CAPACITY_RULE {
    u32 lowerSectors;
    u32 upperSectors;
    SD_FORMAT_GEOMETRY geometry;
} SD_FORMAT_CAPACITY_RULE;

typedef struct SD_FORMAT_PARAMETERS {
    u32 partitionStart;
    u32 partitionSectors;
    u8 sectorsPerCluster;
    u32 sectorsPerFat;
    u32 volumeId;
    u32 fatType;
    u32 reservedSectors;
    u32 totalSectors;
} SD_FORMAT_PARAMETERS;

static const SD_FORMAT_CAPACITY_RULE sSdFormatCapacityRules[14] = {
    {0, 0x1000, {0x10, 2, 0x10, 0x10}},
    {0x1000, 0x4000, {0x10, 2, 0x20, 0x10}},
    {0x4000, 0x8000, {0x20, 2, 0x20, 0x20}},
    {0x8000, 0x10000, {0x20, 4, 0x20, 0x20}},
    {0x10000, 0x20000, {0x20, 8, 0x20, 0x20}},
    {0x20000, 0x40000, {0x40, 8, 0x20, 0x20}},
    {0x40000, 0x80000, {0x40, 0x10, 0x20, 0x20}},
    {0x80000, 0xFC000, {0x80, 0x10, 0x3F, 0x20}},
    {0xFC000, 0x1F8000, {0x80, 0x20, 0x3F, 0x20}},
    {0x1F8000, 0x200000, {0x80, 0x40, 0x3F, 0x20}},
    {0x200000, 0x3F0000, {0x80, 0x40, 0x3F, 0x40}},
    {0x3F0000, 0x400000, {0x80, 0x80, 0x3F, 0x40}},
    {0x400000, 0x7E0000, {0x2000, 0x80, 0x3F, 0x40}},
    {0x7E0000, 0x4000000, {0x2000, 0xFF, 0x3F, 0x40}}
};

static inline s32 pfd_sddrv_get_value_with_total_sectors(u32 totalSectors, SD_FORMAT_GEOMETRY* p_geometry) {
    s32 index;
    for (index = 0; index < 14; index++) {
        if (sSdFormatCapacityRules[index].lowerSectors < totalSectors && sSdFormatCapacityRules[index].upperSectors >= totalSectors) {
            p_geometry->alignmentSectors = sSdFormatCapacityRules[index].geometry.alignmentSectors;
            p_geometry->heads = sSdFormatCapacityRules[index].geometry.heads;
            p_geometry->sectorsPerTrack = sSdFormatCapacityRules[index].geometry.sectorsPerTrack;
            p_geometry->sectorsPerCluster = sSdFormatCapacityRules[index].geometry.sectorsPerCluster;
            break;
        }
    }
    return index == 14 ? -30 : 0;
}

static inline u32 pfd_sddrv_count_data_clusters(u32 dataSectors, u32 sectorsPerCluster) {
    return dataSectors / sectorsPerCluster + 1;
}

static inline u32 pfd_sddrv_count_format_clusters(SD_FORMAT_PARAMETERS* p_parameters, u32 partitionStart, s32 sectorsPerFat) {
    return (p_parameters->totalSectors - partitionStart - (2 * sectorsPerFat + 33)) / p_parameters->sectorsPerCluster + 1;
}


static inline void SdUpdateDrive(void) {
    char drive;

    if (sSdDriver.drive == 0 && pfd_get_media_drv_char(sSdDriver.p_disk, &drive, 1) == 1) {
        sSdDriver.drive = drive;
    }
}

static s32 pfd_st_inter_callback(s32 event, void* p_context) {
    if ((event & ISD_EVENT_INSERT) != ISD_EVENT_INSERT) {
        return 0;
    }

    if (sSdDriver.p_handle) {
        sSdIntrEvent = ISD_EVENT_REMOVE;
        if (ISD_RegisterDeviceIntrHandler(sSdDriver.p_handle, pfd_st_removal_callback, &sSdIntrEvent) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_inter_callback()\n");
        }
    }

    if (sSdDriver.p_disk) {
        SdUpdateDrive();
        pdm_disk_notify_media_insert(sSdDriver.p_disk);
        if (sSdDriver.drive != 0 && sSdInsertCallback) {
            sSdInsertCallback(sSdDriver.drive);
        }
    }

    sSdDriver.inserted = 1;
    sSdDriver.media_changed = 1;
    return 0;
}

static s32 pfd_st_removal_callback(s32 event, void* p_context) {
    if ((event & ISD_EVENT_REMOVE) != ISD_EVENT_REMOVE) {
        return 0;
    }

    sSdDriver.inserted = 0;
    if (sSdDriver.p_handle) {
        sSdIntrEvent = ISD_EVENT_INSERT;
        if (ISD_RegisterDeviceIntrHandler(sSdDriver.p_handle, pfd_st_inter_callback, &sSdIntrEvent) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_removal_callback()\n");
        }
    }

    if (sSdDriver.p_disk) {
        SdUpdateDrive();
        pdm_disk_notify_media_eject(sSdDriver.p_disk);
        if (sSdDriver.drive != 0 && sSdEjectCallback) {
            sSdEjectCallback(sSdDriver.drive);
        }
    }

    return 0;
}

s32 pfd_sddrv_init(PDM_DISK* p_disk) {
    ISD_HANDLE* p_handle;
    u32 status;
    s32 result;

    if (p_disk == NULL) {
        return -30;
    }

    if (sSdDriver.status & SD_DRIVER_INITIALIZED) {
        OSReport("INFO SD Card driver is already initialize. pfd_sddrv_init()\n");
        if (p_disk != sSdDriver.p_disk) {
            return -44;
        }

        return 0;
    }

    if (!(sSdDriver.status & SD_DRIVER_ISD_READY)) {
        sSdDriver.sector_size = 512;
        pf_memset(&sSdHandle, 0, sizeof(ISD_HANDLE));
        if (ISD_InitCard() != 0) {
            OSReport("ERR:Failed to init SD Card Driver in pfd_sddrv_init()\n");
            return -40;
        }

        sSdDriver.status |= SD_DRIVER_ISD_READY;
    }

    pf_memset(&sSdHandle, 0, sizeof(ISD_HANDLE));
    p_handle = &sSdHandle;
    result = ISD_MountCard(0, &p_handle);
    if (result != 0) {
        OSReport("ERR SD card can not mount [ret = 0x%x]. pfd_sddrv_init()\n", result);
        return -41;
    }

    result = ISD_GetDeviceStatus(p_handle, &status);
    if (result != 0) {
        OSReport("ERR Failed to get sd card status. [ret = 0x%x]\n", result);
        return PDM_ERR_DRIVER;
    }

    if (status & ISD_STATUS_INSERTED) {
        sSdDriver.inserted = 1;
    }

    sSdDriver.p_handle = p_handle;
    if (sSdDriver.inserted) {
        sSdIntrEvent = ISD_EVENT_REMOVE;
        result = ISD_RegisterDeviceIntrHandler(p_handle, pfd_st_removal_callback, &sSdIntrEvent);
        if (result != 0) {
            OSReport("ERR:Failed to regist intr handler1 [ret = 0x%x] pfd_sddrv_init()\n", result);
            ISD_UnmountCard(p_handle);
            sSdDriver.p_handle = NULL;
            return -42;
        }
    } else {
        sSdIntrEvent = ISD_EVENT_INSERT;
        result = ISD_RegisterDeviceIntrHandler(p_handle, pfd_st_inter_callback, &sSdIntrEvent);
        if (result != 0) {
            OSReport("ERR:Failed to regist intr handler1 [ret = 0x%x] pfd_sddrv_init()\n", result);
            ISD_UnmountCard(p_handle);
            sSdDriver.p_handle = NULL;
            return -42;
        }
    }

    sSdDriver.p_disk = p_disk;
    sSdDriver.status |= SD_DRIVER_INITIALIZED;
    return 0;
}

s32 pfd_sddrv_mount(PDM_DISK* p_disk) {
    u16 rca;
    u32 ocr;
    u8 cid[16];
    u32 retry;
    s32 result;

    if (p_disk == NULL) {
        return -30;
    }

    if (!sSdDriver.inserted) {
        return -33;
    }

    if (sSdDriver.status & SD_DRIVER_MOUNTED) {
        OSReport("INFO sdcard alrady mounted. pfd_sddrv_mount()\n");
        return 0;
    }

    for (retry = 0; retry < SD_MOUNT_RETRIES; retry++) {
        if (!sSdDriver.inserted) {
            return -33;
        }

        result = ISD_ResetDevice(sSdDriver.p_handle);
        if (result == 0) {
            sSdDriver.media_changed = 0;
            break;
        }

        OSReport("ERROR Failed to SD Card Reset [ret = 0x%x]. pfd_sddrv_mount()\n", result);
    }

    if (retry == SD_MOUNT_RETRIES) {
        return -38;
    }

    if (sSdDriver.media_changed) {
        return -33;
    }

    ocr = 0;
    if (ISD_ReadCardRegister(sSdDriver.p_handle, ISD_CMD_SD_SEND_OP_COND, &ocr, sizeof(ocr)) != 0) {
        OSReport("ERR Failed to read OCR reg. pfd_sddrv_mount()\n");
        return -43;
    }

    pf_memset(cid, 0, sizeof(cid));
    if (ISD_ReadCardRegister(sSdDriver.p_handle, ISD_CMD_SEND_CID, (u32*)cid, sizeof(cid)) != 0) {
        OSReport("ERR Failed to read CID reg. pfd_sddrv_mount()\n");
        return -43;
    }

    rca = 0;
    if (ISD_ReadCardRegister(sSdDriver.p_handle, ISD_CMD_SEND_RELATIVE_ADDR, (u32*)&rca, sizeof(rca)) != 0) {
        OSReport("ERR Failed to read RCA reg. pfd_sddrv_mount()\n");
        return -43;
    }

    result = ISD_GetCardSize(sSdDriver.p_handle, NULL, NULL, NULL);
    if (result != 0) {
        OSReport("ERR Failed to get SD CardSize. [ret = %d].\r\n"
                 "                              pfd_sddrv_mount()\n",
                 result);
        return PDM_ERR_DRIVER;
    }

    sSdDriver.status |= SD_DRIVER_MOUNTED;
    return 0;
}

s32 pfd_sddrv_format(PDM_DISK* p_disk, const u8* p_param) {
    s32 result;

    if (p_disk == NULL) {
        return -30;
    }

    if (!sSdDriver.inserted) {
        return -33;
    }

    if (p_param != NULL && pf_strcmp((const s8*)p_param, (const s8*)"FULL_FORMAT") == 0) {
        result = pfd_sddrv_full_format();
        if (result != 0) {
            OSReport("ERR Failed to full format. pfd_sddrv_full_format()\n");
            return result;
        }
    }

    return 0;
}

s32 pfd_sddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success) {
    if (p_disk == NULL || p_buf == NULL || p_success == NULL) {
        return -30;
    }

    return pfd_sddrv_physical_read(count, p_buf, block, sSdDriver.sector_size, p_success);
}

s32 pfd_sddrv_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success) {
    if (p_disk == NULL || p_buf == NULL || p_success == NULL) {
        return -30;
    }

    return pfd_sddrv_physical_write(count, p_buf, block, sSdDriver.sector_size, p_success);
}

s32 pfd_sddrv_unmount(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -30;
    }

    if (sSdDriver.status & SD_DRIVER_MOUNTED) {
        sSdDriver.status &= ~SD_DRIVER_MOUNTED;
    }

    return 0;
}

s32 pfd_sddrv_finalize(PDM_DISK* p_disk) {
    s32 result;

    if (p_disk == NULL) {
        return -30;
    }

    if (sSdDriver.status & SD_DRIVER_MOUNTED) {
        sSdDriver.status &= ~SD_DRIVER_MOUNTED;
    }

    if (sSdDriver.status & SD_DRIVER_INITIALIZED) {
        result = ISD_UnregisterDeviceIntrHandler(sSdDriver.p_handle);
        if (result != 0) {
            OSReport("WARNING Faild to UnregisterDeviceIntrHandler sd card [ret = %d]\n", result);
        }

        result = ISD_UnmountCard(sSdDriver.p_handle);
        if (result != 0) {
            OSReport("WARNING Faild to unmount sd card [ret = %d]\n", result);
        }

        sSdDriver.p_handle = NULL;
    }

    sSdDriver.status &= ~SD_DRIVER_INITIALIZED;
    sSdDriver.inserted = 0;
    sSdDriver.p_disk = NULL;
    sSdDriver.drive = 0;
    return 0;
}

s32 pfd_sddrv_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info) {
    u32 status;
    s32 result;
    if (p_disk == NULL || p_info == NULL) {
        return -30;
    }
    if (!(sSdDriver.status & SD_DRIVER_MOUNTED)) {
        return -45;
    }
    if (sSdDriver.media_changed) {
        return -45;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    result = ISD_GetDeviceStatus(sSdDriver.p_handle, &status);
    if (result != 0) {
        OSReport("ERR Failed to get sd card status. [ret = 0x%x]\n", result);
        return -39;
    }
    if (!(status & 1)) {
        OSReport("INFO Card not inserted! in pfd_sddrv_get_disk_info()\n");
        return -33;
    }
    if (status & 2) {
        OSReport("INFO Card removed! in pfd_sddrv_get_disk_info()\n");
        return -33;
    }
    if (!(status & 0x10000) && !(status & 0x100000)) {
        OSReport("INFO SD card type is not memory. in pfd_sddrv_get_disk_info()\n");
        return -35;
    }
    p_info->media_attr = 4;
    if (status & 4) {
        p_info->media_attr |= 1;
    }
    result = pfd_sddrv_get_total_sectors(&p_info->total_sectors, &p_info->bytes_per_sector);
    if (result != 0) {
        OSReport("ERR Failed to read master boot sector. pfd_sddrv_get_disk_info()\n");
        return result;
    }
    p_info->cylinders = 0;
    p_info->heads = 0;
    p_info->sectors_per_track = 0;
    p_info->format_param = NULL;
    return 0;
}

s32 pfd_sddrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension) {
    p_table->ui_ext = extension;
    p_table->p_func = (PDM_FUNCTBL*)&sSdFunctions;
    return 0;
}

s32 pfd_sddrv_registar_callback(PFD_SD_MEDIA_CALLBACK insert, PFD_SD_MEDIA_CALLBACK eject) {
    sSdInsertCallback = insert;
    sSdEjectCallback = eject;
    return 0;
}

s32 pfd_sddrv_is_media_insert(PDM_DISK* p_disk) {
    char drive;
    if (sSdDriver.status & 1) {
        if (sSdDriver.drive == 0 && pfd_get_media_drv_char(sSdDriver.p_disk, &drive, 1) == 1) {
            sSdDriver.drive = drive;
        }
        if (sSdDriver.inserted) {
            pdm_disk_notify_media_insert(sSdDriver.p_disk);
            return 1;
        }
        return 0;
    }
    return 0;
}

s32 pfd_sddrv_physical_read(u32 count, u8* p_buf, u32 block, u32 sector_size, u32* p_success) {
    u32 current_block;
    u32 i;
    s32 result;
    if (!(sSdDriver.status & SD_DRIVER_MOUNTED)) {
        *p_success = 0;
        return -45;
    }
    if (sSdDriver.media_changed) {
        return -45;
    }
    if (count == 0) {
        *p_success = 0;
        return -30;
    }
    current_block = block;
    *p_success = -1;
    if (!((u32)p_buf & 31)) {
        if (!sSdDriver.inserted) {
            *p_success = 0;
            return -33;
        }
        if (count == 1) {
            result = ISD_ReadBlock(sSdDriver.p_handle, current_block, p_buf, count);
            if (result != 0) {
                OSReport("INFO Failed to read SD card1 [ret = 0x%x] pfd_sddrv_physical_read()\n", result);
                *p_success = 0;
                return -37;
            }
        } else {
            result = ISD_ReadMultiBlock(sSdDriver.p_handle, current_block, p_buf, count);
            if (result != 0) {
                OSReport("INFO Failed to read SD card2 [ret = 0x%x] pfd_sddrv_physical_read()\n", result);
                *p_success = 0;
                return -37;
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (!sSdDriver.inserted) {
                if (i != 0) {
                    *p_success = i;
                    return 0;
                }
                *p_success = 0;
                return -33;
            }
            result = ISD_ReadBlock(sSdDriver.p_handle, current_block, sSdSectorBuffer, 1);
            if (result != 0) {
                OSReport("INFO Failed to read SD card3 [ret = 0x%x] pfd_sddrv_physical_read()\n", result);
                if (i != 0) {
                    *p_success = i;
                    return 0;
                }
                *p_success = 0;
                return -37;
            }
            pf_memcpy(p_buf, sSdSectorBuffer, sector_size);
            p_buf += sector_size;
            current_block++;
        }
    }
    *p_success = count;
    return 0;
}

s32 pfd_sddrv_physical_write(u32 count, const u8* p_buf, u32 block, u32 sector_size, u32* p_success) {
    u32 current_block;
    u32 i;
    s32 result;
    if (!(sSdDriver.status & SD_DRIVER_MOUNTED)) {
        *p_success = 0;
        return -45;
    }
    if (sSdDriver.media_changed) {
        return -45;
    }
    if (count == 0) {
        *p_success = 0;
        return -30;
    }
    current_block = block;
    *p_success = -1;
    if (!((u32)p_buf & 31)) {
        if (!sSdDriver.inserted) {
            *p_success = 0;
            return -33;
        }
        if (count == 1) {
            result = ISD_WriteBlock(sSdDriver.p_handle, current_block, (u8*)p_buf, count);
            if (result != 0) {
                OSReport("INFO Failed to write SD card1 [ret = 0x%x] pfd_sddrv_physical_write()\n", result);
                *p_success = 0;
                return -36;
            }
        } else {
            result = ISD_WriteMultiBlock(sSdDriver.p_handle, current_block, (u8*)p_buf, count);
            if (result != 0) {
                OSReport("INFO Failed to write SD card2 [ret = 0x%x] pfd_sddrv_physical_write()\n", result);
                *p_success = 0;
                return -36;
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (!sSdDriver.inserted) {
                if (i != 0) {
                    *p_success = i;
                    return 0;
                }
                *p_success = 0;
                return -33;
            }
            pf_memcpy(sSdSectorBuffer, p_buf, sector_size);
            result = ISD_WriteBlock(sSdDriver.p_handle, current_block, sSdSectorBuffer, 1);
            if (result != 0) {
                OSReport("INFO Failed to write SD card3 [ret = 0x%x] pfd_sddrv_physical_write()\n", result);
                if (i != 0) {
                    *p_success = i;
                    return 0;
                }
                *p_success = 0;
                return -36;
            }
            p_buf += sector_size;
            current_block++;
        }
    }
    *p_success = count;
    return 0;
}

s32 pfd_sddrv_get_total_sectors(u32* p_total, u16* p_sector_size) {
    u32 csd[4];
    u32 card_size;
    u32 multiplier;
    u16 sector_factor;
    u16 read_length;
    u16 blocks_per_cluster;
    if (!sSdDriver.inserted) {
        return -33;
    }
    pf_memset(csd, 0, sizeof(csd));
    if (ISD_ReadCardRegister(sSdDriver.p_handle, 9, csd, sizeof(csd)) != 0) {
        OSReport("ERR Failed to get CSD Info. pfd_sddrv_get_total_sectors()\n");
        return -43;
    }
    if (!(csd[3] & 0x00400000)) {
        multiplier = (csd[1] >> 7) & 7;
        card_size = ((csd[2] << 10) & 0xC00) | (csd[1] >> 22);
        read_length = (csd[2] >> 8) & 15;
        blocks_per_cluster = 1 << (multiplier + 2);
        card_size = (card_size + 1) * blocks_per_cluster;
        read_length = read_length < 9 ? 9 : read_length;
        read_length = read_length > 11 ? 11 : read_length;
        sector_factor = read_length - 9;
        sector_factor = 1 << sector_factor;
        *p_total = card_size * sector_factor;
    } else {
        *p_total = (((csd[1] >> 8) & 0x3FFFFF) + 1) << 10;
    }
    *p_sector_size = 512;
    return 0;
}

s32 pfd_sddrv_calc_mbr_bpb(SD_FORMAT_PARAMETERS* p_parameters) {
    SD_FORMAT_GEOMETRY geometry;
    OSCalendarTime calendar;
    s32 result;
    u32 clusterSectors;
    u32 boundary;
    u32 alignmentSectors;
    u32 totalSectors;
    u32 fatSectors;
    s32 clusterCount;
    u32 fatBits;
    u32 requiredFatSectors;
    u32 overhead;
    u32 alignmentLimit;
    u32 units;
    s32 partitionStart;
    BOOL growing = FALSE;
    if (p_parameters == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    clusterSectors = geometry.sectorsPerCluster;
    totalSectors = p_parameters->totalSectors;
    p_parameters->sectorsPerCluster = clusterSectors;
    clusterCount = totalSectors / clusterSectors;
    if (clusterCount < 4101U) {
        fatBits = 12;
    } else if (clusterCount < 65525U) {
        fatBits = 16;
    } else {
        return -31;
    }
    if (totalSectors % clusterSectors) {
        clusterCount++;
    }
    requiredFatSectors = clusterCount * fatBits;
    fatSectors = requiredFatSectors / 4096;
    if (requiredFatSectors % 4096) {
        fatSectors++;
    }
    alignmentSectors = geometry.alignmentSectors;
recalculateFat:
    overhead = 2 * fatSectors + 33;
    alignmentLimit = alignmentSectors;
    units = 1;
searchBoundary:
    partitionStart = alignmentLimit - overhead;
    if (partitionStart <= 0) {
        alignmentLimit += alignmentSectors;
        units++;
        goto searchBoundary;
    }
    boundary = units * alignmentSectors;
retryPartition:
    if (!growing && alignmentSectors != partitionStart) {
        partitionStart += alignmentSectors;
    }
    growing = FALSE;
    clusterCount = pfd_sddrv_count_data_clusters(totalSectors - partitionStart - overhead, clusterSectors);
    if (clusterCount >= 4069U && clusterCount < 4101U) {
        boundary += alignmentSectors;
        partitionStart = boundary - overhead;
        goto retryPartition;
    }
    requiredFatSectors = (clusterCount - 1) * fatBits + 2;
    clusterCount = requiredFatSectors / 4096;
    if (requiredFatSectors % 4096) {
        clusterCount++;
    }
    if (clusterCount > fatSectors) {
        partitionStart += alignmentSectors;
        growing = TRUE;
        goto retryPartition;
    }
    if (clusterCount != fatSectors) {
        fatSectors = clusterCount;
        goto recalculateFat;
    }
    p_parameters->sectorsPerFat = fatSectors;
    p_parameters->partitionStart = partitionStart;
    p_parameters->partitionSectors = p_parameters->totalSectors - partitionStart;
    clusterCount = pfd_sddrv_count_format_clusters(p_parameters, partitionStart, fatSectors);
    if (clusterCount <= 4084U) {
        p_parameters->fatType = 0;
    } else {
        p_parameters->fatType = 1;
    }
    OSTicksToCalendarTime(OSGetTime(), &calendar);
    p_parameters->volumeId = (((calendar.mday & 31) | (calendar.mon & 15) << 5 | ((calendar.year - 1900) & 127) << 9) << 16) +
        ((calendar.sec & 31) | (calendar.min & 63) << 5 | (calendar.hour & 31) << 11);
    return 0;
}

#define SD_BPB_LE16(buffer, offset, value) do { \
    if ((u32)((buffer) + (offset)) & 1) { \
        (buffer)[offset] = (value); \
        (buffer)[(offset) + 1] = (value) >> 8; \
    } else { \
        ((u16*)(buffer))[(offset) / 2 + (offset) % 2] = PDM_SWAP16(value); \
    } \
} while (0)

#define SD_BPB_LE32(buffer, offset, value) do { \
    if ((u32)((buffer) + (offset)) & 3) { \
        (buffer)[offset] = (value); \
        (buffer)[(offset) + 1] = (value) >> 8; \
        (buffer)[(offset) + 2] = (value) >> 16; \
        (buffer)[(offset) + 3] = (value) >> 24; \
    } else { \
        ((u32*)(buffer))[(offset) / 4 + (offset) % 4] = PDM_SWAP32(value); \
    } \
} while (0)

static inline void pfd_sddrv_copy_format_field(u8* p_dest, const u8* p_source, u32 count) {
    u32 index;
    if (p_dest == NULL || p_source == NULL) {
        return;
    }
    for (index = 0; index < count; index++) {
        p_dest[index] = p_source[index];
    }
}

s32 pfd_sddrv_store_bpb_buf(SD_FORMAT_PARAMETERS* p_parameters, u8* p_buffer) {
    SD_FORMAT_GEOMETRY geometry;
    s32 result;
    u16 totalSectors16;
    u32 totalSectors32;
    if (p_parameters == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    totalSectors32 = p_parameters->partitionSectors;
    if (totalSectors32 != 0 && totalSectors32 < 65536U) {
        totalSectors16 = totalSectors32;
        totalSectors32 = 0;
    } else if (totalSectors32 >= 65536U) {
        totalSectors16 = 0;
    } else {
        return -30;
    }
    pf_memset(p_buffer, 0, 512);
    pfd_sddrv_copy_format_field(p_buffer + 3, (const u8*)"        ", 8);
    pfd_sddrv_copy_format_field(p_buffer + 43, (const u8*)"NO NAME    ", 11);
    if (p_parameters->fatType == 0) {
        pfd_sddrv_copy_format_field(p_buffer + 54, (const u8*)"FAT12   ", 8);
    } else {
        pfd_sddrv_copy_format_field(p_buffer + 54, (const u8*)"FAT16   ", 8);
    }
    p_buffer[0] = 0xEB;
    p_buffer[1] = 0;
    p_buffer[2] = 0x90;
    SD_BPB_LE16(p_buffer, 11, 512);
    p_buffer[13] = p_parameters->sectorsPerCluster;
    SD_BPB_LE16(p_buffer, 14, 1);
    p_buffer[16] = 2;
    SD_BPB_LE16(p_buffer, 17, 512);
    SD_BPB_LE16(p_buffer, 19, totalSectors16);
    p_buffer[21] = 0xF8;
    SD_BPB_LE16(p_buffer, 22, p_parameters->sectorsPerFat);
    SD_BPB_LE16(p_buffer, 24, geometry.sectorsPerTrack);
    SD_BPB_LE16(p_buffer, 26, geometry.heads);
    SD_BPB_LE32(p_buffer, 28, p_parameters->partitionStart);
    SD_BPB_LE32(p_buffer, 32, totalSectors32);
    p_buffer[36] = 0x80;
    p_buffer[38] = 0x29;
    SD_BPB_LE32(p_buffer, 39, p_parameters->volumeId);
    p_buffer[510] = 0x55;
    p_buffer[511] = 0xAA;
    return 0;
}

#define SD_STORE_LE16(buffer, offset, value) do { \
    if ((u32)((buffer) + (offset)) & 1) { \
        (buffer)[offset] = (value); \
        (buffer)[(offset) + 1] = (value) >> 8; \
    } else { \
        *(u16*)((buffer) + (offset)) = ((u16)((((value) & 0xFF) << 8) | (((value) >> 8) & 0xFF))); \
    } \
} while (0)

static inline u8 pfd_sddrv_get_partition_type(u32 partitionSectors) {
    if (partitionSectors != 0 && partitionSectors < 32680U) {
        return 1;
    } else if (partitionSectors >= 32680U && partitionSectors < 65536U) {
        return 4;
    }
    return 6;
}

s32 pfd_sddrv_store_mbr_buf(SD_FORMAT_PARAMETERS* p_parameters, u8* p_buffer) {
    SD_FORMAT_GEOMETRY geometry;
    s32 result;
    u8 partitionType;
    u8 firstHead;
    u16 firstSector;
    u16 firstCylinder;
    u8 lastHead;
    u16 lastSector;
    u16 lastCylinder;
    u32 sectorsPerTrack;
    u32 firstBlock;
    u32 sectorsPerCylinder;
    u32 lastBlock;
    s32 partitionSectors;
    if (p_parameters == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    sectorsPerTrack = geometry.sectorsPerTrack;
    sectorsPerCylinder = geometry.heads * sectorsPerTrack;
    firstBlock = p_parameters->partitionStart;
    lastBlock = p_parameters->totalSectors - 1;
    partitionSectors = p_parameters->partitionSectors;
    firstCylinder = firstBlock / sectorsPerCylinder;
    firstHead = (firstBlock % sectorsPerCylinder) / sectorsPerTrack;
    firstSector = firstBlock % sectorsPerTrack + 1;
    lastCylinder = lastBlock / sectorsPerCylinder;
    lastHead = (lastBlock % sectorsPerCylinder) / sectorsPerTrack;
    lastSector = lastBlock % sectorsPerTrack + 1;
    if (partitionSectors == 0) {
        return -30;
    }
    partitionType = pfd_sddrv_get_partition_type(partitionSectors);
    pf_memset(p_buffer, 0, 512);
    p_buffer[446] = 0;
    p_buffer[447] = firstHead;
    SD_STORE_LE16(p_buffer, 448, ((firstCylinder & 0xFF) << 8) + ((firstSector & 0x3F) | (firstCylinder & 0x300) >> 2));
    p_buffer[450] = partitionType;
    p_buffer[451] = lastHead;
    SD_STORE_LE16(p_buffer, 452, ((lastCylinder & 0xFF) << 8) + ((lastSector & 0x3F) | (lastCylinder & 0x300) >> 2));
    if ((u32)(p_buffer + 454) & 3) {
        p_buffer[454] = p_parameters->partitionStart;
        p_buffer[455] = p_parameters->partitionStart >> 8;
        p_buffer[456] = p_parameters->partitionStart >> 16;
        p_buffer[457] = p_parameters->partitionStart >> 24;
    } else {
        *(u32*)(p_buffer + 460) = PDM_SWAP32(p_parameters->partitionStart);
    }
    if ((u32)(p_buffer + 458) & 3) {
        p_buffer[458] = p_parameters->partitionSectors;
        p_buffer[459] = p_parameters->partitionSectors >> 8;
        p_buffer[460] = p_parameters->partitionSectors >> 16;
        p_buffer[461] = p_parameters->partitionSectors >> 24;
    } else {
        *(u32*)(p_buffer + 464) = PDM_SWAP32(p_parameters->partitionSectors);
    }
    p_buffer[510] = 0x55;
    p_buffer[511] = 0xAA;
    return 0;
}

s32 pfd_sddrv_build_mbr_bpb(u32 totalSectors) {
    SD_FORMAT_PARAMETERS parameters;
    s32 result;
    u32 partitionStart;
    pf_memset(&parameters, 0, sizeof(parameters));
    parameters.totalSectors = totalSectors;
    result = pfd_sddrv_calc_mbr_bpb(&parameters);
    if (result != 0) {
        OSReport("ERR Failed to calculate MBR and BPB values. pfd_sddrv_calc_mbr_bpb()\n");
        return result;
    }
    result = pfd_sddrv_store_bpb_buf(&parameters, sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store BPB values to buf. pfd_sddrv_store_bpb_buf()\n");
        return result;
    }
    partitionStart = parameters.partitionStart;
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, partitionStart, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    result = pfd_sddrv_store_mbr_buf(&parameters, sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store MBR values to buf. pfd_sddrv_store_mbr_buf()\n");
        return result;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, 0, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write MBR fields. ISD_WriteBlock()\n");
        return -36;
    }
    return 0;
}

s32 pfd_sddrv_calc_fat32_mbr_bpb(SD_FORMAT_PARAMETERS* p_parameters) {
    SD_FORMAT_GEOMETRY geometry;
    OSCalendarTime calendar;
    s32 result;
    u32 clusterSectors;
    u32 fatExtent;
    u32 availableSectors;
    u32 fatSectors;
    u32 usedSectors;
    u32 clusterCount;
    u32 fatBits;
    u32 requiredFatSectors;
    u32 boundary;
    u32 totalSectors;
    s32 reservedSectors = 0;
    if (p_parameters == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    clusterSectors = geometry.sectorsPerCluster;
    totalSectors = p_parameters->totalSectors;
    p_parameters->sectorsPerCluster = clusterSectors;
    fatBits = (totalSectors / clusterSectors) * 32;
    fatSectors = fatBits / 4096;
    if (fatBits % 4096) {
        fatSectors++;
    }
    fatExtent = 2 * fatSectors;
    availableSectors = totalSectors - 8192;
    for (; fatSectors != 0; fatSectors--) {
        boundary = 8192;
        for (;;) {
            reservedSectors = boundary - fatExtent;
            if (reservedSectors > 0) {
                break;
            }
            boundary += 8192;
        }
        if ((u32)reservedSectors < 9) {
            reservedSectors += 8192;
        }
        usedSectors = reservedSectors + fatExtent;
        for (;;) {
            clusterCount = (availableSectors - usedSectors) / clusterSectors + 2;
            fatBits = clusterCount * 32;
            requiredFatSectors = fatBits / 4096;
            if (fatBits % 4096) {
                requiredFatSectors++;
            }
            if (requiredFatSectors <= fatSectors) {
                break;
            }
            reservedSectors += 8192;
            usedSectors += 8192;
        }
        if (requiredFatSectors == fatSectors) {
            break;
        }
        fatExtent -= 2;
    }
    if (fatSectors == 0) {
        return -31;
    }
    p_parameters->sectorsPerFat = fatSectors;
    p_parameters->reservedSectors = reservedSectors;
    p_parameters->partitionStart = 8192;
    p_parameters->partitionSectors = p_parameters->totalSectors - 8192;
    OSTicksToCalendarTime(OSGetTime(), &calendar);
    p_parameters->volumeId = (((calendar.mday & 31) | (calendar.mon & 15) << 5 | ((calendar.year - 1900) & 127) << 9) << 16) +
        ((calendar.sec & 31) | (calendar.min & 63) << 5 | (calendar.hour & 31) << 11);
    return 0;
}

s32 pfd_sddrv_store_fat32_mbr_buf(SD_FORMAT_PARAMETERS* p_parameters, u8* p_buffer) {
    SD_FORMAT_GEOMETRY geometry;
    s32 result;
    u8 partitionType;
    u8 firstHead;
    u16 firstSector;
    u16 firstCylinder;
    u8 lastHead;
    u16 lastSector;
    u16 lastCylinder;
    if (p_parameters == NULL || p_buffer == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    if (p_parameters->partitionStart > 0xFB0400) {
        firstHead = 254;
        firstSector = 63;
        firstCylinder = 1023;
    } else {
        firstCylinder = p_parameters->partitionStart / (geometry.heads * geometry.sectorsPerTrack);
        firstHead = (p_parameters->partitionStart % (geometry.heads * geometry.sectorsPerTrack)) / geometry.sectorsPerTrack;
        firstSector = p_parameters->partitionStart % geometry.sectorsPerTrack + 1;
    }
    if (p_parameters->totalSectors > 0xFB0400) {
        lastHead = 254;
        lastSector = 63;
        lastCylinder = 1023;
        partitionType = 12;
    } else {
        lastCylinder = (p_parameters->totalSectors - 1) / (geometry.heads * geometry.sectorsPerTrack);
        lastHead = ((p_parameters->totalSectors - 1) % (geometry.heads * geometry.sectorsPerTrack)) / geometry.sectorsPerTrack;
        lastSector = (p_parameters->totalSectors - 1) % geometry.sectorsPerTrack + 1;
        partitionType = 11;
    }
    pf_memset(p_buffer, 0, 512);
    p_buffer[446] = 0;
    p_buffer[447] = firstHead;
    SD_STORE_LE16(p_buffer, 448, ((firstCylinder & 0xFF) << 8) + ((firstSector & 0x3F) | (firstCylinder & 0x300) >> 2));
    p_buffer[450] = partitionType;
    p_buffer[451] = lastHead;
    SD_STORE_LE16(p_buffer, 452, ((lastCylinder & 0xFF) << 8) + ((lastSector & 0x3F) | (lastCylinder & 0x300) >> 2));
    if ((u32)(p_buffer + 454) & 3) {
        p_buffer[454] = p_parameters->partitionStart;
        p_buffer[455] = p_parameters->partitionStart >> 8;
        p_buffer[456] = p_parameters->partitionStart >> 16;
        p_buffer[457] = p_parameters->partitionStart >> 24;
    } else {
        *(u32*)(p_buffer + 460) = PDM_SWAP32(p_parameters->partitionStart);
    }
    if ((u32)(p_buffer + 458) & 3) {
        p_buffer[458] = p_parameters->partitionSectors;
        p_buffer[459] = p_parameters->partitionSectors >> 8;
        p_buffer[460] = p_parameters->partitionSectors >> 16;
        p_buffer[461] = p_parameters->partitionSectors >> 24;
    } else {
        *(u32*)(p_buffer + 464) = PDM_SWAP32(p_parameters->partitionSectors);
    }
    p_buffer[510] = 0x55;
    p_buffer[511] = 0xAA;
    return 0;
}

s32 pfd_sddrv_store_fat32_fsi_buf(u8* p_buffer) {
    pf_memset(p_buffer, 0, 512);
    SD_BPB_LE32(p_buffer, 0, 0x41615252);
    SD_BPB_LE32(p_buffer, 484, 0x61417272);
    SD_BPB_LE32(p_buffer, 488, 0xFFFFFFFF);
    SD_BPB_LE32(p_buffer, 492, 0xFFFFFFFF);
    SD_BPB_LE32(p_buffer, 508, 0xAA550000);
    return 0;
}

s32 pfd_sddrv_store_fat32_bpb_buf(SD_FORMAT_PARAMETERS* p_parameters, u8* p_buffer) {
    SD_FORMAT_GEOMETRY geometry;
    s32 result;
    if (p_parameters == NULL || p_buffer == NULL) {
        return -30;
    }
    pf_memset(&geometry, 0, sizeof(geometry));
    result = pfd_sddrv_get_value_with_total_sectors(p_parameters->totalSectors, &geometry);
    if (result != 0) {
        OSReport("ERR Failed to get values with total sectors. pfd_sddrv_get_value_with_total_sectors()\n");
        return result;
    }
    pf_memset(p_buffer, 0, 512);
    pfd_sddrv_copy_format_field(p_buffer + 3, (const u8*)"        ", 8);
    pfd_sddrv_copy_format_field(p_buffer + 71, (const u8*)"NO NAME    ", 11);
    pfd_sddrv_copy_format_field(p_buffer + 82, (const u8*)"FAT32   ", 8);
    p_buffer[0] = 0xEB;
    p_buffer[1] = 0;
    p_buffer[2] = 0x90;
    SD_BPB_LE16(p_buffer, 11, 512);
    p_buffer[13] = p_parameters->sectorsPerCluster;
    SD_BPB_LE16(p_buffer, 14, p_parameters->reservedSectors);
    p_buffer[16] = 2;
    SD_BPB_LE16(p_buffer, 17, 0);
    SD_BPB_LE16(p_buffer, 19, 0);
    p_buffer[21] = 0xF8;
    SD_BPB_LE16(p_buffer, 22, 0);
    SD_BPB_LE16(p_buffer, 24, geometry.sectorsPerTrack);
    SD_BPB_LE16(p_buffer, 26, geometry.heads);
    SD_BPB_LE32(p_buffer, 28, p_parameters->partitionStart);
    SD_BPB_LE32(p_buffer, 32, p_parameters->partitionSectors);
    SD_BPB_LE32(p_buffer, 36, p_parameters->sectorsPerFat);
    SD_BPB_LE16(p_buffer, 40, 0);
    SD_BPB_LE16(p_buffer, 42, 0);
    SD_BPB_LE32(p_buffer, 44, 2);
    SD_BPB_LE16(p_buffer, 48, 1);
    SD_BPB_LE16(p_buffer, 50, 6);
    p_buffer[64] = 0x80;
    p_buffer[66] = 0x29;
    SD_BPB_LE32(p_buffer, 67, p_parameters->volumeId);
    p_buffer[510] = 0x55;
    p_buffer[511] = 0xAA;
    return 0;
}

static inline s32 pfd_sddrv_store_fat32_reserved_buf(u8* p_buffer) {
    pf_memset(p_buffer, 0, 512);
    SD_BPB_LE16(p_buffer, 510, 0xAA55);
    return 0;
}

s32 pfd_sddrv_build_fat32_mbr_bpb(u32 totalSectors) {
    SD_FORMAT_PARAMETERS parameters;
    s32 result;
    u32 partitionStart;
    pf_memset(&parameters, 0, sizeof(parameters));
    parameters.totalSectors = totalSectors;
    result = pfd_sddrv_calc_fat32_mbr_bpb(&parameters);
    if (result != 0) {
        OSReport("ERR Failed to calculate MBR and BPB values. pfd_sddrv_calc_fat32_mbr_bpb()\n");
        return result;
    }
    result = pfd_sddrv_store_fat32_bpb_buf(&parameters, sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store BPB values to buf. pfd_sddrv_store_fat32_bpb_buf()\n");
        return result;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    partitionStart = parameters.partitionStart;
    if (ISD_WriteBlock(sSdDriver.p_handle, partitionStart, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, parameters.partitionStart + 6, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    result = pfd_sddrv_store_fat32_fsi_buf(sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store FSInfo values to buf. pfd_sddrv_store_fat32_fsi_buf()\n");
        return result;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, parameters.partitionStart + 1, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, parameters.partitionStart + 7, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    result = pfd_sddrv_store_fat32_reserved_buf(sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store reserved for boot sector values to buf. ");
        OSReport("pfd_sddrv_store_fat32_reserved_buf()\n");
        return result;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, parameters.partitionStart + 2, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, parameters.partitionStart + 8, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write BPB fields. ISD_WriteBlock()\n");
        return -36;
    }
    result = pfd_sddrv_store_fat32_mbr_buf(&parameters, sSdSectorBuffer);
    if (result != 0) {
        OSReport("ERR Failed to store MBR values to buf. pfd_sddrv_store_mbr_buf()\n");
        return result;
    }
    if (!sSdDriver.inserted) {
        return -33;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    if (ISD_WriteBlock(sSdDriver.p_handle, 0, sSdSectorBuffer, 1) != 0) {
        OSReport("ERR Failed to write MBR fields. ISD_WriteBlock()\n");
        return -36;
    }
    return 0;
}

s32 pfd_sddrv_full_format(void) {
    PDM_DISK_INFO diskInfo;
    u32 status;
    s32 result;
    u32 retry;
    for (retry = 0; retry < SD_MOUNT_RETRIES; retry++) {
        if (!sSdDriver.inserted) {
            return -33;
        }
        result = ISD_ResetDevice(sSdDriver.p_handle);
        if (result == 0) {
            sSdDriver.media_changed = 0;
            break;
        }
        OSReport("ERROR Failed to SD Card Reset [ret = 0x%x]. pfd_sddrv_full_format()\n", result);
    }
    if (retry == SD_MOUNT_RETRIES) {
        return -38;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    result = ISD_GetDeviceStatus(sSdDriver.p_handle, &status);
    if (result != 0) {
        OSReport("ERR Failed to get sd card status. [ret = 0x%x]\n", result);
        return -39;
    }
    if (status & 4) {
        return -32;
    }
    if (sSdDriver.media_changed) {
        return -33;
    }
    result = pfd_sddrv_get_total_sectors(&diskInfo.total_sectors, &diskInfo.bytes_per_sector);
    if (result != 0) {
        OSReport("ERR Failed to get total sectors. pfd_sddrv_get_total_sectors()\n");
        return result;
    }
    if ((status & 0x10000) && !(status & 0x100000)) {
        if (diskInfo.total_sectors > 0x400000) {
            return -34;
        }
        result = pfd_sddrv_build_mbr_bpb(diskInfo.total_sectors);
    } else if ((status & 0x10000) && (status & 0x100000)) {
        if (diskInfo.total_sectors > 0x4000000) {
            return -34;
        }
        result = pfd_sddrv_build_fat32_mbr_bpb(diskInfo.total_sectors);
    } else {
        return -31;
    }
    if (result != 0) {
        OSReport("ERR Failed to build up and write MBR and BPB fields.\n");
        return result;
    }
    return 0;
}
