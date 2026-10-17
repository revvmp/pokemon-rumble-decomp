#include <revolution/os.h>
#include <revolution/os/OSMutex.h>
#include <revolution/ipc.h>
#include <revolution/ncd.h>
#include <string.h>

#define NCD_IOCTLV_GET_WIRELESS_MAC_ADDRESS 8
#define NCD_IPC_BUFFER_SIZE 0x1B60

const char* __NCDVersion = "<< RVL_SDK - NCD \trelease build: Jun  9 2009 11:59:48 (0x4199_60831) >>";

static u8* ncdIpcBuffer;
static u32 ncdInitFlags;

static IOSIoVector ncdVectors[4] ATTRIBUTE_ALIGN(32);
static s32 ncdReply[8] ATTRIBUTE_ALIGN(32);
static OSMutex ncdMutex;

static void NCDiLock(void) {
    BOOL enabled = OSDisableInterrupts();
    u8* buffer;

    if (!(ncdInitFlags & 1)) {
        OSRegisterVersion(__NCDVersion);
        OSInitMutex(&ncdMutex);
        buffer = (u8*)OSRoundUp32B(IPCGetBufferLo());
        if ((u32)((u8*)IPCGetBufferHi() - buffer) < NCD_IPC_BUFFER_SIZE) {
            OSPanic(__FILE__, 1500, "Could not reserve heap for NCD library from IPC arena");
        }
        IPCSetBufferLo(buffer + NCD_IPC_BUFFER_SIZE);
        ncdIpcBuffer = buffer;
        memset(buffer, 0, NCD_IPC_BUFFER_SIZE);
        memset(ncdReply, 0, sizeof(ncdReply));
        memset(ncdVectors, 0, sizeof(ncdVectors));
        ncdInitFlags |= 1;
    }
    OSRestoreInterrupts(enabled);
    OSLockMutex(&ncdMutex);
}

NCDErr NCDiGetWirelessMacAddress(u8* macAddr) {
    NCDErr result = NCD_RESULT_SUCCESS;
    IOSFd fd;

    if (macAddr == NULL) {
        return NCD_RESULT_ILLEGAL_PARAM;
    }
    if (!OSGetCurrentThread()) {
        return NCD_RESULT_NOT_THREAD;
    }

    NCDiLock();
    fd = IOS_Open("/dev/net/ncd/manage", 0);
    if (fd < 0) {
        if (fd == IOS_ERROR_NOEXISTS) {
            result = NCD_RESULT_INPROGRESS;
        } else {
            result = NCD_RESULT_UNDECIDED;
        }
    } else {
        ncdVectors[0].base = (u8*)ncdReply;
        ncdVectors[0].length = sizeof(ncdReply);
        ncdVectors[1].base = ncdIpcBuffer;
        ncdVectors[1].length = NCD_MAC_ADDRESS_LENGTH;
        if (IOS_Ioctlv(fd, NCD_IOCTLV_GET_WIRELESS_MAC_ADDRESS, 0, 2, ncdVectors) < 0) {
            result = NCD_RESULT_UNDECIDED;
        } else {
            result = ncdReply[0];
            if (result == NCD_RESULT_SUCCESS) {
                memcpy(macAddr, ncdIpcBuffer, NCD_MAC_ADDRESS_LENGTH);
            }
        }
        if (IOS_Close(fd) < 0) {
            result = NCD_RESULT_FATAL;
        }
    }
    OSUnlockMutex(&ncdMutex);
    return result;
}

