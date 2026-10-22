#include <revolution/fa.h>
#include <revolution/fa/msc_drv.h>
#include <revolution/fa/nand_drv.h>
#include <revolution/fa/pdm_dskmng.h>
#include <revolution/fa/sd_drv.h>
#include <revolution/os.h>
#include <private/fa/pf_stub.h>
#include <string.h>

enum FADevice {
    FA_DEVICE_SD,
    FA_DEVICE_USB,
    FA_DEVICE_NAND,
    FA_DEVICE_USER,
    FA_DEVICE_COUNT
};

#define FA_ERROR -1
#define FA_ERROR_PARAMETER -2
#define FA_ERROR_QUEUE -3

PDM_DISK* gOpenDisk[26];
PDM_PARTITION* gOpenPartition[26];
FAMediaCallback gMediaInOutCallback[6];

static PDM_INIT_DISK sDriveInitTable[3] = {
    {pfd_sddrv_init_drv_tbl, 0},
    {pfd_mscdrv_init_drv_tbl, 0},
    {pfd_nanddrv_init_drv_tbl, 0},
};

static const char sVersion[] = "<< RVL_SDK - FA \trelease build: Mar  4 2009 15:07:30 (0x4302_145) >>";

static PDM_INIT_DISK* drvInitFunc = sDriveInitTable;
#pragma force_active on
s32 sDriveCount = 3;
#pragma force_active reset
static const char* __FAVersion = "<< RVL_SDK - FA 	release build: Mar  4 2009 15:07:30 (0x4302_145) >>";

static s32 FADiskInitialized;
static s32 FAInInitializing;
static s32 FAFileSysInitialized;

static inline PF_STUB_COMMAND* FAGetCommand(FACommandBlock* block) {
    return (PF_STUB_COMMAND*)block->asyncData;
}

s32 FAAttachAsync(u8 device, s8* nand_path, u32 nand_size, FADrvTbl* p_driver_table, FACallback callback, FACommandBlock* block) {
    PDM_DISK* disk;
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    if (device >= FA_DEVICE_COUNT) {
        return FA_ERROR_PARAMETER;
    }

    if (device == FA_DEVICE_NAND && nand_path == NULL) {
        return FA_ERROR_PARAMETER;
    }

    if (device == FA_DEVICE_USER && p_driver_table->p_part == NULL) {
        return FA_ERROR_PARAMETER;
    }

    if (device <= FA_DEVICE_NAND) {
        if (pdm_open_disk(&drvInitFunc[device], &disk) != 0) {
            return FA_ERROR;
        }

        if (pdm_open_partition(disk, 0, &p_driver_table->p_part) != 0) {
            return FA_ERROR;
        }
    }

    if (device == FA_DEVICE_NAND) {
        memcpy(block->nandInfo.nandFilePath, nand_path, 78);
        block->nandInfo.nandFileSize = nand_size;
    }

    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    p->first.device_type = device;
    p->driver_table[0] = p_driver_table;
    p->driver_table[1] = NULL;

    if (device == FA_DEVICE_NAND) {
        p->secondary.nand_file = (const struct PF_NAND_FILE*)&block->nandInfo;
    } else {
        p->secondary.nand_file = NULL;
    }

    if (device <= FA_DEVICE_NAND) {
        p->p_disk = disk;
        p->p_partition = p_driver_table->p_part;
    } else {
        p->p_disk = NULL;
        p->p_partition = NULL;
    }

    result = pfstub_attach(block);

    if (result == -1) {
        return FA_ERROR_QUEUE;
    }

    return result;
}

s32 FABuffering(s8 drive, s32 mode) {
    return pfstub_buffering(drive, mode) == 0 ? 0 : FA_ERROR;
}

s32 FAChdir(const char* path) {
    return pfstub_chdir(path) == 0 ? 0 : FA_ERROR;
}

