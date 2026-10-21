#include <revolution/fa/isd.h>

#define SD_MAX_SLOTS 3

enum {
    SD_ERROR_SUCCESS = 0,
    SD_ERROR_FATAL = 0xC1000000,
    SD_ERROR_01000003 = 0xC1000003,
    SD_ERROR_21000000 = 0xC2100000
};

s32 ISD_GetHCRegister(ISD_HANDLE* dev, u32 reg, u32* value, u32 size) __attribute__((never_inline));
s32 ISD_ReadBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count);
s32 ISD_WriteBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count);

#include <revolution/os.h>
#include <revolution/os/OSMutex.h>

#include <revolution/ipc.h>

#include <string.h>

#define SD_SLOT_0_PATH "/dev/sdio/slot0"
#define SD_SLOT_1_PATH "/dev/sdio/slot1"
#define SD_SLOT_W_PATH "/dev/sdio/slotw"

#define SD_SLOT_PATH_LENGTH 32

typedef void (*SDCallbackFunc)(s32, void*);

#define SD_CMD_SIZE 0x24

u32 __sdCardStatus = 0;
u32 __sdReq = 0;

u32* __sdCmdBuffer = NULL;

u32* __sdResp = NULL;
u32* __sdResp2 = NULL;

u32* __sdReg = NULL;

IOSIoVector* __sdVect = NULL;

IOSHeapId __sdHeapId = -1;

char __sdPaths[SD_MAX_SLOTS][OSRoundUp32B(SD_SLOT_PATH_LENGTH)] ATTRIBUTE_ALIGN(32);
OSMutex __reqMutex;

typedef struct __sdCbArg {
    SDCallbackFunc cb;
    void* cbArg;
    ISD_HANDLE* dev;
    u32 command;
    u32 SDDevRca;
    u32* resp;
    u8* SDSectorSize;
} __sdCbArg;

enum {
    SD_IOCTL_2 = 2,
};

static IOSFd sduOpenFD(u32 slot);

static IOSError sduCommandv(s32 fd, u32 cmd, u32 cmdType, u32 respType, u32 arg, u32 buffer, u32 blockCount, u32 sectorSize, u32 transferMode, u32* resp,
                            void* cb, void* cbArg);
static s32 sduCommand(s32 fd, u32 cmd, u32 cmdType, u32 respType, u32 arg, u32 buffer, u32 blockCount, u32 sectorSize, u32 transferMode, u32* resp,
                            void* cb, void* cbArg);

static s32 sduDatabuswidth(ISD_HANDLE* dev, u32 buswidth) __attribute__((never_inline));
static s32 sduGetSCR(ISD_HANDLE* dev, u32* data);
static s32 sduGetOCR(ISD_HANDLE* dev, u32* data);

IOSError __sdCb(s32 result, void* arg) {
    __sdCbArg* data = (__sdCbArg*)arg;

    switch (data->command) {
        case 4: {
            if (result != 0) {
                OSReport("   +++ SD RESET Failed: %d +++\n", result);
                data->dev->rca = 0;
            } else {
                if (data->dev != NULL) {
                    data->dev->rca = *data->resp;
                    __sdCardStatus = 0x10000;
                    data->dev->opened = 2;
                }
            }

            __sdReq = 0;
            break;
        }
        case 25:
        case 18:
        case 38: {
            __sdReq = 0;
            break;
        }
    }

    if (data->cb != NULL && result != SD_ERROR_21000000) {
        data->cb(result, data->cbArg);
    }

    if (data->SDSectorSize != NULL) {
        iosFree(__sdHeapId, data->SDSectorSize);
    }
    iosFree(__sdHeapId, data);

    return result;
}

s32 ISD_GetHCRegister(ISD_HANDLE* dev, u32 address, u32* value, u32 registerSize) {
    IOSError ret;

    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }

    __sdReg[0] = address;
    __sdReg[3] = registerSize;
    __sdReg[4] = 0;

    if (value != NULL) {
        ret = IOS_Ioctl(dev->fd, SD_IOCTL_2, __sdReg, 0x18, __sdCmdBuffer, 4);
        if (ret != IOS_ERROR_OK) {
            return SD_ERROR_FATAL;
        }
        *value = *__sdCmdBuffer;
        return ret;
    }

    return SD_ERROR_FATAL;
}

