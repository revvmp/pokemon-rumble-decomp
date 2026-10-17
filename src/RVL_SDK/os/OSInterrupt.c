#include "revolution/os/OSInterrupt.h"
#include "private/flipper.h"
#include "revolution/os/OSAddress.h"
#include "revolution/os/OSThread.h"
#include <string.h>

#define __OSUserInterruptMask (*(OSInterruptMask*)OSPhysicalToCached(0xC4))
#define __OSSystemInterruptMask (*(OSInterruptMask*)OSPhysicalToCached(0xC8))
#define __OSInterruptTable ((__OSInterruptHandler*)OSPhysicalToCached(0x3040))

static __OSInterruptHandler* InterruptHandlerTable;

static OSInterruptMask InterruptPrioTable[] = {
    OS_INTERRUPTMASK_PI_ERROR,
    OS_INTERRUPTMASK_PI_DEBUG,
    OS_INTERRUPTMASK_MEM,
    OS_INTERRUPTMASK_PI_RSW,
    OS_INTERRUPTMASK_PI_VI,
    OS_INTERRUPTMASK_PI_ACR,
    OS_INTERRUPTMASK_PI_PE,
    OS_INTERRUPTMASK_PI_HSP,
    OS_INTERRUPTMASK_DSP_ARAM | OS_INTERRUPTMASK_DSP_DSP | OS_INTERRUPTMASK_AI | OS_INTERRUPTMASK_EXI |
        OS_INTERRUPTMASK_PI_SI | OS_INTERRUPTMASK_PI_DI,
    OS_INTERRUPTMASK_DSP_AI,
    OS_INTERRUPTMASK_PI_CP,
    0xFFFFFFFF,
};

static asm void ExternalInterruptHandler(register __OSException exception, register OSContext* context);

asm BOOL OSDisableInterrupts(void) {
    nofralloc
entry __RAS_OSDisableInterrupts_begin
    mfmsr r3
    rlwinm r4, r3, 0, 17, 15
    mtmsr r4
entry __RAS_OSDisableInterrupts_end
    extrwi r3, r3, 1, 16
    blr
}

asm BOOL OSEnableInterrupts(void) {
    nofralloc
    mfmsr r3
    ori r4, r3, 0x8000
    mtmsr r4
    extrwi r3, r3, 1, 16
    blr
}

asm BOOL OSRestoreInterrupts(register BOOL level) {
    nofralloc
    cmpwi level, 0
    mfmsr r4
    beq disable
    ori r5, r4, 0x8000
    b restore
disable:
    rlwinm r5, r4, 0, 17, 15
restore:
    mtmsr r5
    extrwi r3, r4, 1, 16
    blr
}

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler) {
    __OSInterruptHandler oldHandler = InterruptHandlerTable[interrupt];
    InterruptHandlerTable[interrupt] = handler;
    return oldHandler;
}

__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt) {
    return InterruptHandlerTable[interrupt];
}

void __OSInterruptInit(void) {
    InterruptHandlerTable = __OSInterruptTable;
    memset(InterruptHandlerTable, 0, 32 * sizeof(__OSInterruptHandler));
    __OSUserInterruptMask = 0;
    __OSSystemInterruptMask = 0;
    __PIRegs[1] = 0xF0;
    __ACRRegs[0xD] = 0x40000000;
    __OSMaskInterrupts(OS_INTERRUPTMASK_MEM | OS_INTERRUPTMASK_DSP | OS_INTERRUPTMASK_AI | OS_INTERRUPTMASK_EXI |
                       OS_INTERRUPTMASK_PI);
    __OSSetExceptionHandler(__OS_EXCEPTION_EXTERNAL_INTERRUPT, ExternalInterruptHandler);
}

