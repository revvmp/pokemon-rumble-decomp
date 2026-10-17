#include "revolution/os/OSMemory.h"
#include "private/flipper.h"
#include "revolution/os/OSAddress.h"
#include "revolution/os/OSError.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSReset.h"

#define OS_MEMORY_CONFIG_KEY 0xBA2CF

static BOOL OnShutdown(BOOL final, u32 event);

static OSShutdownFunctionInfo ShutdownFunctionInfo = {
    OnShutdown,
    127,
    NULL,
    NULL,
};

u32 OSGetPhysicalMem1Size(void) {
    return *(u32*)OSPhysicalToCached(0x3100);
}

u32 OSGetPhysicalMem2Size(void) {
    return *(u32*)OSPhysicalToCached(0x3118);
}

u32 OSGetConsoleSimulatedMem1Size(void) {
    return *(u32*)OSPhysicalToCached(0x3104);
}

u32 OSGetConsoleSimulatedMem2Size(void) {
    return *(u32*)OSPhysicalToCached(0x311C);
}

static BOOL OnShutdown(BOOL final, u32 event) {
    if (final) {
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                           OS_INTERRUPTMASK_MEM_3);
    }

    return TRUE;
}

static void MEMIntrruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 cause;
    u32 addr;

    cause = __MEMRegs[0xF];
    addr = ((u32)(__MEMRegs[0x12] & 0x3FF) << 16) | __MEMRegs[0x11];
    __MEMRegs[0x10] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION]) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, addr);
        return;
    }

    __OSUnhandledException(OS_ERROR_PROTECTION, context, cause, addr);
}

static asm void ConfigMEM1_24MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x00000002@ha
    addi r4, r4, 0x00000002@l
    lis r3, 0x800001FF@ha
    addi r3, r3, 0x800001FF@l
    lis r6, 0x01000002@ha
    addi r6, r6, 0x01000002@l
    lis r5, 0x810000FF@ha
    addi r5, r5, 0x810000FF@l
    isync
    mtspr DBAT0U, r7
    mtspr DBAT0L, r4
    mtspr DBAT0U, r3
    isync
    mtspr IBAT0U, r7
    mtspr IBAT0L, r4
    mtspr IBAT0U, r3
    isync
    mtspr DBAT2U, r7
    mtspr DBAT2L, r6
    mtspr DBAT2U, r5
    isync
    mtspr IBAT2U, r7
    mtspr IBAT2L, r6
    mtspr IBAT2U, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_52MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr DBAT4U, r7
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4U, r7
    mtspr IBAT4L, r7
    isync
    mtspr DBAT5U, r7
    mtspr DBAT5L, r6
    mtspr DBAT5U, r5
    isync
    mtspr IBAT5U, r7
    mtspr IBAT5L, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x9300007F@ha
    addi r5, r5, 0x9300007F@l
    isync
    mtspr DBAT6U, r7
    mtspr DBAT6L, r4
    mtspr DBAT6U, r3
    isync
    mtspr IBAT6U, r7
    mtspr IBAT6L, r7
    isync
    mtspr DBAT7U, r7
    mtspr DBAT7L, r6
    mtspr DBAT7U, r5
    isync
    mtspr IBAT7U, r7
    mtspr IBAT7L, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_56MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900003FF@ha
    addi r3, r3, 0x900003FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr DBAT4U, r7
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4U, r7
    mtspr IBAT4L, r7
    isync
    mtspr DBAT5U, r7
    mtspr DBAT5L, r6
    mtspr DBAT5U, r5
    isync
    mtspr IBAT5U, r7
    mtspr IBAT5L, r7
    isync
    lis r4, 0x12000002@ha
    addi r4, r4, 0x12000002@l
    lis r3, 0x920001FF@ha
    addi r3, r3, 0x920001FF@l
    lis r6, 0x13000002@ha
    addi r6, r6, 0x13000002@l
    lis r5, 0x930000FF@ha
    addi r5, r5, 0x930000FF@l
    isync
    mtspr DBAT6U, r7
    mtspr DBAT6L, r4
    mtspr DBAT6U, r3
    isync
    mtspr IBAT6U, r7
    mtspr IBAT6L, r7
    isync
    mtspr DBAT7U, r7
    mtspr DBAT7L, r6
    mtspr DBAT7U, r5
    isync
    mtspr IBAT7U, r7
    mtspr IBAT7L, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_64MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD00007FF@ha
    addi r5, r5, 0xD00007FF@l
    isync
    mtspr DBAT4U, r7
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4U, r7
    mtspr IBAT4L, r7
    isync
    mtspr DBAT5U, r7
    mtspr DBAT5L, r6
    mtspr DBAT5U, r5
    isync
    mtspr IBAT5U, r7
    mtspr IBAT5L, r7
    isync
    mtspr IBAT6U, r7
    mtspr IBAT6L, r7
    isync
    mtspr IBAT7U, r7
    mtspr IBAT7L, r7
    isync
    mtspr DBAT6U, r7
    mtspr DBAT6L, r7
    isync
    mtspr DBAT7U, r7
    mtspr DBAT7L, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_112MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x900007FF@ha
    addi r3, r3, 0x900007FF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr DBAT4U, r7
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4U, r7
    mtspr IBAT4L, r7
    isync
    mtspr DBAT5U, r7
    mtspr DBAT5L, r6
    mtspr DBAT5U, r5
    isync
    mtspr IBAT5U, r7
    mtspr IBAT5L, r7
    isync
    lis r4, 0x14000002@ha
    addi r4, r4, 0x14000002@l
    lis r3, 0x940003FF@ha
    addi r3, r3, 0x940003FF@l
    lis r6, 0x16000002@ha
    addi r6, r6, 0x16000002@l
    lis r5, 0x960001FF@ha
    addi r5, r5, 0x960001FF@l
    isync
    mtspr DBAT6U, r7
    mtspr DBAT6L, r4
    mtspr DBAT6U, r3
    isync
    mtspr IBAT6U, r7
    mtspr IBAT6L, r7
    isync
    mtspr DBAT7U, r7
    mtspr DBAT7L, r6
    mtspr DBAT7U, r5
    isync
    mtspr IBAT7U, r7
    mtspr IBAT7L, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void ConfigMEM2_128MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x10000002@ha
    addi r4, r4, 0x10000002@l
    lis r3, 0x90000FFF@ha
    addi r3, r3, 0x90000FFF@l
    lis r6, 0x1000002A@ha
    addi r6, r6, 0x1000002A@l
    lis r5, 0xD0000FFF@ha
    addi r5, r5, 0xD0000FFF@l
    isync
    mtspr DBAT4U, r7
    mtspr DBAT4L, r4
    mtspr DBAT4U, r3
    isync
    mtspr IBAT4U, r7
    mtspr IBAT4L, r7
    isync
    mtspr DBAT5U, r7
    mtspr DBAT5L, r6
    mtspr DBAT5U, r5
    isync
    mtspr IBAT5U, r7
    mtspr IBAT5L, r7
    isync
    mtspr IBAT6U, r7
    mtspr IBAT6L, r7
    isync
    mtspr IBAT7U, r7
    mtspr IBAT7L, r7
    isync
    mtspr DBAT6U, r7
    mtspr DBAT6L, r7
    isync
    mtspr DBAT7U, r7
    mtspr DBAT7L, r7
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void RealMode(register u32 config) {
    nofralloc
    clrlwi config, config, 2
    mtsrr0 config
    mfmsr config
    rlwinm config, config, 0, 28, 25
    mtsrr1 config
    rfi
}

