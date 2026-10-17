#include <revolution/fa/nand_drv.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_kernel.h>
#include <revolution/nand.h>

#define NAND_DEVICE_COUNT 23
#define NAND_DEVICE_MOUNTED 2
#define NAND_DEVICE_FORMATTING 4

typedef struct PFD_NAND_DEVICE {
    PDM_DISK* disk;
    s8 path[80];
    u32 size;
    NANDFileInfo file;
    u32 flags;
    u32 reserved;
    s32 last_error;
} PFD_NAND_DEVICE;

static s8 sNandWorkingDirectory[NAND_MAX_PATH];
static PFD_NAND_DEVICE sNandDevices[NAND_DEVICE_COUNT];
static PFK_SEMAPHORE_PARAMS sNandSemaphoreParams;
static OSSemaphore sNandSemaphore;
static OSSemaphore* sNandSemaphoreHandle;
static s32 sNandInitialized;
static s32 sNandDriverInitialized;

static s32 NandOpenDevice(PDM_DISK* p_disk, PFD_NAND_DEVICE* p_device, s32 force_format);
static s32 NandFormatDevice(PFD_NAND_DEVICE* p_device);
static s32 NandCreateDeviceFile(PFD_NAND_DEVICE* p_device);
static s32 NandGetFatType(u8* p_sector, u32* p_data_sectors);
static s32 NandBuildFsInfoSector(u8* p_sector) __attribute__((never_inline));
static s32 NandBuildBootSector(PFD_NAND_DEVICE* p_device, u8* p_sector, s32* p_fat_type, u32 data_sectors);
static s32 pfd_nanddrv_init(PDM_DISK* p_disk);
static s32 pfd_nanddrv_finalize(PDM_DISK* p_disk);
static s32 pfd_nanddrv_mount(PDM_DISK* p_disk);
static s32 pfd_nanddrv_unmount(PDM_DISK* p_disk);
static s32 pfd_nanddrv_format(PDM_DISK* p_disk, const u8* p_param);
static s32 pfd_nanddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success);
static s32 pfd_nanddrv_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success);
static s32 pfd_nanddrv_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info);

static const PDM_FUNCTBL sNandFunctions = {
    pfd_nanddrv_init,
    pfd_nanddrv_finalize,
    pfd_nanddrv_mount,
    pfd_nanddrv_unmount,
    pfd_nanddrv_format,
    pfd_nanddrv_pread,
    pfd_nanddrv_pwrite,
    pfd_nanddrv_get_disk_info
};

static inline s32 NandFindDevice(PDM_DISK* p_disk, PFD_NAND_DEVICE** pp_device) {
    u16 i;

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk) {
            *pp_device = &sNandDevices[i];
            break;
        }
    }

    return i == NAND_DEVICE_COUNT ? -22 : 0;
}

static inline PFD_NAND_DEVICE* NandFindAvailableDevice(PDM_DISK* p_disk) {
    u16 i;

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk && !(sNandDevices[i].flags & NAND_DEVICE_MOUNTED)) {
            return &sNandDevices[i];
        }
    }

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == NULL && sNandDevices[i].path[0] != 0) {
            return &sNandDevices[i];
        }
    }

    return NULL;
}

static inline void NandStoreLe16(u8* p_buf, u32 offset, u16 value) {
    if ((u32)(p_buf + offset) & 1) {
        p_buf[offset] = value;
        p_buf[offset + 1] = value >> 8;
    } else {
        *(u16*)(p_buf + ((offset + 1) & ~1)) = PDM_SWAP16(value);
    }
}

static inline void NandStoreLe32(u8* p_buf, u32 offset, u32 aligned_offset, u32 value) {
    if ((u32)(p_buf + offset) & 3) {
        p_buf[offset] = value;
        p_buf[offset + 1] = value >> 8;
        p_buf[offset + 2] = value >> 16;
        p_buf[offset + 3] = value >> 24;
    } else {
        *(u32*)(p_buf + aligned_offset) = PDM_SWAP32(value);
    }
}

static inline s32 NandSetLastError(PDM_DISK* p_disk, s32 error) {
    u16 i;

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk) {
            sNandDevices[i].last_error = error;
            break;
        }
    }

    return i == NAND_DEVICE_COUNT ? -22 : 0;
}