s32 ISD_GetDeviceStatus(ISD_HANDLE* dev, u32* status) {
    IOSError ret = SD_ERROR_FATAL;

    if (__sdHeapId >= 0 && status != NULL) {
        OSLockMutex(&__reqMutex);

        if (__sdReq != 0) {
            OSUnlockMutex(&__reqMutex);
        } else {
            __sdReq = 1;
            OSUnlockMutex(&__reqMutex);

            ret = IOS_Ioctl(dev->fd, 11, NULL, 0, __sdCmdBuffer, 4);
            if (ret != IOS_ERROR_OK) {
                __sdReq = 0;
            } else {
                *status = *__sdCmdBuffer;
                __sdReq = 0;
            }
        }
    }

    return ret;
}

static IOSError sduCommandv(s32 fd, u32 cmd, u32 cmdType, u32 respType, u32 arg, u32 buffer, u32 blockCount, u32 sectorSize, u32 transferMode, u32* resp,
                            void* cb, void* cbArg) {
    IOSError ret;
    u32 readCount, writeCount;
    u32* sdCmd;

    if (transferMode != 0) {
        readCount = 2;
        writeCount = 1;
    } else {
        readCount = 1;
        writeCount = 2;
    }

    sdCmd = iosAlloc(__sdHeapId, OSRoundUp32B(SD_CMD_SIZE));
    if (sdCmd == NULL) {
        ret = IOS_ERROR_FAIL_ALLOC;
    } else {
        sdCmd[0] = cmd;
        sdCmd[1] = cmdType;
        sdCmd[2] = respType;
        sdCmd[3] = arg;
        sdCmd[6] = buffer;
        sdCmd[4] = blockCount;
        sdCmd[5] = sectorSize;
        sdCmd[7] = 1;

        __sdVect->base = (u8*)sdCmd;
        __sdVect->length = SD_CMD_SIZE;
        __sdVect[1].base = (u8*)buffer;
        __sdVect[1].length = blockCount * sectorSize;
        __sdVect[2].base = (u8*)__sdResp;
        __sdVect[2].length = 0x10;

        if (cb != NULL) {
            __sdCbArg* data = iosAlloc(__sdHeapId, sizeof(__sdCbArg));
            if (data == NULL) {
                ret = IOS_ERROR_FAIL_ALLOC;
            } else {
                data->cb = cb;
                data->cbArg = cbArg;
                data->resp = resp;
                data->command = cmd;
                data->SDSectorSize = (u8*)sdCmd;

                ret = IOS_IoctlvAsync(fd, 7, readCount, writeCount, __sdVect, __sdCb, data);
            }
        } else {
            ret = IOS_Ioctlv(fd, 7, readCount, writeCount, __sdVect);
            if (ret == IOS_ERROR_OK && resp != NULL) {
                resp[0] = __sdResp[0];
            }
        }
    }

    if ((cb == NULL || ret != IOS_ERROR_OK) && sdCmd != NULL) {
        iosFree(__sdHeapId, sdCmd);
    }

    return ret;
}

static s32 sduCommand(s32 fd, u32 cmd, u32 cmdType, u32 respType, u32 arg, u32 buffer, u32 blockCount, u32 sectorSize, u32 transferMode, u32* resp,
                            void* cb, void* cbArg) {
    s32 ret;
    u32* sdCmd;

    sdCmd = iosAlloc(__sdHeapId, ((SD_CMD_SIZE + 63) & ~63));
    if (sdCmd == NULL) {
        ret = IOS_ERROR_FAIL_ALLOC;
    } else {
        sdCmd[0] = cmd;
        sdCmd[1] = cmdType;
        sdCmd[2] = respType;
        sdCmd[3] = arg;
        sdCmd[6] = buffer;
        sdCmd[4] = blockCount;
        sdCmd[5] = sectorSize;
        sdCmd[7] = transferMode;

        if (resp != NULL) {
            if (cb != NULL) {
                __sdCbArg* data = iosAlloc(__sdHeapId, sizeof(__sdCbArg));
                if (data == NULL) {
                    ret = IOS_ERROR_FAIL_ALLOC;
                } else {
                    data->cb = cb;
                    data->cbArg = cbArg;
                    data->resp = resp;
                    data->command = cmd;
                    data->SDDevRca = 1;
                    data->SDSectorSize = (u8*)sdCmd;

                    ret = IOS_IoctlAsync(fd, 7, sdCmd, SD_CMD_SIZE  , __sdResp, 0x10, __sdCb, data);
                    if (ret != IOS_ERROR_OK) {
                        ret = SD_ERROR_FATAL;
                    }
                }
            } else {
                ret = IOS_Ioctl(fd, 7, sdCmd, 0x24, __sdResp, 0x10);
                if (ret != IOS_ERROR_OK) {
                    if (cmd == 7) {
                        ret = SD_ERROR_SUCCESS;
                    }
                } else {
                    resp[0] = __sdResp[0];
                    resp[1] = __sdResp[1];
                    resp[2] = __sdResp[2];
                    resp[3] = __sdResp[3];
                }
            }
        } else {
            ret = SD_ERROR_FATAL;
        }
    }

    if ((resp == NULL || cb == NULL || ret != 0) && sdCmd != NULL) {
        iosFree(__sdHeapId, sdCmd);
    }

    return ret;
}