static OSInterruptMask SetInterruptMask(OSInterruptMask mask, OSInterruptMask current) {
    u32 reg;

    switch (__cntlzw(mask)) {
    case __OS_INTERRUPT_MEM_0:
    case __OS_INTERRUPT_MEM_1:
    case __OS_INTERRUPT_MEM_2:
    case __OS_INTERRUPT_MEM_3:
    case __OS_INTERRUPT_MEM_ADDRESS:
        reg = 0;
        if (!(current & OS_INTERRUPTMASK_MEM_0)) {
            reg |= 0x1;
        }
        if (!(current & OS_INTERRUPTMASK_MEM_1)) {
            reg |= 0x2;
        }
        if (!(current & OS_INTERRUPTMASK_MEM_2)) {
            reg |= 0x4;
        }
        if (!(current & OS_INTERRUPTMASK_MEM_3)) {
            reg |= 0x8;
        }
        if (!(current & OS_INTERRUPTMASK_MEM_ADDRESS)) {
            reg |= 0x10;
        }
        __MEMRegs[0xE] = (u16)reg;
        mask &= ~OS_INTERRUPTMASK_MEM;
        break;

    case __OS_INTERRUPT_DSP_AI:
    case __OS_INTERRUPT_DSP_ARAM:
    case __OS_INTERRUPT_DSP_DSP:
        reg = __DSPRegs[5];
        reg &= ~0x1F8;
        if (!(current & OS_INTERRUPTMASK_DSP_AI)) {
            reg |= 0x10;
        }
        if (!(current & OS_INTERRUPTMASK_DSP_ARAM)) {
            reg |= 0x40;
        }
        if (!(current & OS_INTERRUPTMASK_DSP_DSP)) {
            reg |= 0x100;
        }
        __DSPRegs[5] = (u16)reg;
        mask &= ~OS_INTERRUPTMASK_DSP;
        break;

    case __OS_INTERRUPT_AI_AI:
        reg = __AIRegs[0];
        reg &= ~0x2C;
        if (!(current & OS_INTERRUPTMASK_AI_AI)) {
            reg |= 0x4;
        }
        __AIRegs[0] = reg;
        mask &= ~OS_INTERRUPTMASK_AI;
        break;

    case __OS_INTERRUPT_EXI_0_EXI:
    case __OS_INTERRUPT_EXI_0_TC:
    case __OS_INTERRUPT_EXI_0_EXT:
        reg = __EXIRegs[0];
        reg &= ~0x2C0F;
        if (!(current & OS_INTERRUPTMASK_EXI_0_EXI)) {
            reg |= 0x1;
        }
        if (!(current & OS_INTERRUPTMASK_EXI_0_TC)) {
            reg |= 0x4;
        }
        if (!(current & OS_INTERRUPTMASK_EXI_0_EXT)) {
            reg |= 0x400;
        }
        __EXIRegs[0] = reg;
        mask &= ~(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_0_TC | OS_INTERRUPTMASK_EXI_0_EXT);
        break;

    case __OS_INTERRUPT_EXI_1_EXI:
    case __OS_INTERRUPT_EXI_1_TC:
    case __OS_INTERRUPT_EXI_1_EXT:
        reg = __EXIRegs[5];
        reg &= ~0xC0F;
        if (!(current & OS_INTERRUPTMASK_EXI_1_EXI)) {
            reg |= 0x1;
        }
        if (!(current & OS_INTERRUPTMASK_EXI_1_TC)) {
            reg |= 0x4;
        }
        if (!(current & OS_INTERRUPTMASK_EXI_1_EXT)) {
            reg |= 0x400;
        }
        __EXIRegs[5] = reg;
        mask &= ~(OS_INTERRUPTMASK_EXI_1_EXI | OS_INTERRUPTMASK_EXI_1_TC | OS_INTERRUPTMASK_EXI_1_EXT);
        break;

    case __OS_INTERRUPT_EXI_2_EXI:
    case __OS_INTERRUPT_EXI_2_TC:
        reg = __EXIRegs[10];
        reg &= ~0xF;
        if (!(current & OS_INTERRUPTMASK_EXI_2_EXI)) {
            reg |= 0x1;
        }
        if (!(current & OS_INTERRUPTMASK_EXI_2_TC)) {
            reg |= 0x4;
        }
        __EXIRegs[10] = reg;
        mask &= ~(OS_INTERRUPTMASK_EXI_2_EXI | OS_INTERRUPTMASK_EXI_2_TC);
        break;

    case __OS_INTERRUPT_PI_CP:
    case __OS_INTERRUPT_PI_SI:
    case __OS_INTERRUPT_PI_DI:
    case __OS_INTERRUPT_PI_RSW:
    case __OS_INTERRUPT_PI_ERROR:
    case __OS_INTERRUPT_PI_VI:
    case __OS_INTERRUPT_PI_DEBUG:
    case __OS_INTERRUPT_PI_PE_TOKEN:
    case __OS_INTERRUPT_PI_PE_FINISH:
    case __OS_INTERRUPT_PI_HSP:
    case __OS_INTERRUPT_PI_ACR:
        reg = 0xF0;
        if (!(current & OS_INTERRUPTMASK_PI_CP)) {
            reg |= 0x800;
        }
        if (!(current & OS_INTERRUPTMASK_PI_SI)) {
            reg |= 0x8;
        }
        if (!(current & OS_INTERRUPTMASK_PI_DI)) {
            reg |= 0x4;
        }
        if (!(current & OS_INTERRUPTMASK_PI_RSW)) {
            reg |= 0x2;
        }
        if (!(current & OS_INTERRUPTMASK_PI_ERROR)) {
            reg |= 0x1;
        }
        if (!(current & OS_INTERRUPTMASK_PI_VI)) {
            reg |= 0x100;
        }
        if (!(current & OS_INTERRUPTMASK_PI_DEBUG)) {
            reg |= 0x1000;
        }
        if (!(current & OS_INTERRUPTMASK_PI_PE_TOKEN)) {
            reg |= 0x200;
        }
        if (!(current & OS_INTERRUPTMASK_PI_PE_FINISH)) {
            reg |= 0x400;
        }
        if (!(current & OS_INTERRUPTMASK_PI_HSP)) {
            reg |= 0x2000;
        }
        if (!(current & OS_INTERRUPTMASK_PI_ACR)) {
            reg |= 0x4000;
        }
        __PIRegs[1] = reg;
        mask &= ~OS_INTERRUPTMASK_PI;
        break;

    default:
        break;
    }

    return mask;
}