static inline void NandRecordError(const PDM_DISK* p_disk, s32 error) {
    u16 i;

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk) {
            sNandDevices[i].last_error = error;
            break;
        }
    }
}

static s32 NandOpenDevice(PDM_DISK* p_disk, PFD_NAND_DEVICE* p_device, s32 force_format) {
    s32 result;

    result = NANDChangeDir((char*)sNandWorkingDirectory);
    if (result != 0) {
        return result;
    }

    if (p_device->flags & NAND_DEVICE_FORMATTING) {
        result = 0;
    } else {
        result = NANDOpen((char*)p_device->path, &p_device->file, 3);
    }

    if (force_format != 0) {
        result = -12;
    }

    switch (result) {
    case -12:
        result = NandFormatDevice(p_device);
        if (result == 0) {
            p_device->disk = p_disk;
        }
        return result;
    case 0:
        p_device->disk = p_disk;
        return 0;
    default:
        return result;
    }
}

static s32 NandFormatDevice(PFD_NAND_DEVICE* p_device) {
    u8 zero_sector[512] ATTRIBUTE_ALIGN(32);
    u8 boot_sector[512] ATTRIBUTE_ALIGN(32);
    u8 fs_info[512] ATTRIBUTE_ALIGN(32);
    s32 fat_type;
    u32 data_sectors;
    s32 result;
    u32 remaining;

    result = NandCreateDeviceFile(p_device);
    if (result != 0) {
        return result;
    }

    pf_memset(boot_sector, 0, sizeof(boot_sector));
    data_sectors = 0;
    NandBuildBootSector(p_device, boot_sector, &fat_type, 0);
    if (fat_type != NandGetFatType(boot_sector, &data_sectors)) {
        NandBuildBootSector(p_device, boot_sector, &fat_type, data_sectors);
    }

    result = NANDWrite(&p_device->file, boot_sector, sizeof(boot_sector));
    if (result < 0) {
        NANDClose(&p_device->file);
        NANDDelete((char*)p_device->path);
        return result;
    }

    remaining = p_device->size - sizeof(boot_sector);
    if (fat_type == PDM_FAT_32) {
        NandBuildFsInfoSector(fs_info);
        result = NANDWrite(&p_device->file, fs_info, sizeof(fs_info));
        if (result < 0) {
            NANDClose(&p_device->file);
            NANDDelete((char*)p_device->path);
            return result;
        }

        remaining -= sizeof(fs_info);
    }

    pf_memset(zero_sector, 0, sizeof(zero_sector));
    while (remaining >= sizeof(zero_sector)) {
        result = NANDWrite(&p_device->file, zero_sector, sizeof(zero_sector));
        if (result < 0) {
            NANDClose(&p_device->file);
            NANDDelete((char*)p_device->path);
            return result;
        }

        remaining -= sizeof(zero_sector);
    }

    if (remaining != 0) {
        result = NANDWrite(&p_device->file, zero_sector, remaining);
        if (result < 0) {
            NANDClose(&p_device->file);
            NANDDelete((char*)p_device->path);
            return result;
        }
    }

    return 0;
}

static s32 NandCreateDeviceFile(PFD_NAND_DEVICE* p_device) {
    s8 component[16];
    u16 length = 0;
    s8* p_path;
    u16 processed = 0;
    s8* p_component;
    s32 result;

    if (p_device->flags & NAND_DEVICE_FORMATTING) {
        return 0;
    }

    p_path = p_device->path;
    pf_memset(component, 0, sizeof(component));
    p_component = component;

    for (;;) {
        switch (*p_path) {
        case '/':
            if (length == 0) {
                if (processed == 0) {
                    result = NANDChangeDir("/");
                    if (result != 0) {
                        return result;
                    }
                }
                p_path++;
                break;
            }

            if (p_path[1] == 0) {
                if (length > 12) {
                    return -20;
                }
                result = NANDCreate((char*)component, 0x30, 0);
                if (result == 0 || result == -6) {
                    NANDOpen((char*)component, &p_device->file, 3);
                    result = NANDChangeDir((char*)sNandWorkingDirectory);
                }
                return result;
            }

            if (length > 11) {
                return -20;
            }

            if (pf_strcmp(component, (const s8*)"..") != 0) {
                result = NANDCreateDir((char*)component, 0x30, 0);
            } else {
                result = 0;
            }
            if (result == 0 || result == -6 || result == -1) {
                result = NANDChangeDir((char*)component);
                if (result == 0) {
                    goto next_component;
                }
                return result;
            } else {
                return result;
            }

        next_component:
            p_path++;
            pf_memset(component, 0, sizeof(component));
            p_component = component;
            length = 0;
            break;
        case 0:
            if (length > 12) {
                return -20;
            }
            result = NANDCreate((char*)component, 0x30, 0);
            if (result == 0 || result == -6) {
                NANDOpen((char*)component, &p_device->file, 3);
                result = NANDChangeDir((char*)sNandWorkingDirectory);
            }
            return result;
        }

        processed++;
        *p_component++ = *p_path++;
        length++;
    }
}