s32 ISD_ResetDevice(ISD_HANDLE* dev) {
    IOSError ret;

    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }

    OSLockMutex(&__reqMutex);
    if (__sdReq != 0) {
        OSUnlockMutex(&__reqMutex);
        return SD_ERROR_FATAL;
    }

    __sdReq = 1;

    OSUnlockMutex(&__reqMutex);

    ret = IOS_Ioctl(dev->fd, 4, NULL, 0, __sdCmdBuffer, 4);
    if (ret != IOS_ERROR_OK) {
        dev->rca = 0;
        __sdReq = 0;
        return ret;
    }

    dev->rca = *__sdCmdBuffer;
    __sdCardStatus = 0x10000;
    dev->opened = 2;
    __sdReq = 0;

    return ret;
}

static IOSFd sduOpenFD(u32 slot) {
    IOSError ret = IOS_Open(__sdPaths[slot], 0);
    switch (ret) {
        case IOS_ERROR_ACCESS:
        case IOS_ERROR_EXISTS:
            return ret;
    }
    return ret;
}



static s32 sduSetClock(s32 fd, u32 enabled) {
    s32 ret;
    *__sdCmdBuffer = enabled;
    ret = IOS_Ioctl(fd, 6, __sdCmdBuffer, 4, NULL, 0);
    if (ret != 0) {
        ret = SD_ERROR_FATAL;
    }
    return ret;
}

s32 ISD_GetCardSize(ISD_HANDLE* dev, u32* size, u32* capacity, u32* sectorSize);
s32 ISD_ReadMultiBlockAsync(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count, SDCallbackFunc callback, void* argument);
s32 ISD_WriteMultiBlockAsync(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count, SDCallbackFunc callback, void* argument);

s32 ISD_ReadBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count) {
    return ISD_ReadMultiBlockAsync(dev, offset, buffer, count, NULL, NULL);
}

s32 ISD_WriteBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count) {
    return ISD_WriteMultiBlockAsync(dev, offset, buffer, count, NULL, NULL);
}

s32 ISD_ReadMultiBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count) {
    return ISD_ReadMultiBlockAsync(dev, offset, buffer, count, NULL, NULL);
}

s32 ISD_WriteMultiBlock(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count) {
    return ISD_WriteMultiBlockAsync(dev, offset, buffer, count, NULL, NULL);
}

s32 ISD_ReadMultiBlockAsync(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count, SDCallbackFunc callback, void* argument) {
    s32 ret;
    u32 status;
    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }
    if (dev->opened == 0) {
        return SD_ERROR_FATAL;
    }
    if (dev->capacity[2] == 0) {
        ret = ISD_GetCardSize(dev, NULL, NULL, NULL);
        if (ret < 0) {
            goto out;
        }
    }
    if (offset + count > dev->capacity[2]) {
        ret = -4;
        goto out;
    }
    OSLockMutex(&__reqMutex);
    if (__sdReq) {
        OSUnlockMutex(&__reqMutex);
        ret = SD_ERROR_FATAL;
        goto out;
    }
    __sdReq = 1;
    OSUnlockMutex(&__reqMutex);
    if (dev->opened == 2) {
        u32 blockResponse[4];
        u32 response[4];
        ret = sduCommand(dev->fd, 7, 3, 2, dev->rca, 0, 0, 0, 0, response, NULL, NULL);
        if (ret == 0) {
            dev->opened = 1;
            ret = sduCommand(dev->fd, 16, 3, 1, 512, 0, 0, 0, 0, blockResponse, NULL, NULL);
            if (ret == 0) {
                ret = sduDatabuswidth(dev, 4);
                if (ret == 0) {
                    ret = sduSetClock(dev->fd, 1);
                }
            }
        }
        if (ret != 0) {
            __sdReq = 0;
            goto out;
        }
    }
    ret = IOS_Ioctl(dev->fd, 11, NULL, 0, &status, 4);
    if (ret < 0) {
        goto out;
    }
    if (!(status & 0x100000)) {
        offset <<= 9;
    }
    DCInvalidateRange(buffer, count << 9);
    ret = sduCommandv(dev->fd, 18, 3, 1, offset, (u32)buffer, count, 512, 1, __sdResp2, callback, argument);
    if (ret != 0) {
        __sdReq = 0;
    }
    if (callback == NULL) {
        __sdReq = 0;
    }