OSInterruptMask __OSMaskInterrupts(OSInterruptMask global) {
    BOOL enabled;
    OSInterruptMask prev;
    OSInterruptMask local;
    OSInterruptMask mask;

    enabled = OSDisableInterrupts();
    prev = __OSUserInterruptMask;
    local = __OSSystemInterruptMask;
    mask = ~(prev | local) & global;
    global |= prev;
    __OSUserInterruptMask = global;

    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }

    OSRestoreInterrupts(enabled);
    return prev;
}

OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask global) {
    BOOL enabled;
    OSInterruptMask prev;
    OSInterruptMask local;
    OSInterruptMask mask;

    enabled = OSDisableInterrupts();
    prev = __OSUserInterruptMask;
    local = __OSSystemInterruptMask;
    mask = (prev | local) & global;
    global = prev & ~global;
    __OSUserInterruptMask = global;

    while (mask) {
        mask = SetInterruptMask(mask, global | local);
    }

    OSRestoreInterrupts(enabled);
    return prev;
}

volatile OSTime __OSLastInterruptTime;
volatile __OSInterrupt __OSLastInterrupt;
volatile u32 __OSLastInterruptSrr0;

void __OSDispatchInterrupt(__OSException exception, OSContext* context) {
    u32 intsr;
    u32 intmr;
    u32 reg;
    OSInterruptMask cause;
    OSInterruptMask unmasked;
    OSInterruptMask* prio;
    __OSInterrupt interrupt;
    __OSInterruptHandler handler;

    intsr = __PIRegs[0];
    intsr &= ~0x10000;
    intmr = __PIRegs[1];

    if (intsr == 0 || (intsr & intmr) == 0) {
        OSLoadContext(context);
    }

    cause = 0;

    if (intsr & 0x80) {
        reg = __MEMRegs[0xF];
        if (reg & 0x1) {
            cause |= OS_INTERRUPTMASK_MEM_0;
        }
        if (reg & 0x2) {
            cause |= OS_INTERRUPTMASK_MEM_1;
        }
        if (reg & 0x4) {
            cause |= OS_INTERRUPTMASK_MEM_2;
        }
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_MEM_3;
        }
        if (reg & 0x10) {
            cause |= OS_INTERRUPTMASK_MEM_ADDRESS;
        }
    }

    if (intsr & 0x40) {
        reg = __DSPRegs[5];
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_DSP_AI;
        }
        if (reg & 0x20) {
            cause |= OS_INTERRUPTMASK_DSP_ARAM;
        }
        if (reg & 0x80) {
            cause |= OS_INTERRUPTMASK_DSP_DSP;
        }
    }

    if (intsr & 0x20) {
        reg = __AIRegs[0];
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_AI_AI;
        }
    }

    if (intsr & 0x10) {
        reg = __EXIRegs[0];
        if (reg & 0x2) {
            cause |= OS_INTERRUPTMASK_EXI_0_EXI;
        }
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_EXI_0_TC;
        }
        if (reg & 0x800) {
            cause |= OS_INTERRUPTMASK_EXI_0_EXT;
        }

        reg = __EXIRegs[5];
        if (reg & 0x2) {
            cause |= OS_INTERRUPTMASK_EXI_1_EXI;
        }
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_EXI_1_TC;
        }
        if (reg & 0x800) {
            cause |= OS_INTERRUPTMASK_EXI_1_EXT;
        }

        reg = __EXIRegs[10];
        if (reg & 0x2) {
            cause |= OS_INTERRUPTMASK_EXI_2_EXI;
        }
        if (reg & 0x8) {
            cause |= OS_INTERRUPTMASK_EXI_2_TC;
        }
    }

    if (intsr & 0x2000) {
        cause |= OS_INTERRUPTMASK_PI_HSP;
    }
    if (intsr & 0x1000) {
        cause |= OS_INTERRUPTMASK_PI_DEBUG;
    }
    if (intsr & 0x400) {
        cause |= OS_INTERRUPTMASK_PI_PE_FINISH;
    }
    if (intsr & 0x200) {
        cause |= OS_INTERRUPTMASK_PI_PE_TOKEN;
    }
    if (intsr & 0x100) {
        cause |= OS_INTERRUPTMASK_PI_VI;
    }
    if (intsr & 0x8) {
        cause |= OS_INTERRUPTMASK_PI_SI;
    }
    if (intsr & 0x4) {
        cause |= OS_INTERRUPTMASK_PI_DI;
    }
    if (intsr & 0x2) {
        cause |= OS_INTERRUPTMASK_PI_RSW;
    }
    if (intsr & 0x800) {
        cause |= OS_INTERRUPTMASK_PI_CP;
    }
    if (intsr & 0x1) {
        cause |= OS_INTERRUPTMASK_PI_ERROR;
    }
    if (intsr & 0x4000) {
        cause |= OS_INTERRUPTMASK_PI_ACR;
    }

    unmasked = cause & ~(__OSUserInterruptMask | __OSSystemInterruptMask);

    if (unmasked) {
        for (prio = InterruptPrioTable;; ++prio) {
            if (unmasked & *prio) {
                interrupt = (__OSInterrupt)__cntlzw(unmasked & *prio);
                break;
            }
        }

        handler = __OSGetInterruptHandler(interrupt);
        if (handler) {
            if (interrupt > __OS_INTERRUPT_MEM_ADDRESS) {
                __OSLastInterrupt = interrupt;
                __OSLastInterruptTime = OSGetTime();
                __OSLastInterruptSrr0 = context->srr0;
            }

            OSDisableScheduler();
            handler(interrupt, context);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
    }

    OSLoadContext(context);
}

static asm void ExternalInterruptHandler(register __OSException exception, register OSContext* context) {
    nofralloc
    stw r0, OS_CONTEXT_R0(context)
    stw r1, OS_CONTEXT_R1(context)
    stw r2, OS_CONTEXT_R2(context)
    stmw r6, OS_CONTEXT_R6(context)
    mfspr r0, GQR1
    stw r0, OS_CONTEXT_GQR1(context)
    mfspr r0, GQR2
    stw r0, OS_CONTEXT_GQR2(context)
    mfspr r0, GQR3
    stw r0, OS_CONTEXT_GQR3(context)
    mfspr r0, GQR4
    stw r0, OS_CONTEXT_GQR4(context)
    mfspr r0, GQR5
    stw r0, OS_CONTEXT_GQR5(context)
    mfspr r0, GQR6
    stw r0, OS_CONTEXT_GQR6(context)
    mfspr r0, GQR7
    stw r0, OS_CONTEXT_GQR7(context)
    stwu r1, -8(r1)
    b __OSDispatchInterrupt
}
