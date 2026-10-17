#include <revolution/base/PPCArch.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSArena.h>
#include <revolution/os/OSReset.h>
#include "revolution/os.h"

#include "revolution/ipc/ipcclt.h"
#include "revolution/sc.h"

extern BOOL __OSInIPL;

typedef enum NWC24Err {
    NWC24_OK = 0,
    NWC24_ERR_FATAL = -1,
    NWC24_ERR_INVALID_VALUE = -3,
    NWC24_ERR_INPROGRESS = -29,
    NWC24_ERR_INTERNAL_IPC = -42,
} NWC24Err;

NWC24Err NWC24iPrepareShutdown(void) NO_INLINE;
NWC24Err NWC24iSynchronizeRtcCounter(BOOL) NO_INLINE;
s32 NWC24SuspendScheduler(void) NO_INLINE;

static NWC24Err NWC24iRequestShutdown(u32 event, NWC24Err* pResult) NO_INLINE;

static s32 nwc24ShtFd = -1;
static s32 nwc24ShtRetryRest = 0;
static BOOL NWC24iIsRequestPending = FALSE;
static BOOL nwc24ShuttingDown = FALSE;
static NWC24Err nwc24ShutdownResult = NWC24_OK;
static OSShutdownFunctionInfo ShutdownFuncInfo = {0};
static s32 nwc24ScdResult[8] ATTRIBUTE_ALIGN(32) = {0};
static u32 nwc24ShtBuffer[8] ATTRIBUTE_ALIGN(32) = {0};
static s32 nwc24ShtResult[8] ATTRIBUTE_ALIGN(32) = {0};
static u32 nwc24RtcBuffer[8] ATTRIBUTE_ALIGN(32) = {0};
static s32 nwc24RtcResult[8] ATTRIBUTE_ALIGN(32) = {0};

static BOOL NWC24Shutdown_(BOOL final, u32 event);
static s32 CallbackAsyncIpc(s32 result, void* pArg);
static NWC24Err NWC24iSetRtcCounter_(u32 rtc, u32 flags) NO_INLINE;

static inline NWC24Err OpenResourceManager(const char* pUser, const char* pName, s32* pFd, u32 mode) {
    s32 result;
    if (pFd == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }
    result = IOS_Open(pName, mode);
    *pFd = result;
    if (result < 0) {
        if (result == -6) {
            return NWC24_ERR_INPROGRESS;
        }
        return NWC24_ERR_INTERNAL_IPC;
    }
    return NWC24_OK;
}