out:
    return ret;
}

s32 ISD_WriteMultiBlockAsync(ISD_HANDLE* dev, u32 offset, u8* buffer, u32 count, SDCallbackFunc callback, void* argument) {
    s32 ret;
    u32 status;
    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }
    if (dev->opened == 0) {
        return SD_ERROR_FATAL;
    }
    if (dev->capacity[2] == 0) {
        ret = ISD_GetCardSize(dev, NULL, NULL, NULL);
        if (ret < 0) {
            goto out;
        }
    }
    if (offset + count > dev->capacity[2]) {
        ret = -4;
        goto out;
    }
    OSLockMutex(&__reqMutex);
    if (__sdReq) {
        OSUnlockMutex(&__reqMutex);
        ret = SD_ERROR_FATAL;
        goto out;
    }
    __sdReq = 1;
    OSUnlockMutex(&__reqMutex);
    if (dev->opened == 2) {
        u32 blockResponse[4];
        u32 response[4];
        ret = sduCommand(dev->fd, 7, 3, 2, dev->rca, 0, 0, 0, 0, response, NULL, NULL);
        if (ret == 0) {
            dev->opened = 1;
            ret = sduCommand(dev->fd, 16, 3, 1, 512, 0, 0, 0, 0, blockResponse, NULL, NULL);
            if (ret == 0) {
                ret = sduDatabuswidth(dev, 4);
                if (ret == 0) {
                    ret = sduSetClock(dev->fd, 1);
                }
            }
        }
        if (ret != 0) {
            __sdReq = 0;
            goto out;
        }
    }
    ret = IOS_Ioctl(dev->fd, 11, NULL, 0, &status, 4);
    if (ret < 0) {
        goto out;
    }
    if (!(status & 0x100000)) {
        offset <<= 9;
    }
    DCFlushRange(buffer, count << 9);
    ret = sduCommand(dev->fd, 25, 3, 1, offset, (u32)buffer, count, 512, 1, __sdResp2, callback, argument);
    if (ret != 0) {
        __sdReq = 0;
    }
    if (callback == NULL) {
        __sdReq = 0;
    }
out:
    return ret;
}

s32 ISD_MountCard(u32 slot, ISD_HANDLE** dev) {
    IOSError ret = IOS_ERROR_OK;

    if (__sdHeapId < 0) {
        ret = SD_ERROR_FATAL;
        goto out;
    }

    if (slot >= SD_MAX_SLOTS + 1) {
        return IOS_ERROR_INVALID;
    }

    if (*dev == NULL) {
        return IOS_ERROR_INVALID;
    }

    (*dev)->fd = sduOpenFD(slot);
    if ((*dev)->fd < 0) {
        ret = (*dev)->fd;
    }

    (*dev)->opened = 0;
    (*dev)->capacity[0] = 0x4000;
    (*dev)->capacity[2] = 0;

out:
    return ret;
}

s32 ISD_UnmountCard(ISD_HANDLE* dev) {
    dev->opened = 0;
    IOS_Close(dev->fd);
    dev->fd = -1;
}



