#include "revolution/os/OSAlarm.h"

#define NULL 0

typedef int BOOL;
typedef unsigned long u32;
typedef unsigned char __OSException;

struct OSContext {
    unsigned long gpr[32];
    unsigned long cr;
    unsigned long lr;
    unsigned long ctr;
    unsigned long xer;
    double fpr[32];
    unsigned long fpscr_pad;
    unsigned long fpscr;
    unsigned long srr0;
    unsigned long srr1;
    unsigned short mode;
    unsigned short state;
    unsigned long gqr[8];
    unsigned long psf_pad;
    double psf[32];
};

typedef void (*__OSExceptionHandler)(__OSException, OSContext*);
typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;
struct OSShutdownFunctionInfo {
    BOOL (*func)(BOOL, u32);
    u32 priority;
    OSShutdownFunctionInfo* next;
    OSShutdownFunctionInfo* prev;
};

__OSExceptionHandler __OSGetExceptionHandler(__OSException);
__OSExceptionHandler __OSSetExceptionHandler(__OSException, __OSExceptionHandler);
void OSRegisterShutdownFunction(OSShutdownFunctionInfo*);
OSTime __OSGetSystemTime(void);
OSTime __OSTimeToSystemTime(OSTime);
void PPCMtdec(u32);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL);
void OSLoadContext(OSContext*);
void OSClearContext(OSContext*);
void OSSetCurrentContext(OSContext*);
signed long OSDisableScheduler(void);
signed long OSEnableScheduler(void);
void __OSReschedule(void);

BOOL __DVDTestAlarm(const OSAlarm *);
static void DecrementerExceptionHandler(__OSException, OSContext *);
static BOOL OnReset(BOOL, u32);

static struct OSAlarmQueue {
    OSAlarm* head;
    OSAlarm* tail;
} AlarmQueue;

static OSShutdownFunctionInfo ShutdownFunctionInfo = {
    OnReset,
    0xFFFFFFFF,
    NULL,
    NULL
};

void __OSInitAlarm(void) {
    if (__OSGetExceptionHandler(8) != DecrementerExceptionHandler) {
        AlarmQueue.head = AlarmQueue.tail = NULL;
        __OSSetExceptionHandler(8, DecrementerExceptionHandler);
        OSRegisterShutdownFunction(&ShutdownFunctionInfo);
    }
}

void OSCreateAlarm(OSAlarm* alarm) {
    alarm->handler = NULL;
    alarm->tag = 0;
}

static void SetTimer(OSAlarm* alarm) {
    OSTime delta;
    delta = alarm->fire - __OSGetSystemTime();

    if (delta < 0) {
        PPCMtdec(0);
    }
    else if (delta < 0x80000000) {
        PPCMtdec((u32)delta);
    }
    else {
        PPCMtdec(0x7FFFFFFF);
    }
}

static void InsertAlarm(OSAlarm* alarm, OSTime fire, OSAlarmHandler handler) {
    OSAlarm* next;
    OSAlarm* prev;

    if (0 < alarm->period) {
        OSTime time = __OSGetSystemTime();
        fire = alarm->start;

        if (alarm->start < time) {
            fire += alarm->period * ((time - alarm->start) / alarm->period + 1);
        }
    }

    alarm->handler = handler;
    alarm->fire = fire;

    for (next = AlarmQueue.head; next; next = next->next) {
        if (next->fire <= fire) {
            continue;
        }

        alarm->prev = next->prev;
        next->prev = alarm;
        alarm->next = next;
        prev = alarm->prev;

        if (prev != NULL) {
            prev->next = alarm;
        }
        else {
            AlarmQueue.head = alarm;
            SetTimer(alarm);
        }

        return;
    }

    alarm->next = NULL;
    prev = AlarmQueue.tail;
    AlarmQueue.tail = alarm;
    alarm->prev = prev;

    if (prev != NULL) {
        prev->next = alarm;
    }
    else {
        AlarmQueue.head = AlarmQueue.tail = alarm;
        SetTimer(alarm);
    }
}

void OSSetAlarm(OSAlarm* alarm, OSTime tick, OSAlarmHandler handler) {
    BOOL enabled;
    enabled = OSDisableInterrupts();
    alarm->period = 0;
    InsertAlarm(alarm, __OSGetSystemTime() + tick, handler);
    OSRestoreInterrupts(enabled);
}