s32 FACreateAsync(const char* path, s32 mode, FAFile** pp_file, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.path = path;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = (s32*)pp_file;
    p->first.mode = mode;
    result = pfstub_create(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FADetach(s8 drive) {
    PDM_DISK* disk = gOpenDisk[drive - 'A'];
    PDM_PARTITION* partition = gOpenPartition[drive - 'A'];

    if (pfstub_detach(drive) != 0) {
        return FA_ERROR;
    }

    if (disk != NULL && partition != NULL) {
        if (pdm_close_partition(partition) != 0) {
            return FA_ERROR;
        }

        if (pdm_close_disk(disk) != 0) {
            return FA_ERROR;
        }
    }

    return 0;
}

s32 FAErrnum(void) {
    s32 result;

    result = pfstub_errnum();

    if (result == 5) {
        return 5;
    }

    return result == 0 ? 0 : result;
}

s32 FAFclose(FAFile* p_file) {
    return pfstub_fclose(p_file) == 0 ? 0 : FA_ERROR;
}

s32 FAFsfirstAsync(const char* path, u8 attributes, FADta* p_dta, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.path = path;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    p->first.attributes = attributes;
    p->secondary.dta = (struct PF_DTA*)p_dta;
    result = pfstub_fsfirst(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FAFsnextAsync(FADta* p_dta, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.dta = (struct PF_DTA*)p_dta;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    result = pfstub_fsnext(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FAFwriteAsync(void* buffer, u32 size, u32 count, FAFile* p_file, u32* p_written, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.buffer = buffer;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = (s32*)p_written;
    p->first.size = size;
    p->second.size = count;
    p->secondary.file = p_file;
    result = pfstub_fwrite(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FAGetdev(s8 drive, struct PF_DEV_INF* p_info) {
    return pfstub_devinf(drive, p_info) == 0 ? 0 : FA_ERROR;
}

s32 FAInit(long config) {
    s32 result;

    if (FAInInitializing == 1) {
        return FA_ERROR;
    }

    FAInInitializing = 1;

    if (!FADiskInitialized) {
        result = pdm_init_diskmanager(0, NULL);
        FADiskInitialized = result == 0;

        if (result != 0) {
            FAInInitializing = 0;
            return FA_ERROR;
        }
    }

    if (!FAFileSysInitialized) {
        result = pfstub_init_prfile2(config, NULL);
        FAFileSysInitialized = result == 0;

        if (result == -1) {
            FAInInitializing = 0;
            return FA_ERROR;
        }
    }

    FAInInitializing = 0;
    OSRegisterVersion(__FAVersion);
    return 0;
}

s32 FAIsWriteProtected(s8 drive) {
    s32 result;

    result = pfstub_iswriteprotected(drive);

    if (result != 0 && result != 1) {
        return FA_ERROR;
    }

    return result;
}

s32 FAMkdirAsync(const char* path, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.path = path;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    result = pfstub_mkdir_async(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FAMountAsync(s8 drive, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->drive = drive;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    result = pfstub_mount_async(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FARegistCB(u8 device, FAMediaCallback inserted, FAMediaCallback ejected) {
    s32 result;

    switch (device) {
    case FA_DEVICE_SD:
        gMediaInOutCallback[0] = inserted;
        gMediaInOutCallback[1] = ejected;
        result = pfd_sddrv_registar_callback((PFD_SD_MEDIA_CALLBACK)FANotifySDInsert, (PFD_SD_MEDIA_CALLBACK)FANotifySDEject);
        break;
    case FA_DEVICE_USB:
        gMediaInOutCallback[2] = inserted;
        gMediaInOutCallback[3] = ejected;
        result = pfd_mscdrv_registar_callback(FANotifyUSBInsert, FANotifyUSBEject);
        break;
    default:
        return FA_ERROR_PARAMETER;
    }

    if (result != 0) {
        return FA_ERROR;
    }

    return 0;
}

void FANotifySDInsert(s8 drive) {
    if (gMediaInOutCallback[0] != NULL) {
        gMediaInOutCallback[0](drive);
    }
}

void FANotifySDEject(s8 drive) {
    if (gMediaInOutCallback[1] != NULL) {
        gMediaInOutCallback[1](drive);
    }
}

void FANotifyUSBInsert(s8 drive) {
    if (gMediaInOutCallback[2] != NULL) {
        gMediaInOutCallback[2](drive);
    }
}

void FANotifyUSBEject(s8 drive) {
    if (gMediaInOutCallback[3] != NULL) {
        gMediaInOutCallback[3](drive);
    }
}

s32 FARemove(const char* path) {
    return pfstub_remove(path) == 0 ? 0 : FA_ERROR;
}

s32 FARemoveAsync(const char* path, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.path = path;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    result = pfstub_remove_async(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FARmdirAsync(const char* path, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->primary.path = path;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    result = pfstub_rmdir_async(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}

s32 FASetupFSI(s8 drive, s16 flags) {
    return pfstub_setupfsi(drive, flags) == 0 ? 0 : FA_ERROR;
}

s32 FAUnmount(s8 drive, u32 mode) {
    s32 result;

    result = pfstub_unmount(drive, mode);

    if (result == -1) {
        return FA_ERROR;
    }

    if (result == 1) {
        return 1;
    }

    return result == 0 ? 0 : FA_ERROR;
}

s32 FAUnmountAsync(s8 drive, u32 mode, FACallback callback, FACommandBlock* block) {
    s32 result;
    PF_STUB_COMMAND* p = FAGetCommand(block);

    p->drive = drive;
    p->callback = (void (*)(s32, void*))callback;
    p->p_parameter = block;
    p->p_result = NULL;
    p->first.mode = mode;
    result = pfstub_unmount_async(block);
    return result == -1 ? FA_ERROR_QUEUE : result;
}
