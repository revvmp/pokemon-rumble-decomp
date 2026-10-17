#include "private/flipper.h"
#include "revolution/dvd.h"
#include "revolution/esp/esp.h"
#include "revolution/os.h"
#include "revolution/os/OSArena.h"
#include "revolution/os/OSAudioSystem.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSExec.h"
#include "revolution/os/OSExecParams.h"
#include "revolution/os/OSPlayRecord.h"
#include "revolution/os/OSPlayTime.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSRtc.h"
#include "revolution/os/OSStateFlags.h"
#include "revolution/os/OSStateTM.h"
#include "revolution/os/OSThread.h"
#include "revolution/pad.h"
#include "revolution/sc.h"
#include "revolution/vi.h"
#include <string.h>

extern BOOL __OSInNandBoot;
extern BOOL __OSInReboot;
extern OSExecParams __OSRebootParams;

extern BOOL __OSSyncSram(void);
extern void __OSHotReset(void);
extern void __OSInitSTM(void);
extern void __OSShutdownToSBY(void);
extern void __OSRelaunchTitle(u32 resetCode);
extern void __OSReboot(u32 resetCode, u32 bootDol);
extern void OSPanic(const char* file, int line, const char* msg, ...);
extern void OSReport(const char* msg, ...);

OSThreadQueue __OSActiveThreadQueue : 0x800000DC;

typedef struct OSShutdownFunctionQueue {
    OSShutdownFunctionInfo* head;
    OSShutdownFunctionInfo* tail;
} OSShutdownFunctionQueue;

static OSShutdownFunctionQueue ShutdownFunctionQueue;
static u32 bootThisDol = 0;
BOOL __OSIsReturnToIdle = FALSE;

#define EnqueueTail(queue, info)                                                                                       \
    do {                                                                                                               \
        OSShutdownFunctionInfo* __prev;                                                                                \
                                                                                                                       \
        __prev = (queue)->tail;                                                                                        \
        if (__prev == NULL)                                                                                            \
            (queue)->head = (info);                                                                                    \
        else                                                                                                           \
            __prev->next = (info);                                                                                     \
        (info)->prev = __prev;                                                                                         \
        (info)->next = NULL;                                                                                           \
        (queue)->tail = (info);                                                                                        \
    } while (0)

#define EnqueuePrio(queue, info)                                                                                       \
    do {                                                                                                               \
        OSShutdownFunctionInfo* __prev;                                                                                \
        OSShutdownFunctionInfo* __next;                                                                                \
                                                                                                                       \
        for (__next = (queue)->head; __next && __next->priority <= info->priority; __next = __next->next)              \
            ;                                                                                                          \
        if (__next == NULL)                                                                                            \
            EnqueueTail(queue, info);                                                                                  \
        else {                                                                                                         \
            (info)->next = __next;                                                                                     \
            __prev = __next->prev;                                                                                     \
            __next->prev = (info);                                                                                     \
            (info)->prev = __prev;                                                                                     \
            if (__prev == NULL)                                                                                        \
                (queue)->head = (info);                                                                                \
            else                                                                                                       \
                __prev->next = (info);                                                                                 \
        }                                                                                                              \
    } while (0)

static void KillThreads(void);

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info) {
    EnqueuePrio(&ShutdownFunctionQueue, info);
}

BOOL __OSCallShutdownFunctions(BOOL final, u32 event) {
    OSShutdownFunctionInfo* info;
    BOOL err;
    u32 priority = 0;

    err = FALSE;
    info = ShutdownFunctionQueue.head;
    while (info) {
        if (err && priority != info->priority) {
            break;
        }

        err |= !info->func(final, event);
        priority = info->priority;
        info = info->next;
    }
    err |= !__OSSyncSram();
    return err ? FALSE : TRUE;
}

void __OSShutdownDevices(u32 event) {
    BOOL rc;
    BOOL disableRecalibration;
    BOOL doRecal;

    switch (event) {
    case OS_SHUTDOWN_EVENT_FATAL:
    case OS_SHUTDOWN_EVENT_RETURN_TO_MENU:
    case OS_SHUTDOWN_EVENT_LAUNCH:
        doRecal = FALSE;
        break;
    case OS_SHUTDOWN_EVENT_SHUTDOWN:
    case OS_SHUTDOWN_EVENT_RESERVED:
    case OS_SHUTDOWN_EVENT_RESTART:
    case OS_SHUTDOWN_EVENT_REBOOT:
    default:
        doRecal = TRUE;
        break;
    }

    __OSStopAudioSystem();

    if (!doRecal) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }

    while (!__OSCallShutdownFunctions(FALSE, event)) {
    }

    while (!__OSSyncSram()) {
    }

    OSDisableInterrupts();
    rc = __OSCallShutdownFunctions(TRUE, event);
    LCDisable();

    if (!doRecal) {
        __PADDisableRecalibration(disableRecalibration);
    }

    KillThreads();
}

u8 __OSGetDiscState(u8 last) {
    u32 flags;

    if (__DVDGetCoverStatus() != 2) {
        return 3;
    } else {
        if ((last == 1) && (__OSGetRTCFlags(&flags) && !flags)) {
            return 1;
        } else {
            return 2;
        }
    }
}

