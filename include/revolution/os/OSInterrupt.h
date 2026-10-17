#ifndef REVOLUTION_OS_OSINTERRUPT_H
#define REVOLUTION_OS_OSINTERRUPT_H

#include "revolution/types.h"
#include "revolution/os/OSContext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef s16 __OSInterrupt;
typedef u32 OSInterruptMask;

typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext* context);

#define __OS_INTERRUPT_PI_PE_TOKEN 18
#define __OS_INTERRUPT_PI_PE_FINISH 19
#define __OS_INTERRUPT_PI_DEBUG 25

#define OS_INTERRUPTMASK(interrupt) (0x80000000u >> (interrupt))

#define OS_INTERRUPTMASK_EXI_2_EXI 0x00008000
#define OS_INTERRUPTMASK_EXI_2_TC 0x00010000
#define OS_INTERRUPTMASK_EXI_2 (OS_INTERRUPTMASK_EXI_2_EXI | OS_INTERRUPTMASK_EXI_2_TC)
#define OS_INTERRUPTMASK_PI_PE_TOKEN OS_INTERRUPTMASK(__OS_INTERRUPT_PI_PE_TOKEN)
#define OS_INTERRUPTMASK_PI_PE_FINISH OS_INTERRUPTMASK(__OS_INTERRUPT_PI_PE_FINISH)
#define OS_INTERRUPTMASK_PI_DEBUG 0x00000040

BOOL OSDisableInterrupts(void);
BOOL OSEnableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt);

OSInterruptMask __OSMaskInterrupts(OSInterruptMask mask);
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask);

#ifdef __cplusplus
}
#endif

#endif
