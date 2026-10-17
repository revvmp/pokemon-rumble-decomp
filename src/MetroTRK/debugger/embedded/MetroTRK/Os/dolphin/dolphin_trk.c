#include <MetroTRK/Os/dolphin/dolphin_trk.h>
#include <MetroTRK/Os/dolphin/dolphin_trk_glue.h>
#include <MetroTRK/Portable/main_TRK.h>
#include <MetroTRK/Portable/mem_TRK.h>
#include <MetroTRK/ppc/Generic/flush_cache.h>
#include <MetroTRK/ppc/Generic/targimpl.h>
#include <revolution/os.h>

static u32 lc_base;
static u32 TRK_ISR_OFFSETS[15] = {PPC_SystemReset,
                                  PPC_MachineCheck,
                                  PPC_DataStorage,
                                  PPC_InstructionStorage,
                                  PPC_ExternalInterrupt,
                                  PPC_Alignment,
                                  PPC_Program,
                                  PPC_FloatingPointUnavaiable,
                                  PPC_Decrementer,
                                  PPC_SystemCall,
                                  PPC_Trace,
                                  PPC_PerformanceMonitor,
                                  PPC_InstructionAddressBreakpoint,
                                  PPC_SystemManagementInterrupt,
                                  PPC_ThermalManagementInterrupt};

asm void InitMetroTRK(void) {
    nofralloc
	addi r1, r1, -4
	stw r3, 0(r1)
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	stmw r0, ProcessorState_PPC.Default.GPR(r3)
	lwz r4, 0(r1)
	addi r1, r1, 4
	stw r1, ProcessorState_PPC.Default.GPR[1](r3)
	stw r4, ProcessorState_PPC.Default.GPR[3](r3)
	mflr r4
	stw r4, ProcessorState_PPC.Default.LR(r3)
	stw r4, ProcessorState_PPC.Default.PC(r3)
	mfcr r4
	stw r4, ProcessorState_PPC.Default.CR(r3)
	mfmsr r4
	ori r3, r4, (1 << (31 - 16))
	xori r3, r3, (1 << (31 - 16))
	mtmsr r3
	mtsrr1 r4
	bl TRKSaveExtended1Block
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	lmw r0, ProcessorState_PPC.Default.GPR(r3)
	li r0, 0
	mtspr  0x3f2, r0
	mtspr  0x3f5, r0
	lis r1, _db_stack_addr@h
	ori r1, r1, _db_stack_addr@l
	mr r3, r5
	bl InitMetroTRKCommTable
	cmpwi r3, 1
	bne initCommTableSuccess
	lwz r4, ProcessorState_PPC.Default.LR(r3)
	mtlr r4
	lmw r0, ProcessorState_PPC.Default.GPR(r3)
	blr
initCommTableSuccess:
	b TRK_main
	blr
}
asm void InitMetroTRK_BBA(void) {
    nofralloc
	addi r1, r1, -4
	stw r3, 0(r1)
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	stmw r0, ProcessorState_PPC.Default.GPR(r3)
	lwz r4, 0(r1)
	addi r1, r1, 4
	stw r1, ProcessorState_PPC.Default.GPR[1](r3)
	stw r4, ProcessorState_PPC.Default.GPR[3](r3)
	mflr r4
	stw r4, ProcessorState_PPC.Default.LR(r3)
	stw r4, ProcessorState_PPC.Default.PC(r3)
	mfcr r4
	stw r4, ProcessorState_PPC.Default.CR(r3)
	mfmsr r4
	ori r3, r4, (1 << (31 - 16))
	mtmsr r3
	mtsrr1 r4
	bl TRKSaveExtended1Block
	lis r3, gTRKCPUState@h
	ori r3, r3, gTRKCPUState@l
	lmw r0, ProcessorState_PPC.Default.GPR(r3)
	li r0, 0
	mtspr  0x3f2, r0
	mtspr 0x3f5, r0
	lis r1, _db_stack_addr@h
	ori r1, r1, _db_stack_addr@l
	li r3, 2
	bl InitMetroTRKCommTable
	cmpwi r3, 1
	bne initCommTableSuccess
	lwz r4, ProcessorState_PPC.Default.LR(r3)
	mtlr r4
	lmw r0, ProcessorState_PPC.Default.GPR(r3)
	blr
initCommTableSuccess:
	b TRK_main
	blr
}
void EnableMetroTRKInterrupts(void) {
    EnableEXI2Interrupts();
}
u32 TRKTargetTranslate(u32 address) {
    if (address >= lc_base) {
        if ((address < lc_base + 0x4000) && ((gTRKCPUState.Extended1.DBAT3U & 3) != 0)) {
            return address;
        }
    }
if (address < 0x03000000) return address & 0x3FFFFFFF | 0x80000000;
if ((const void*)address >= (const void*)0x10000000) {
        if ((const void*)address < (const void*)0x1C000000) {
            return (address & 0x3FFFFFFF) | 0x90000000;
        }
    }
return address;
}
void TRK_copy_vector(u32 offset) {
    void* destPtr = (void*)TRKTargetTranslate(offset);
    TRK_memcpy(destPtr, (void*)(gTRKInterruptVectorTable + offset), 0x100);
    TRK_flush_cache((u32)destPtr, 0x100);
}
void __TRK_copy_vectors(void) {
    u32 maskAddress = lc_base;
    u32* isrOffsetPtr;
    int i;
    u32 exceptionMask;

    if (maskAddress <= 0x44 && maskAddress + 0x4000 > 0x44 && gTRKCPUState.Extended1.DBAT3U & 3) {
        maskAddress = 0x44;
    } else {
        maskAddress = 0x80000044;
    }

    i = 0;
    exceptionMask = *(u32*)maskAddress;
    isrOffsetPtr = TRK_ISR_OFFSETS;

    do {
        if ((exceptionMask & (1 << i)) && i != 4) {
            TRK_copy_vector(isrOffsetPtr[i]);
        }

        i++;
    } while (i <= 14);
}
DSError TRKInitializeTarget(void) {
    gTRKState.isStopped = TRUE;
    gTRKState.msr = __TRK_get_MSR();
    lc_base = 0xE0000000;
    return DS_NoError;
}
void __TRK_reset(void) {
    OSResetSystem(FALSE, 0, FALSE);
}
