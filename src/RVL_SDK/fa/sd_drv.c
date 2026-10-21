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
static u32 sSdIntrEvent;
static PFD_SD_MEDIA_CALLBACK sSdEjectCallback;
static PFD_SD_MEDIA_CALLBACK sSdInsertCallback;

static s32 pfd_st_inter_callback(s32 event, void* p_context);
static s32 pfd_st_removal_callback(s32 event, void* p_context);
s32 pfd_sddrv_physical_read(u32 count, u8* p_buf, u32 block, u32 sector_size, u32* p_success);
s32 pfd_sddrv_physical_write(u32 count, const u8* p_buf, u32 block, u32 sector_size, u32* p_success);
s32 pfd_sddrv_full_format(void);

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

    if (sSdDriver.p_handle != NULL) {
        sSdIntrEvent = ISD_EVENT_REMOVE;
        if (ISD_RegisterDeviceIntrHandler(sSdDriver.p_handle, pfd_st_removal_callback, &sSdIntrEvent) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_inter_callback()\n");
        }
    }

    if (sSdDriver.p_disk != NULL) {
        SdUpdateDrive();
        pdm_disk_notify_media_insert(sSdDriver.p_disk);
        if (sSdDriver.drive != 0 && sSdInsertCallback != NULL) {
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
    if (sSdDriver.p_handle != NULL) {
        sSdIntrEvent = ISD_EVENT_INSERT;
        if (ISD_RegisterDeviceIntrHandler(sSdDriver.p_handle, pfd_st_inter_callback, &sSdIntrEvent) != 0) {
            OSReport("ERR:Failed to regist intr handler. pfd_st_removal_callback()\n");
        }
    }

    if (sSdDriver.p_disk != NULL) {
        SdUpdateDrive();
        pdm_disk_notify_media_eject(sSdDriver.p_disk);
        if (sSdDriver.drive != 0 && sSdEjectCallback != NULL) {
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
