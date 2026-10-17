#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include "revolution/os/OSError.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef s64 OSTime;
typedef u8 __OSException;
typedef s16 __OSInterrupt;
typedef u32 OSInterruptMask;

typedef struct OSContext OSContext;

typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext* context);

typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm* alarm, OSContext* context);

struct OSAlarm {
    OSAlarmHandler handler;
    u32 tag;
    OSTime fire;
    OSAlarm* prev;
    OSAlarm* next;
    OSTime period;
    OSTime start;
    void* userData;
};

#define OS_BUS_CLOCK (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + 0x80000000))
#define OSCachedToPhysical(caddr) ((u32)((u8*)(caddr) - 0x80000000))

#define OS_INTERRUPT_PI_SI 20
#define OS_INTERRUPTMASK_PI_SI 0x800

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

void OSRegisterVersion(const char* id);
void OSDumpContext(OSContext* context);

OSTime __OSGetSystemTime(void);

void OSSetAlarm(OSAlarm* alarm, OSTime tick, OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm* alarm);

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask);

u16 OSGetWirelessID(s32 chan);
void OSSetWirelessID(s32 chan, u16 id);

#ifdef __cplusplus
}
#endif

#endif