static s32 NandGetFatType(u8* p_sector, u32* p_data_sectors) {
    u8 sector_shift = 0;
    u8 cluster_shift;
    u16 value = 512;
    u16 root_sectors;
    s32 root_bytes;
    u16 sectors_per_fat16;
    u32 sectors_per_fat;
    u32 first_data_sector;
    u32 total_sectors;
    u32 data_sectors;
    u32 clusters;
    PDM_FAT_TYPE fat_type;

    while ((value >>= 1) != 0) {
        sector_shift++;
    }
    cluster_shift = 0;
    value = 1;
    while ((value >>= 1) != 0) {
        cluster_shift++;
    }

    if ((u32)(p_sector + 17) & 1) {
        root_bytes = (u16)(p_sector[17] | (p_sector[18] << 8));
    } else {
        root_bytes = PDM_SWAP16(*(u16*)(p_sector + 18)) << 5;
    }
    root_sectors = (root_bytes + 511) >> sector_shift;
    sectors_per_fat16 = PDM_LOAD_LE16(p_sector, 22);
    sectors_per_fat = sectors_per_fat16;
    if (sectors_per_fat16 == 0) {
        sectors_per_fat = PDM_LOAD_LE32(p_sector + 36);
    }
    first_data_sector = root_sectors + sectors_per_fat * 2 + 1;
    total_sectors = PDM_LOAD_LE16(p_sector, 19);
    if (total_sectors == 0) {
        total_sectors = PDM_LOAD_LE32(p_sector + 32);
    }
    data_sectors = total_sectors - first_data_sector;
    clusters = data_sectors >> cluster_shift;
    *p_data_sectors = data_sectors;
    if (clusters < 4085) {
        fat_type = PDM_FAT_12;
    } else if (clusters < 65525) {
        fat_type = PDM_FAT_16;
    } else {
        fat_type = PDM_FAT_32;
    }
    return fat_type;
}

static s32 NandBuildFsInfoSector(u8* p_sector) {
    pf_memset(p_sector, 0, 512);
    NandStoreLe32(p_sector, 0, 0, 0x41615252);
    NandStoreLe32(p_sector, 484, 484, 0x61417272);
    NandStoreLe32(p_sector, 488, 488, 0xFFFFFFFF);
    NandStoreLe32(p_sector, 492, 492, 0xFFFFFFFF);
    NandStoreLe32(p_sector, 508, 508, 0xAA550000);
    return 0;
}