void OSSetPeriodicAlarm(OSAlarm* alarm, OSTime start, OSTime period, OSAlarmHandler handler) {
    BOOL enabled;
    enabled = OSDisableInterrupts();
    alarm->period = period;
    alarm->start = __OSTimeToSystemTime(start);
    InsertAlarm(alarm, 0, handler);
    OSRestoreInterrupts(enabled);
}

void OSCancelAlarm(OSAlarm* alarm) {
    OSAlarm* next;
    BOOL enabled;
    enabled = OSDisableInterrupts();

    if (alarm->handler == NULL) {
        OSRestoreInterrupts(enabled);
        return;
    }

    next = alarm->next;

    if (next == NULL) {
        AlarmQueue.tail = alarm->prev;
    }
    else {
        next->prev = alarm->prev;
    }

    if (alarm->prev != NULL) {
        alarm->prev->next = next;
    }
    else {
        AlarmQueue.head = next;

        if (next != NULL) {
            SetTimer(next);
        }
    }

    alarm->handler = NULL;
    OSRestoreInterrupts(enabled);
}

static void DecrementerExceptionCallback(register __OSException exception, register OSContext* context) {
    OSAlarm* alarm;
    OSAlarm* next;
    OSAlarmHandler handler;
    OSTime time;
    OSContext exceptionContext;

    time = __OSGetSystemTime();
    alarm = AlarmQueue.head;

    if (alarm == NULL) {
        OSLoadContext(context);
    }

    if (time < alarm->fire) {
        SetTimer(alarm);
        OSLoadContext(context);
    }

    next = alarm->next;
    AlarmQueue.head = next;

    if (next == NULL) {
        AlarmQueue.tail = NULL;
    }
    else {
        next->prev = NULL;
    }

    handler = alarm->handler;
    alarm->handler = NULL;

    if (0 < alarm->period) {
        InsertAlarm(alarm, 0, handler);
    }

    if (AlarmQueue.head != NULL) {
        SetTimer(AlarmQueue.head);
    }

    OSDisableScheduler();
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    handler(alarm, context);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
    OSEnableScheduler();
    __OSReschedule();
    OSLoadContext(context);
}

asm void DecrementerExceptionHandler(register __OSException exception, register OSContext* context) {
    nofralloc

    stw r0, 0(context)
    stw r1, 4(context)
    stw r2, 8(context)
    stmw r6, 0x18(context)

    mfspr r0, 0x391
    stw r0, 0x1A8(context)

    mfspr r0, 0x392
    stw r0, 0x1AC(context)

    mfspr r0, 0x393
    stw r0, 0x1B0(context)

    mfspr r0, 0x394
    stw r0, 0x1B4(context)

    mfspr r0, 0x395
    stw r0, 0x1B8(context)

    mfspr r0, 0x396
    stw r0, 0x1BC(context)

    mfspr r0, 0x397
    stw r0, 0x1C0(context)

    stwu r1, -8(r1)
    b DecrementerExceptionCallback
}

static BOOL OnReset(BOOL final, u32 event) {
    OSAlarm* alarm;
    OSAlarm* next;

    if (final) {
        for (alarm = AlarmQueue.head, next = alarm ? alarm->next : NULL; alarm; alarm = next, next = alarm ? alarm->next : NULL) {
            if (!__DVDTestAlarm(alarm)) {
                OSCancelAlarm(alarm);
            }
        }
    }

    return 1;
}

void OSSetAlarmUserData(OSAlarm* alarm, void *userData) {
    alarm->userData = userData;
}

void* OSGetAlarmUserData(const OSAlarm* alarm) {
    return alarm->userData;
}

void __OSCancelAlarmsForUserData(void* userData) {
    BOOL enabled;
    OSAlarm* alarm;
    OSAlarm* next;
    enabled = OSDisableInterrupts();

    for (alarm = AlarmQueue.head, next = alarm ? alarm->next : NULL;
         alarm;
         alarm = next, next = alarm ? alarm->next : NULL) {
        if (alarm->tag == 0xFFFFFFFF && alarm->userData == userData) {
            OSCancelAlarm(alarm);
        }
    }

    OSRestoreInterrupts(enabled);
}
