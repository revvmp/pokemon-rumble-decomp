#ifndef REVOLUTION_OS_OSTIME_H
#define REVOLUTION_OS_OSTIME_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef s64 OSTime;

#define OS_BUS_CLOCK (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

OSTime OSGetTime(void);
OSTime __OSGetSystemTime(void);

#ifdef __cplusplus
}
#endif

#endif