static s32 NandBuildBootSector(PFD_NAND_DEVICE* p_device, u8* p_sector, s32* p_fat_type, u32 data_sectors) {
    u32 clusters;
    u32 sectors_per_fat;
    s32 fat_type;

    if (data_sectors != 0) {
        clusters = data_sectors;
    } else {
        clusters = p_device->size >> 9;
    }

    if (clusters < 4085) {
        fat_type = PDM_FAT_12;
        sectors_per_fat = ((clusters * 3 / 2) + 511) >> 9;
    } else if (clusters < 65525) {
        fat_type = PDM_FAT_16;
        sectors_per_fat = (clusters * 2 + 511) >> 9;
    } else {
        fat_type = PDM_FAT_32;
        sectors_per_fat = (clusters * 4 + 511) >> 9;
    }

    pf_memset(p_sector, 0, 512);
    p_sector[0] = 0xEB;
    p_sector[2] = 0x90;
    p_sector[510] = 0x55;
    p_sector[511] = 0xAA;
    NandStoreLe16(p_sector, 24, 63);
    NandStoreLe16(p_sector, 26, 255);
    NandStoreLe16(p_sector, 11, 512);
    p_sector[13] = 1;
    NandStoreLe16(p_sector, 14, 1);
    p_sector[16] = 2;
    NandStoreLe16(p_sector, 17, 128);
    p_sector[21] = 0xF0;
    if (clusters < 65536) {
        NandStoreLe16(p_sector, 19, clusters);
        NandStoreLe32(p_sector, 32, 32, 0);
    } else {
        NandStoreLe16(p_sector, 19, 0);
        NandStoreLe32(p_sector, 32, 32, clusters);
    }

    if (fat_type == PDM_FAT_32) {
        NandStoreLe16(p_sector, 17, 0);
        NandStoreLe32(p_sector, 36, 36, sectors_per_fat);
        NandStoreLe16(p_sector, 40, 0);
        NandStoreLe16(p_sector, 42, 0);
        NandStoreLe32(p_sector, 44, 44, 2);
        NandStoreLe16(p_sector, 48, 1);
        p_sector[65] = 0;
        p_sector[66] = 0x29;
        NandStoreLe32(p_sector, 67, 76, 0x1234);
    } else {
        NandStoreLe16(p_sector, 17, 128);
        NandStoreLe16(p_sector, 22, sectors_per_fat);
        p_sector[37] = 0;
        p_sector[38] = 0x29;
        NandStoreLe32(p_sector, 39, 48, 0x1234);
    }

    *p_fat_type = fat_type;
    return 0;
}

static s32 pfd_nanddrv_init(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -20;
    }

    if (!sNandInitialized) {
        if (NANDInit() != 0) {
            return -21;
        }

        if (NANDGetCurrentDir((char*)sNandWorkingDirectory) != 0) {
            return -21;
        }

        sNandInitialized++;
    }

    pdm_disk_notify_media_insert(p_disk);
    return 0;
}

static s32 pfd_nanddrv_mount(PDM_DISK* p_disk) {
    PFD_NAND_DEVICE* p_device;
    s32 result;

    pfk_get_semaphore(sNandSemaphoreHandle);
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    if (NandFindDevice(p_disk, &p_device) == 0 && (p_device->flags & NAND_DEVICE_MOUNTED)) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -21;
    }

    p_device = NandFindAvailableDevice(p_disk);
    if (p_device == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -22;
    }

    if (p_device->disk != NULL) {
        result = 0;
    } else {
        result = NandOpenDevice(p_disk, p_device, 0);
    }

    if (result == 0) {
        p_device->flags |= NAND_DEVICE_MOUNTED;
    }

    pfk_release_semaphore(sNandSemaphoreHandle);
    return result;
}

static s32 pfd_nanddrv_format(PDM_DISK* p_disk, const u8* p_param) {
    PFD_NAND_DEVICE* p_device = NULL;
    s32 result;

    pfk_get_semaphore(sNandSemaphoreHandle);
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    if (NandFindDevice(p_disk, &p_device) != 0) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    p_device->flags |= NAND_DEVICE_FORMATTING;
    result = NandOpenDevice(p_disk, p_device, 1);
    p_device->flags &= ~NAND_DEVICE_FORMATTING;
    pfk_release_semaphore(sNandSemaphoreHandle);
    return result;
}

static s32 pfd_nanddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success) {
    s32 result;

    pfk_get_semaphore(sNandSemaphoreHandle);
    *p_success = 0;
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    if (p_buf == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    result = NandPhysicalRead(count, p_buf, block, 512, p_success, p_disk);
    if (result != 0) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return result;
    }

    pfk_release_semaphore(sNandSemaphoreHandle);
    return 0;
}

static s32 pfd_nanddrv_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success) {
    s32 result;

    pfk_get_semaphore(sNandSemaphoreHandle);
    *p_success = 0;
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    if (p_buf == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    result = NandPhysicalWrite(count, p_buf, block, 512, p_success, p_disk);
    if (result != 0) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return result;
    }

    pfk_release_semaphore(sNandSemaphoreHandle);
    return 0;
}