s32 ISD_InitCard(void) {
    static void* lo;
    s32 ret = 0;
    BOOL enabled = OSDisableInterrupts();
    if (__sdHeapId >= 0) {
        OSRestoreInterrupts(enabled);
        goto out;
    }
    OSRestoreInterrupts(enabled);
    __sdReq = 0;
    OSInitMutex(&__reqMutex);
    lo = IPCGetBufferLo();
    IPCGetBufferHi();
    __sdHeapId = iosCreateHeap(lo, 2048);
    if (__sdHeapId < 0) {
        ret = -22;
        goto out;
    }
    __sdCmdBuffer = iosAlloc(__sdHeapId, 32);
    if (!__sdCmdBuffer) {
        ret = -22;
        goto out;
    }
    __sdResp = iosAlloc(__sdHeapId, 32);
    if (!__sdResp) {
        ret = -22;
        goto out;
    }
    __sdResp2 = iosAlloc(__sdHeapId, 32);
    if (!__sdResp2) {
        ret = -22;
        goto out;
    }
    __sdReg = iosAlloc(__sdHeapId, 32);
    if (!__sdReg) {
        ret = -22;
        goto out;
    }
    __sdVect = iosAlloc(__sdHeapId, 96);
    if (!__sdVect) {
        ret = -22;
        goto out;
    }
    IPCSetBufferLo((u8*)lo + 2048);
    strcpy(__sdPaths[0], SD_SLOT_0_PATH);
    strcpy(__sdPaths[1], SD_SLOT_1_PATH);
    strcpy(__sdPaths[2], SD_SLOT_W_PATH);
out:
    return ret;
}

s32 ISD_ReadCardRegister(ISD_HANDLE* dev, u32 cmd, u32* cmdResp, u32 cmdRespSize) {
    s32 ret;
    u32 cmdArg = dev->rca;

    if (__sdHeapId < 0) {
        ret = SD_ERROR_FATAL;
        goto out;
    }

    if (dev->opened == 0) {
        ret = SD_ERROR_FATAL;
        goto out;
    }

    OSLockMutex(&__reqMutex);
    if (__sdReq) {
        OSUnlockMutex(&__reqMutex);
        ret = SD_ERROR_FATAL;
        goto out;
    }

    __sdReq = 1;

    OSUnlockMutex(&__reqMutex);

    if (dev->opened == 1) {
        u32 resp[4];

        ret = sduCommand(dev->fd, 7, 3, 2, 0, 0, 0, 0, 0, resp, NULL, NULL);

        dev->opened = 2;

        if (ret == SD_ERROR_01000003) {
            ret = IOS_ERROR_OK;
        }

        __sdReq = 0;

        if (ret != IOS_ERROR_OK) {
            goto out;
        }
    }

    if (cmdResp == NULL || cmdRespSize == 0) {
        ret = -4;
        __sdReq = 0;
        goto out;
    }

    switch (cmd) {
        case 10:
        case 9: {
            break;
        }
        case 51: {
            ret = sduGetSCR(dev, cmdResp);
            __sdReq = 0;
            goto out;
        }
        case 41: {
            ret = sduGetOCR(dev, cmdResp);
            __sdReq = 0;
            goto out;
        }
        case 3: {
            cmdResp[0] = dev->rca;
            ret = IOS_ERROR_OK;
            __sdReq = 0;
            goto out;
        }
        default: {
            return -4;
        }
    }

    ret = sduCommand(dev->fd, cmd, 3, 3, cmdArg, 0, 0, 0, 0, cmdResp, NULL, NULL);

    __sdReq = 0;

out:
    return ret;
}

static s32 sduGetSCR(ISD_HANDLE* dev, u32* data) {
    s32 ret = SD_ERROR_FATAL;
    u32 resp[4];

    if (data != NULL) {
        sduCommand(dev->fd, 0x10, 3, 1, 4, 0, 0, 0, 0, resp, NULL, NULL);
        ret = sduCommand(dev->fd, 55, 3, 1, dev->rca, 0, 0, 0, 0, resp, NULL, NULL);
        if (ret != 0) {
            return ret;
        }
        sduCommand(dev->fd, 51, 3, 1, 0, (u32)data, 1, 4, 0, resp, NULL, NULL);
        ret = sduCommand(dev->fd, 0x10, 3, 1, 0x200, 0, 0, 0, 0, resp, NULL, NULL);
    }

    return ret;
}

static s32 sduGetOCR(ISD_HANDLE* dev, u32* data) {
    s32 ret = SD_ERROR_SUCCESS;

    if (data != NULL) {
        ret = IOS_Ioctl(dev->fd, 12, 0, 0, __sdCmdBuffer, 4);
        if (ret == IOS_ERROR_OK) {
            *data = *__sdCmdBuffer;
        }
    }

    return ret;
}

