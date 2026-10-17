#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include "revolution/os/OSError.h"

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSIOSRev {
    u8 reserved;
    u8 major;
    u8 minor;
    u8 micro;
    u8 month;
    u8 date;
    u16 year;
} OSIOSRev;

u32 OSGetPhysicalMem2Size(void);
void __OSGetIOSRev(OSIOSRev*);

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

#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))
#define OSDiffTick(tick1, tick0) ((s32)(tick1) - (s32)(tick0))

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + 0x80000000))
#define OSCachedToPhysical(caddr) ((u32)((u8*)(caddr) - 0x80000000))

#define OS_INTERRUPT_PI_SI 20
#define OS_INTERRUPTMASK_PI_SI 0x800

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

void OSRegisterVersion(const char* id);
const char* OSGetAppGamename(void);
u8 OSGetAppType(void);

extern BOOL __OSIsReturnToIdle;
void OSDumpContext(OSContext* context);

OSTime __OSGetSystemTime(void);

void OSCreateAlarm(OSAlarm* alarm);
void OSSetAlarm(OSAlarm* alarm, OSTime tick, OSAlarmHandler handler);
void OSSetPeriodicAlarm(OSAlarm* alarm, OSTime start, OSTime period, OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm* alarm);
void OSSetAlarmUserData(OSAlarm* alarm, void* data);
void* OSGetAlarmUserData(const OSAlarm* alarm);

OSTime OSGetTime(void);

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask);

BOOL __OSSetVIForceDimming(BOOL isEnabled, u32 yShift, u32 xShift);

u16 OSGetWirelessID(s32 chan);
void OSSetWirelessID(s32 chan, u16 id);

#ifdef __cplusplus
}
#endif

#endif