static s32 pfd_nanddrv_unmount(PDM_DISK* p_disk) {
    PFD_NAND_DEVICE* p_device = NULL;
    s32 result;
    s32 status;
    u32 position;

    pfk_get_semaphore(sNandSemaphoreHandle);
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    result = NandSetLastError(p_disk, 0);
    if (result == 0) {
        if (NandFindDevice(p_disk, &p_device) != 0) {
            pfk_release_semaphore(sNandSemaphoreHandle);
            return -20;
        }

        p_device->flags &= ~NAND_DEVICE_MOUNTED;
        status = NANDTell(&p_device->file, &position);
        if (status != 0) {
            result = status;
        } else {
            status = NANDClose(&p_device->file);
            if (status < 0) {
                result = status;
            } else {
                status = NANDOpen((char*)p_device->path, &p_device->file, 3);
                if (status < 0) {
                    result = status;
                } else {
                    status = NANDSeek(&p_device->file, position, 0);
                    result = status < 0 ? status : 0;
                }
            }
        }

        if (result < 0) {
            NandSetLastError(p_disk, result);
            pfk_release_semaphore(sNandSemaphoreHandle);
            return result;
        }
    }

    pfk_release_semaphore(sNandSemaphoreHandle);
    return result;
}

static s32 pfd_nanddrv_finalize(PDM_DISK* p_disk) {
    PFD_NAND_DEVICE* p_device;
    s32 result = 0;
    u16 i;

    pfk_get_semaphore(sNandSemaphoreHandle);
    if (p_disk == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        p_device = &sNandDevices[i];
        if (p_device->disk == p_disk) {
            result = NANDClose(&p_device->file);
            pf_memset(p_device, 0, sizeof(PFD_NAND_DEVICE));
            break;
        }
    }

    pfk_release_semaphore(sNandSemaphoreHandle);
    if (result != 0) {
        return -20;
    }

    return i == NAND_DEVICE_COUNT ? -22 : 0;
}

static s32 pfd_nanddrv_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info) {
    PFD_NAND_DEVICE* p_device = NULL;

    pfk_get_semaphore(sNandSemaphoreHandle);
    if (p_disk == NULL || p_info == NULL) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    if (NandFindDevice(p_disk, &p_device) != 0) {
        pfk_release_semaphore(sNandSemaphoreHandle);
        return -20;
    }

    p_info->total_sectors = p_device->size >> 9;
    p_info->cylinders = p_info->total_sectors / 255 / 63 / 512;
    p_info->heads = 255;
    p_info->sectors_per_track = 63;
    p_info->bytes_per_sector = 512;
    p_info->media_attr = 0;
    p_info->format_param = NULL;
    pfk_release_semaphore(sNandSemaphoreHandle);
    return 0;
}

s32 pfd_nanddrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension) {
    p_table->ui_ext = extension;
    p_table->p_func = (PDM_FUNCTBL*)&sNandFunctions;
    return 0;
}

s32 pfd_nanddrv_register_file(const s8* p_path, u32 size) {
    u16 i;

    if (pf_strlen(p_path) >= 76) {
        return -20;
    }

    if (!sNandDriverInitialized) {
        pf_memset(sNandDevices, 0, sizeof(sNandDevices));
        sNandDriverInitialized++;
        sNandSemaphoreParams.initial_count = 1;
        sNandSemaphoreParams.maximum_count = 1;
        sNandSemaphoreParams.semaphore = &sNandSemaphore;
        sNandSemaphoreHandle = pfk_create_semaphore(&sNandSemaphoreParams);
        if (sNandSemaphoreHandle == NULL) {
            return -21;
        }
    }

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (pf_strcmp(sNandDevices[i].path, p_path) == 0) {
            return 0;
        }

        if (sNandDevices[i].disk == NULL) {
            pf_strcpy(sNandDevices[i].path, p_path);
            sNandDevices[i].size = size;
            break;
        }
    }

    return i == NAND_DEVICE_COUNT ? -22 : 0;
}