static inline NWC24Err CloseResourceManager(const char* pUser, s32 fd) {
    return IOS_Close(fd) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

static inline NWC24Err IoctlResourceManager(const char* pUser, s32 fd, s32 type, void* pIn, s32 inSize, void* pOut, s32 outSize) {
    return IOS_Ioctl(fd, type, pIn, inSize, pOut, outSize) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

static inline NWC24Err IoctlResourceManagerAsync(const char* pUser, s32 fd, s32 type, void* pIn, s32 inSize, void* pOut, s32 outSize, void* pCallbackArg) {
    if (IOS_IoctlAsync(fd, type, pIn, inSize, pOut, outSize, CallbackAsyncIpc, pCallbackArg) < 0) {
        return NWC24_ERR_INTERNAL_IPC;
    }
    NWC24iIsRequestPending = TRUE;
    return NWC24_OK;
}

static inline NWC24Err CheckCallingStatus(const char* pUser) {
    return OSGetCurrentThread() == 0 ? NWC24_ERR_FATAL : NWC24_OK;
}

static inline NWC24Err GetRTC(u32* rtcCounter) {
    OSTime time;
    u32 bias, status;
    do {
        status = SCCheckStatus();
        if (status == 2) {
            return NWC24_ERR_FATAL;
        }
    } while (status != 0);
    bias = SCGetCounterBias();
    time = OSGetTime();
    *rtcCounter = (u32)(OSTicksToSeconds(time) - bias);
    return NWC24_OK;
}

void __OSInitNet(void) {
    NWC24Err cur_err;
    s32 res;
    OSIOSRev iosRev;

    __OSGetIOSRev(&iosRev);

    if (iosRev.major <= 4 || iosRev.major == 9) {
        return;
    }

    cur_err = NWC24iPrepareShutdown();

    if (cur_err != NWC24_OK) {
        if (cur_err < NWC24_OK) {
            OSReport("Failed to register network shutdown function. %d\n", cur_err);
        }

        res = NWC24SuspendScheduler();
        if (res < NWC24_OK) {
            OSReport("Failed to suspend the WiiConnect24 scheduler. %d\n", res);
        }
    }

    if (!__OSInIPL) {
        cur_err = NWC24iSynchronizeRtcCounter(0);

        if (cur_err != NWC24_OK) {
            OSReport("Failed to synchronize time with network resource managers. %d\n", cur_err);
        }
    }
}

NWC24Err NWC24iPrepareShutdown(void) {
#pragma force_active on
    static char functionName[] = "NWC24iPrepareShutdown";
#pragma force_active off

    NWC24Err result = NWC24_OK;
    ShutdownFuncInfo.func = NWC24Shutdown_;
    ShutdownFuncInfo.priority = 110;
    OSRegisterShutdownFunction(&ShutdownFuncInfo);
    if (nwc24ShtFd < 0) {
        result = OpenResourceManager(functionName, "/dev/net/kd/request", &nwc24ShtFd, 1);
    }
    nwc24ShtRetryRest = 5;
    if (result == NWC24_OK) {
        result = 1;
    }
    return result;
}

NWC24Err NWC24iSynchronizeRtcCounter(BOOL forceSave) {
    u32 rtc;
    NWC24Err result;
    result = GetRTC(&rtc);
    if (result != NWC24_OK) {
        return result;
    }
    return NWC24iSetRtcCounter_(rtc, forceSave ? 1 : 0);
}

s32 NWC24SuspendScheduler(void) {
#pragma force_active on
    static char functionName[] = "NWC24SuspendScheduler";
    static char resumeFunctionName[] = "NWC24ResumeScheduler";
#pragma force_active off

    NWC24Err result;
    s32 fd;
    result = CheckCallingStatus(functionName);
    if (result < 0) {
        return result;
    }
    fd = IOS_Open("/dev/net/kd/request", 0);
    if (fd < 0) {
        if (fd == -6) {
            result = NWC24_ERR_INPROGRESS;
        } else {
            result = NWC24_ERR_INTERNAL_IPC;
        }
    } else {
        result = NWC24_OK;
    }
    if (result >= 0) {
        result = IoctlResourceManager(functionName, fd, 1, NULL, 0, nwc24ScdResult, sizeof(nwc24ScdResult));
        if (result >= 0) {
            result = nwc24ScdResult[0];
        }
        {
            NWC24Err close = CloseResourceManager(functionName, fd);
            if (close < 0) {
                result = close;
            }
        }
    }
    return result;
}


static NWC24Err NWC24iRequestShutdown(u32 event, NWC24Err* pResult) {
#pragma force_active on
    static char functionName[] = "NWC24iRequestShutdown";
#pragma force_active off

    nwc24ShtBuffer[0] = event;
    return IoctlResourceManagerAsync(functionName, nwc24ShtFd, 40, nwc24ShtBuffer, sizeof(nwc24ShtBuffer), nwc24ShtResult, sizeof(nwc24ShtResult), pResult);
}

static BOOL NWC24Shutdown_(BOOL final, u32 event) {
    if (final) {
        return TRUE;
    }
    if (nwc24ShuttingDown) {
        if (NWC24iIsRequestPending) {
            return FALSE;
        }
        if (nwc24ShutdownResult >= 0) {
            return TRUE;
        }
        if (nwc24ShtRetryRest > 0) {
            nwc24ShuttingDown = FALSE;
            nwc24ShtRetryRest--;
        } else {
            OSReport("NWC24Shutdown_: Give up!\n");
            return TRUE;
        }
    } else if (NWC24iRequestShutdown(event, &nwc24ShutdownResult) >= 0) {
        nwc24ShuttingDown = TRUE;
    }
    return FALSE;
}

static NWC24Err NWC24iSetRtcCounter_(u32 rtc, u32 flags) {
#pragma force_active on
    static char functionName[] = "NWC24iSetRtcCounter_";
#pragma force_active off

    NWC24Err result;
    s32 fd;
    result = CheckCallingStatus(functionName);
    if (result < 0) {
        return result;
    }
    fd = IOS_Open("/dev/net/kd/time", 0);
    if (fd < 0) {
        if (fd == -6) {
            result = NWC24_ERR_INPROGRESS;
        } else {
            result = NWC24_ERR_INTERNAL_IPC;
        }
    } else {
        result = NWC24_OK;
    }
    if (result >= 0) {
        nwc24RtcBuffer[0] = rtc;
        nwc24RtcBuffer[1] = flags;
        result = IoctlResourceManager(functionName, fd, 23, nwc24RtcBuffer, sizeof(nwc24RtcBuffer), nwc24RtcResult, sizeof(nwc24RtcResult));
        if (result >= 0) {
            result = nwc24RtcResult[0];
        }
        {
            NWC24Err close = CloseResourceManager(functionName, fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    return result;
}

static s32 CallbackAsyncIpc(s32 result, void* pArg) {
    if (pArg != NULL) {
        *(s32*)pArg = result;
    }
    NWC24iIsRequestPending = FALSE;
    return 0;
}