static void BATConfig(u32 key) {
    u32 mem1Size;
    u32 mem2Size;
    u32 mem2End;

    mem1Size = OSGetConsoleSimulatedMem1Size();
    if (mem1Size < OSGetPhysicalMem1Size() && mem1Size == 0x1800000) {
        DCInvalidateRange((void*)0x81800000, 0x1800000);
        __MEMRegs[0x14] = 2;
    }

    if (mem1Size <= 0x1800000) {
        RealMode((u32)ConfigMEM1_24MB);
    }

    mem2Size = OSGetConsoleSimulatedMem2Size();
    mem2End = *(u32*)OSPhysicalToCached(0x3120);

    if (mem2Size <= 0x4000000) {
        if (mem2End <= 0x93400000) {
            RealMode((u32)ConfigMEM2_52MB);
        } else if (mem2End <= 0x93800000) {
            RealMode((u32)ConfigMEM2_56MB);
        } else {
            RealMode((u32)ConfigMEM2_64MB);
        }
    } else if (mem2Size <= 0x8000000) {
        if (mem2End <= 0x97000000) {
            RealMode((u32)ConfigMEM2_112MB);
        } else {
            RealMode((u32)ConfigMEM2_128MB);
        }
    }

    while (key != OS_MEMORY_CONFIG_KEY) {
    }
}

void __OSRestoreMEM1Config(u32 key) {
    RealMode((u32)ConfigMEM1_24MB);

    while (key != OS_MEMORY_CONFIG_KEY) {
    }
}

void __OSInitMemoryProtection(void) {
    static BOOL initialized = FALSE;
    BOOL enabled;

    if (!initialized) {
        enabled = OSDisableInterrupts();
        __MEMRegs[0x10] = 0;
        __MEMRegs[8] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0 | OS_INTERRUPTMASK_MEM_1 | OS_INTERRUPTMASK_MEM_2 |
                           OS_INTERRUPTMASK_MEM_3);
        __OSSetInterruptHandler(__OS_INTERRUPT_MEM_0, MEMIntrruptHandler);
        __OSSetInterruptHandler(__OS_INTERRUPT_MEM_1, MEMIntrruptHandler);
        __OSSetInterruptHandler(__OS_INTERRUPT_MEM_2, MEMIntrruptHandler);
        __OSSetInterruptHandler(__OS_INTERRUPT_MEM_3, MEMIntrruptHandler);
        __OSSetInterruptHandler(__OS_INTERRUPT_MEM_ADDRESS, MEMIntrruptHandler);
        OSRegisterShutdownFunction(&ShutdownFunctionInfo);
        BATConfig(OS_MEMORY_CONFIG_KEY);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);
        initialized = TRUE;
        OSRestoreInterrupts(enabled);
    }
}