s32 NandPhysicalRead(u32 count, u8* p_buf, u32 block, u32 sector_size, u32* p_success, const PDM_DISK* p_disk) {
    s32 result = 0;
    u16 i;
    u32 file_size;
    u32 offset;
    u32 remaining;
    PFD_NAND_DEVICE* p_device = NULL;
    u32 current_position;
    u8 sector[512] ATTRIBUTE_ALIGN(32);

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk) {
            p_device = &sNandDevices[i];
            break;
        }
    }
    result = i == NAND_DEVICE_COUNT ? -22 : 0;
    if (result != 0) {
        return -20;
    }

    remaining = count * sector_size;
    offset = block * sector_size;
    file_size = p_device->size;
    if (NANDTell(&p_device->file, &current_position) != 0 || current_position != offset) {
        result = NANDSeek(&p_device->file, offset, 0);
    } else {
        result = 0;
    }

    if (result >= 0) {
        if (offset + remaining > file_size) {
            remaining = file_size - offset;
            offset = 0;
            while (remaining >= sizeof(sector)) {
                result = NANDRead(&p_device->file, sector, sizeof(sector));
                if (result < 0) {
                    return -22;
                }
                pf_memcpy(p_buf + offset, sector, result);
                offset += result;
                remaining -= result;
            }
            if (remaining != 0) {
                result = NANDRead(&p_device->file, sector, remaining);
                if (result < 0) {
                    return -22;
                }
                pf_memcpy(p_buf + offset, sector, result);
            }
            return -22;
        }

        offset = 0;
        while (remaining >= sizeof(sector)) {
            result = NANDRead(&p_device->file, sector, sizeof(sector));
            if (result < 0) {
                NandRecordError(p_disk, result);
                return result;
            }
            pf_memcpy(p_buf + offset, sector, result);
            offset += result;
            remaining -= result;
        }
        if (remaining != 0) {
            result = NANDRead(&p_device->file, sector, remaining);
            if (result < 0) {
                NandRecordError(p_disk, result);
                return result;
            }
            pf_memcpy(p_buf + offset, sector, result);
        }
        *p_success = count;
        return 0;
    } else {
        NandRecordError(p_disk, result);
        return result;
    }
}

s32 NandPhysicalWrite(u32 count, const u8* p_buf, u32 block, u32 sector_size, u32* p_success, const PDM_DISK* p_disk) {
    s32 result = 0;
    u16 i;
    u32 file_size;
    u32 offset;
    u32 remaining;
    PFD_NAND_DEVICE* p_device = NULL;
    u32 current_position;
    u8 sector[512] ATTRIBUTE_ALIGN(32);

    for (i = 0; i < NAND_DEVICE_COUNT; i++) {
        if (sNandDevices[i].disk == p_disk) {
            p_device = &sNandDevices[i];
            break;
        }
    }
    result = i == NAND_DEVICE_COUNT ? -22 : 0;
    if (result != 0) {
        return -20;
    }

    remaining = count * sector_size;
    offset = block * sector_size;
    file_size = p_device->size;
    if (NANDTell(&p_device->file, &current_position) != 0 || current_position != offset) {
        result = NANDSeek(&p_device->file, offset, 0);
    } else {
        result = 0;
    }

    if (result >= 0) {
        if (offset + remaining > file_size) {
            remaining = file_size - offset;
            offset = 0;
            while (remaining >= sizeof(sector)) {
                pf_memcpy(sector, p_buf + offset, sizeof(sector));
                result = NANDWrite(&p_device->file, sector, sizeof(sector));
                if (result < 0) {
                    return -22;
                }
                remaining -= result;
                offset += result;
            }
            if (remaining != 0) {
                pf_memcpy(sector, p_buf + offset, remaining);
                NANDWrite(&p_device->file, sector, remaining);
            }
            return -22;
        }

        offset = 0;
        while (remaining >= sizeof(sector)) {
            pf_memcpy(sector, p_buf + offset, sizeof(sector));
            result = NANDWrite(&p_device->file, sector, sizeof(sector));
            if (result < 0) {
                NandRecordError(p_disk, result);
                return result;
            }
            remaining -= result;
            offset += result;
        }
        if (remaining != 0) {
            pf_memcpy(sector, p_buf + offset, remaining);
            result = NANDWrite(&p_device->file, sector, remaining);
            if (result < 0) {
                NandRecordError(p_disk, result);
                return result;
            }
        }
        *p_success = count;
        return 0;
    } else {
        NandRecordError(p_disk, result);
        return result;
    }
}