void OSShutdownSystem(void) {
    SCIdleModeInfo idleModeInfo;
    OSIOSRev iosRev;
    OSStateFlags state;

    memset(&idleModeInfo, 0, sizeof(SCIdleModeInfo));

    SCInit();
    while (SCCheckStatus() == SC_STATUS_BUSY) {
    }

    SCGetIdleMode(&idleModeInfo);
    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);
    state.lastDiscState = __OSGetDiscState(state.lastDiscState);

    if (idleModeInfo.mode == 1) {
        state.lastShutdown = 5;
    } else {
        state.lastShutdown = 1;
    }

    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    __OSGetIOSRev(&iosRev);

    if (idleModeInfo.mode == 1) {
        __OSIsReturnToIdle = TRUE;

        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_EVENT_RETURN_TO_MENU);
        OSEnableScheduler();
        __OSLaunchMenu();
    } else {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_EVENT_SHUTDOWN);
        __OSShutdownToSBY();
    }
}

void OSRestart(u32 resetCode) {
    u8 type = OSGetAppType();
    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();

    if (type == 0x81) {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_EVENT_RESTART);
        OSEnableScheduler();
        __OSRelaunchTitle(resetCode);
    } else if (type == 0x80) {
        OSDisableScheduler();
        __OSShutdownDevices(OS_SHUTDOWN_EVENT_RESTART);
        OSEnableScheduler();
        __OSReboot(resetCode, bootThisDol);
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_EVENT_REBOOT);
    __OSHotResetForError();
}

static void KillThreads(void) {
    OSThread* thread;
    OSThread* next;

    for (thread = __OSActiveThreadQueue.head; thread; thread = next) {
        next = thread->linkActive.next;

        switch (thread->state) {
        case OS_THREAD_STATE_READY:
        case OS_THREAD_STATE_WAITING:
            OSCancelThread(thread);
            break;
        default:
            break;
        }
    }
}

static void __OSReturnToMenu(u8 menuMode) {
    OSStateFlags state;
    ESTicketView* ticketView;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&state);
    state.lastDiscState = __OSGetDiscState(state.lastDiscState);
    state.lastShutdown = 3;
    state.menuMode = menuMode;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);

    OSSetArenaLo((void*)0x81280000);
    OSSetArenaHi((void*)0x812F0000);

    if (ESP_InitLib() != 0) {
        __OSReturnToMenuForError();
    }

    ticketView = (ESTicketView*)OSAllocFromMEM1ArenaLo(OSRoundUp32B(sizeof(ESTicketView)), 32);
    if (ticketView == NULL) {
        __OSReturnToMenuForError();
    }

    memset(ticketView, 0, OSRoundUp32B(sizeof(ESTicketView)));

    if (ESP_DiGetTicketView(NULL, ticketView) == 0 && OSPlayTimeIsLimited()) {
        __OSPlayTimeType type = 0;
        u32 remaining = 0xFFFFFFFF;

        __OSGetPlayTime(ticketView, &type, &remaining);
        if (remaining == 0) {
            __OSWriteExpiredFlagIfSet();
        }
    }

    OSDisableScheduler();
    __OSShutdownDevices(OS_SHUTDOWN_EVENT_RETURN_TO_MENU);
    OSEnableScheduler();
    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotResetForError();
}

void OSReturnToMenu(void) {
    __OSReturnToMenu(0);
    OSPanic(__FILE__, 0x37F, "OSReturnToMenu(): Falied to boot system menu.\n");
}

void OSReturnToDataManager(void) {
    __OSReturnToMenu(1);
    OSPanic(__FILE__, 0x391, "OSReturnToDataManager(): Falied to boot system menu.\n");
}

void OSReturnToSetting(int setting) {
    char* url;

    switch (setting) {
    case OS_SETTING_CALENDAR:
        url = OS_SYSLAUNCH_URL_CALENDAR;
        break;
    case OS_SETTING_DISPLAY:
        url = OS_SYSLAUNCH_URL_DISPLAY;
        break;
    case OS_SETTING_SOUND:
        url = OS_SYSLAUNCH_URL_SOUND;
        break;
    case OS_SETTING_PARENTAL:
        url = OS_SYSLAUNCH_URL_PARENTAL;
        break;
    case OS_SETTING_INTERNET:
        url = OS_SYSLAUNCH_URL_INTERNET;
        break;
    case OS_SETTING_WC24:
        url = OS_SYSLAUNCH_URL_WC24;
        break;
    case OS_SETTING_UPDATE:
        url = OS_SYSLAUNCH_URL_UPDATE;
        break;
    default:
        OSReport("OSReturnToSetting(): You can't specify %d.  \n", setting);
        OSPanic(__FILE__, 0x3CB, "");
    }

    __OSReturnToMenul(1, url, NULL);
}

void __OSReturnToMenuForError(void) {
    OSStateFlags state;

    __OSReadStateFlags(&state);
    state.lastDiscState = 2;
    state.lastShutdown = 3;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&state);
    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotResetForError();
    OSPanic(__FILE__, 0x3F2, "__OSReturnToMenu(): Falied to boot system menu.\n");
}

void __OSHotResetForError(void) {
    if (__OSInNandBoot || __OSInReboot) {
        __OSInitSTM();
    }

    __OSHotReset();

    OSPanic(__FILE__, 0x40A, "__OSHotReset(): Falied to reset system.\n");
}

u32 OSGetResetCode(void) {
    u32 code;

    if (__OSRebootParams.valid) {
        code = (0x80000000 | __OSRebootParams.restartCode);
    } else {
        code = (__PIRegs[9] & 0xFFFFFFF8) >> 3;
    }

    return code;
}

void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu) {
    OSPanic(__FILE__, 0x4A1, "OSResetSystem() is obsoleted. It doesn't work any longer.\n");
}

u32 OSSetBootDol(u32 dolOffset) {
    OSPanic(__FILE__, 0x4B0, "OSSetBootDol() is obsoleted. It doesn't work any longer.\n");
    return 0;
}
