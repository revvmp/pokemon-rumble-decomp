#ifndef REVOLUTION_OS_OSMEMORY_H
#define REVOLUTION_OS_OSMEMORY_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 OSGetPhysicalMem1Size(void);
u32 OSGetPhysicalMem2Size(void);
u32 OSGetConsoleSimulatedMem1Size(void);
u32 OSGetConsoleSimulatedMem2Size(void);
void __OSRestoreMEM1Config(u32 key);
void __OSInitMemoryProtection(void);

#ifdef __cplusplus
}
#endif

#endif