s32 sduDatabuswidth(ISD_HANDLE* dev, u32 buswidth) {
    u32 resp[4];
    u32 cmdArg = buswidth == 4 ? 2 : 0;
    u32 hcReg;
    s32 ret;

    ret = sduCommand(dev->fd, 0x37, 3, 1, dev->rca, 0, 0, 0, 0, resp, NULL, NULL);
    if (ret != SD_ERROR_SUCCESS) {
        return ret;
    }

    ret = sduCommand(dev->fd, 6, 3, 1, cmdArg, 0, 0, 0, 0, resp, NULL, NULL);
    if (ret != SD_ERROR_SUCCESS) {
        return ret;
    }

    ret = ISD_GetHCRegister(dev, 0x28, &hcReg, 1);
    if (ret != SD_ERROR_SUCCESS) {
        return ret;
    }

    if (cmdArg != 0) {
        hcReg |= 2;
    } else {
        hcReg &= ~0x02;
    }

    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }

    __sdReg[0] = 0x28;
    __sdReg[3] = 1;
    __sdReg[4] = hcReg;

    ret = IOS_Ioctl(dev->fd, 1, __sdReg, 0x18, NULL, 0);
    if (ret != IOS_ERROR_OK) {
        ret = SD_ERROR_FATAL;
    }

    return ret;
}

s32 ISD_GetCardSize(ISD_HANDLE* dev, u32* totalBlocks, u32* blockCount, u32* blockSize) {
    s32 ret;
    u32 sectorCount;
    u32 deviceSize;
    u32 sectorSize;

    u32 resp[4];
    memset(resp, 0, sizeof(resp));

    ret = ISD_ReadCardRegister(dev, 9, resp, 0x10);
    if (ret != SD_ERROR_SUCCESS) {
        return ret;
    }

    if ((resp[3] >> 0x16) & 3) {
        sectorSize = 0x80;
        deviceSize = (((resp[1] >> 8) & 0x3FFFFF) + 1) * 1024;
        sectorCount = ((((resp[1] >> 8) & 0x3FFFFF) + 1) * 8) & 0x01FFFFF8;
    } else {
        u32 readLength = (resp[2] >> 8) & 0xF;
        u32 cardSize = ((resp[2] << 10) & 0xC00) | (resp[1] >> 22);
        u32 sizeMultiplier = (resp[1] >> 7) & 7;
        readLength = readLength >= 9 ? readLength : 9;
        readLength = readLength <= 11 ? readLength : 11;

        sectorSize = (((resp[1] << 1) & 0x7E) | (resp[0] >> 31)) + 1;
        deviceSize = (1 << (readLength - 9)) * ((cardSize + 1) * (1 << (sizeMultiplier + 2)));
        sectorCount = ((deviceSize << 9) / sectorSize) >> (readLength - 9);
    }

    dev->capacity[0] = sectorSize;
    dev->capacity[1] = sectorCount;
    dev->capacity[2] = deviceSize;

    if (totalBlocks != NULL) {
        *totalBlocks = deviceSize;
    }
    if (blockCount != NULL) {
        *blockCount = deviceSize;
    }
    if (blockSize != NULL) {
        *blockSize = 512;
    }

    return SD_ERROR_SUCCESS;
}

s32 ISD_RegisterDeviceIntrHandler(ISD_HANDLE* dev, ISD_INTR_HANDLER callback, u32* argument) {
    u32 event;
    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }
    if (callback == NULL) {
        return SD_ERROR_FATAL;
    }
    if (argument != NULL) {
        event = *(u32*)argument;
    } else {
        return -4;
    }
    if (event != 2 && event != 1) {
        return -4;
    }
    return sduCommand(dev->fd, 64, 0, 0, event, 0, 0, 0, 0, __sdResp2, callback, argument);
}

s32 ISD_UnregisterDeviceIntrHandler(ISD_HANDLE* dev) {
    u32 response[4] ATTRIBUTE_ALIGN(32);
    if (__sdHeapId < 0) {
        return SD_ERROR_FATAL;
    }
    return sduCommand(dev->fd, 65, 0, 0, 0, 0, 0, 0, 0, response, NULL, NULL);
}
