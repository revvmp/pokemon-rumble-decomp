#include <MetroTRK/ppc/Generic/targimpl.h>
#include <MetroTRK/Portable/msgbuf.h>
#include <MetroTRK/Portable/support.h>
#include <MetroTRK/Portable/notify.h>
#include <MetroTRK/Portable/mem_TRK.h>
#include <MetroTRK/ppc/Generic/flush_cache.h>
#include <MetroTRK/ppc/Generic/mpc_7xx_603e.h>
#include <MetroTRK/Os/dolphin/dolphin_trk.h>
#include <MetroTRK/Os/dolphin/dolphin_trk_glue.h>
#include <MetroTRK/ppc/Export/targsupp.h>
#include <revolution/os/OSThread.h>

enum {
    TRKRestoreTimeBase,
    TRKRestoreDecrementer,
    SPR_GQR0 = 912,
    SPR_HID2 = 920,
    SPR_FPECR = 1022,
};

static void GetThreadInfo(int*, int*);
DSError TRKAppendBuffer1_ui32(TRKBuffer*,u32);
DSError TRKAppendBuffer(TRKBuffer*,const void*,size_t);

typedef struct memRange {
      u8* start;
      u8* end;
      BOOL readable;
      BOOL writeable;
} memRange;

const memRange gTRKMemMap[1] = {{(u8*)0, (u8*)-1, TRUE, TRUE}};

typedef struct StopInfo_PPC {
      u32 PC;
      u32 PCInstruction;
      u16 exceptionID;
} StopInfo_PPC;

typedef struct TRKExceptionStatus {
      StopInfo_PPC exceptionInfo;
      u8 inTRK;
      u8 exceptionDetected;
} TRKExceptionStatus;

typedef struct TRKStepStatus {
      BOOL active;
      DSMessageStepOptions type;
      u32 count;
      u32 rangeStart;
      u32 rangeEnd;
} ATTRIBUTE_ALIGN(8) TRKStepStatus;

u8 gTRKRestoreFlags[9] = {FALSE, FALSE};

static TRKExceptionStatus gTRKExceptionStatus = {{0, 0, 0}, TRUE, 0};

static TRKStepStatus gTRKStepStatus = {FALSE, DSSTEP_IntoCount, 0, 0};

static u16 TRK_saved_exceptionID = 0;
Default_PPC gTRKSaveState = {0};
typedef unsigned char u128[16];
u128 TRKvalue128_temp = {0};
TRKState gTRKState = {0};
ProcessorState_PPC gTRKCPUState = {0};

#define INSTR_NOP 0x60000000
#define INSTR_BLR 0x4E800020
#define INSTR_PSQ_ST(psr, offset, rDest, w, gqr) (0xF0000000 | (psr << 21) | (rDest << 16) | (w << 15) | (gqr << 12) | offset)
#define INSTR_PSQ_L(psr, offset, rSrc, w, gqr) (0xE0000000 | (psr << 21) | (rSrc << 16) | (w << 15) | (gqr << 12) | offset)
#define INSTR_STW(rSrc, offset, rDest) (0x90000000 | (rSrc << 21) | (rDest << 16) | offset)
#define INSTR_LWZ(rDest, offset, rSrc) (0x80000000 | (rDest << 21) | (rSrc << 16) | offset)
#define INSTR_STFD(fprSrc, offset, rDest) (0xD8000000 | (fprSrc << 21) | (rDest << 16) | offset)
#define INSTR_LFD(fprDest, offset, rSrc) (0xC8000000 | (fprDest << 21) | (rSrc << 16) | offset)
#define INSTR_MFSPR(rDest, spr) (0x7C000000 | (rDest << 21) | ((spr & 0xFE0) << 6) | ((spr & 0x1F) << 16) | 0x2A6)
#define INSTR_MTSPR(spr, rSrc) (0x7C000000 | (rSrc << 21) | ((spr & 0xFE0) << 6) | ((spr & 0x1F) << 16) | 0x3A6)

#define DSFetch_u32(_p_) (*((u32*)_p_))
#define DSFetch_u64(_p_) (*((u64*)_p_))

DSError TRKPPCAccessSPR(void* value, u32 spr, BOOL read);
DSError TRKPPCAccessPairedSingleRegister(void* value, u32 psr, BOOL read);
DSError TRKPPCAccessFPRegister(void* value, u32 fpr, BOOL read);
DSError TRKPPCAccessSpecialReg(void* value, u32* accessFunction, BOOL read);
static void TRKExceptionHandler(u16);
void TRKInterruptHandlerEnableInterrupts(void);
void WriteFPSCR(register f64*);
void ReadFPSCR(register f64*);
void __TRK_set_MSR(register u32 msr);
u32 __TRK_get_MSR();
static void TRK_ppc_memcpy(register void* dest, register const void* src, register int n, register u32 destinationMSR, register u32 sourceMSR);

void TRKRestoreExtended1Block();
void TRKUARTInterruptHandler();

u32 ConvertAddress(u32 addr);

inline DSError TRKTargetReadInstruction(void* data, u32 start) {
    DSError error;
    size_t registersLength = 4;

    error = TRKTargetAccessMemory(data, start, &registersLength, MEMACCESS_UserMemory, TRUE);

    if (error == DS_NoError && registersLength != 4) {
        error = DS_InvalidMemory;
    }

    return error;
}

inline DSError TRKTargetEnableTrace(BOOL val) {
    if (val) {
        gTRKCPUState.Extended1.MSR = ((gTRKCPUState.Extended1.MSR | 0x400) & ~0x8000);
    } else {
        gTRKCPUState.Extended1.MSR = ((gTRKCPUState.Extended1.MSR & ~0x400) | 0x8000);
    }
    return DS_NoError;
}

inline BOOL TRKTargetStepDone() {
    BOOL result = TRUE;

    if (gTRKStepStatus.active && ((u16)gTRKCPUState.Extended1.exceptionID) == PPC_Trace) {
        switch (gTRKStepStatus.type) {
        case DSSTEP_IntoCount:
            if (gTRKStepStatus.count > 0) {
                result = FALSE;
            }
            break;
        case DSSTEP_IntoRange:
            if (gTRKCPUState.Default.PC >= gTRKStepStatus.rangeStart && gTRKCPUState.Default.PC <= gTRKStepStatus.rangeEnd) {
                result = FALSE;
            }
            break;
        default:
            break;
        }
    }

    return result;
}

inline DSError TRKTargetDoStep() {
    gTRKStepStatus.active = TRUE;
    TRKTargetEnableTrace(TRUE);

    if (gTRKStepStatus.type == DSSTEP_IntoCount || gTRKStepStatus.type == DSSTEP_OverCount) {
        gTRKStepStatus.count--;
    }

    TRKTargetSetStopped(FALSE);
    return DS_NoError;
}

inline u8 TRKReadByte(const void* address) {
    u32 aligned = (u32)address & ~3;
    return (*(u32*)aligned >> ((3 - ((u32)address - aligned)) * 8)) & 0xFF;
}

inline void TRKWriteByte(void* address, u8 value) {
    u32 aligned = (u32)address & ~3;
    u32 offset = (u32)address - aligned;
    u32 original = *(u32*)aligned;
    u32 mask = 0xFF << ((3 - offset) * 8);
    *(u32*)aligned = (original & ~mask) | ((value << ((3 - offset) * 8)) & mask);
}

asm u32 __TRK_get_MSR() {
	nofralloc
	mfmsr r3
	blr
}

asm void __TRK_set_MSR(register u32) {
	nofralloc
	mtmsr r3
	blr
}

DSError TRKValidMemory32(const void* addr, size_t length, ValidMemoryOptions readWriteable) {
    DSError err = DS_InvalidMemory;

    const u8* start;
    const u8* end;

    s32 i;

    start = (const u8*)addr;
    end = ((const u8*)addr + (length - 1));

    if (end < start)
        return DS_InvalidMemory;

    for (i = 0; (i < (s32)(sizeof(gTRKMemMap) / sizeof(memRange))); i++) {

        if ((start <= (const u8*)gTRKMemMap[i].end) && (end >= (const u8*)gTRKMemMap[i].start)) {

            if (((readWriteable == VALIDMEM_Readable) && !gTRKMemMap[i].readable) ||
                ((readWriteable == VALIDMEM_Writeable) && !gTRKMemMap[i].writeable)) {
                err = DS_InvalidMemory;
            } else {
                err = DS_NoError;

                if (start < (const u8*)gTRKMemMap[i].start)
                    err = TRKValidMemory32(start, (u32)((const u8*)gTRKMemMap[i].start - start), readWriteable);

                if ((err == DS_NoError) && (end > (const u8*)gTRKMemMap[i].end))
                    err = TRKValidMemory32((const u8*)gTRKMemMap[i].end, (u32)(end - (const u8*)gTRKMemMap[i].end), readWriteable);
            }

            break;
        }
    }

    return err;
}

static void TRK_ppc_memcpy(register void* destination, register const void* source, int count, u32 destinationMSR, u32 sourceMSR) {
    u32 savedMSR;
    const u8* src = source;
    u8* dest = destination;
    u8 value;
    savedMSR = __TRK_get_MSR();
    while (count != 0) {
        __TRK_set_MSR(sourceMSR);
        value = TRKReadByte(src);
        asm { sync }
        __TRK_set_MSR(destinationMSR);
        TRKWriteByte(dest, value);
        asm { sync }
        src++;
        dest++;
        count--;
    }
    __TRK_set_MSR(savedMSR);
}

DSError TRKTargetAccessMemory(void* data, u32 start, size_t* length, MemoryAccessOptions accessOptions, BOOL read) {
    DSError error;
    u32 currentMSR;
    void* addr;
    u32 memoryMSR;
    TRKExceptionStatus tempExceptionStatus = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;

    addr = (void*)TRKTargetTranslate(start);
    error = TRKValidMemory32(addr, *length, read == FALSE);

    if (error != DS_NoError) {
        *length = 0;
    } else {
        currentMSR = __TRK_get_MSR();
        memoryMSR = currentMSR | gTRKCPUState.Extended1.MSR & 0x10;

        if (read) {
            TRK_ppc_memcpy(data, addr, *length, currentMSR, memoryMSR);
        } else {
            TRK_ppc_memcpy(addr, data, *length, memoryMSR, currentMSR);
            TRK_flush_cache((u32)addr, *length);
            if ((void*)start != addr) {
                TRK_flush_cache(start, *length);
            }
        }
    }

    if (gTRKExceptionStatus.exceptionDetected) {
        *length = 0;
        error = DS_CWDSException;
    }

    gTRKExceptionStatus = tempExceptionStatus;
    return error;
}

DSError TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read) {
    DSError error;
    u32 count;
    u32* data;
    TRKExceptionStatus tempExceptionStatus;

    if (lastRegister > 0x24) {
        return DS_InvalidRegister;
    }

    tempExceptionStatus = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;

    data = gTRKCPUState.Default.GPR + firstRegister;
    count = (lastRegister - firstRegister) + 1;
    *registersLengthPtr = count * sizeof(u32);

    if (read) {
        error = TRKAppendBuffer_ui32(b, data, count);
    } else {
        error = TRKReadBuffer_ui32(b, data, count);
    }

    if (gTRKExceptionStatus.exceptionDetected) {
        *registersLengthPtr = 0;
        error = DS_CWDSException;
    }

    gTRKExceptionStatus = tempExceptionStatus;
    return error;
}

DSError TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read) {
    u64 temp;
    DSError error;
    TRKExceptionStatus tempExceptionStatus;
    u32 current;

    if (lastRegister > 0x21) {
        return DS_InvalidRegister;
    }

    tempExceptionStatus = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;

    __TRK_set_MSR(__TRK_get_MSR() | 0x2000);

    *registersLengthPtr = 0;
    error = DS_NoError;

    for (current = firstRegister; (current <= lastRegister) && (error == DS_NoError); current++, *registersLengthPtr += sizeof(f64)) {
        if (read) {
            TRKPPCAccessFPRegister(&temp, current, read);
            error = TRKAppendBuffer1_ui64(b, temp);
        } else {
            TRKReadBuffer1_ui64(b, &temp);
            error = TRKPPCAccessFPRegister(&temp, current, read);
        }
    }

    if (gTRKExceptionStatus.exceptionDetected) {
        *registersLengthPtr = 0;
        error = DS_CWDSException;
    }

    gTRKExceptionStatus = tempExceptionStatus;
    return error;
}

DSError TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registersLengthPtr, BOOL read) {
    TRKExceptionStatus tempExceptionStatus;
    int error;
    u32* data;
    int count;

    if (lastRegister > 0x60) {
        return DS_InvalidRegister;
    }

    tempExceptionStatus = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;

    *registersLengthPtr = 0;

    if (firstRegister <= lastRegister) {
        data = (u32*)&gTRKCPUState.Extended1 + firstRegister;
        count = lastRegister - firstRegister + 1;
        *registersLengthPtr += count * sizeof(u32);

        if (read) {
            error = TRKAppendBuffer_ui32(b, data, count);
        } else {
            if (data <= &gTRKCPUState.Extended1.TBU && (data + count - 1) >= &gTRKCPUState.Extended1.TBL) {
                gTRKRestoreFlags[TRKRestoreTimeBase] = 1;
            }

            if (data <= &gTRKCPUState.Extended1.DEC && (data + count - 1) >= &gTRKCPUState.Extended1.DEC) {
                gTRKRestoreFlags[TRKRestoreDecrementer] = 1;
            }
            error = TRKReadBuffer_ui32(b, data, count);
        }
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *registersLengthPtr = 0;
        error = DS_CWDSException;
    }

    gTRKExceptionStatus = tempExceptionStatus;
    return error;
}

DSError TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer* b, size_t* registerStorageSize, BOOL read) {
    TRKExceptionStatus savedException;
    u32 i;
    u32 specialRegister[1];
    u32 registerValue[2];
    DSError err;
    u32 accessFunction[10];

    if (lastRegister > 0x1f)
        return DS_InvalidRegister;

    savedException = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;

    TRKPPCAccessSPR(specialRegister, SPR_HID2, TRUE);

    specialRegister[0] |= 0xA0000000;
    TRKPPCAccessSPR(specialRegister, SPR_HID2, FALSE);

    specialRegister[0] = 0;
    TRKPPCAccessSPR(specialRegister, SPR_GQR0, FALSE);

    *registerStorageSize = 0;
    err = DS_NoError;

    for (i = firstRegister; (i <= lastRegister) && (err == DS_NoError); i++) {
        if (read) {
            err = TRKPPCAccessPairedSingleRegister((u64*)registerValue, i, read);
            err = TRKAppendBuffer1_ui64(b, *(u64*)registerValue);
        } else {
            err = TRKReadBuffer1_ui64(b, (u64*)registerValue);
            err = TRKPPCAccessPairedSingleRegister((u64*)registerValue, i, read);
        }

        *registerStorageSize += sizeof(u64);
    }

    if (gTRKExceptionStatus.exceptionDetected) {
        *registerStorageSize = 0;
        err = DS_CWDSException;
    }

    gTRKExceptionStatus = savedException;

    return err;
}

asm void TRKInterruptHandler() {
	nofralloc
	mtsrr0 r2
	mtsrr1 r4
	mfsprg r4, 3
	mfcr r2
	mtsprg 3, r2
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	lwz r2, TRKState.msr(r2)
	ori r2, r2, 0x8002
	xori r2, r2, 0x8002
	sync
	mtmsr r2
	sync
	lis r2, TRK_saved_exceptionID@h
	ori r2, r2, TRK_saved_exceptionID@l
	sth r3, 0(r2)
	cmpwi r3, 0x500
	bne dispatchException
	lis r2, gTRKCPUState@h
	ori r2, r2, gTRKCPUState@l
	mflr r3
	stw r3, ProcessorState_PPC.transport_handler_saved_ra(r2)
	bl TRKUARTInterruptHandler
	lis r2, gTRKCPUState@h
	ori r2, r2, gTRKCPUState@l
	lwz r3, ProcessorState_PPC.transport_handler_saved_ra(r2)
	mtlr r3
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	lwz r2, TRKState.inputPendingPtr(r2)
	lbz r2, TRKState.gpr[0](r2)
	cmpwi r2, 0
	beq resumeExternalInterrupt
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	lbz r2, TRKExceptionStatus.inTRK(r2)
	cmpwi r2, 1
	beq resumeExternalInterrupt
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	li r3, 1
	stb r3, TRKState.inputActivated(r2)
	b dispatchException
resumeExternalInterrupt:
	lis r2, gTRKSaveState@h
	ori r2, r2, gTRKSaveState@l
	lwz r3, Default_PPC.CR(r2)
	mtcrf 0xff, r3
	lwz r3, Default_PPC.GPR[3](r2)
	lwz r2, Default_PPC.GPR[2](r2)
	rfi
dispatchException:
	lis r2, TRK_saved_exceptionID@h
	ori r2, r2, TRK_saved_exceptionID@l
	lhz r3, 0(r2)
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	lbz r2, TRKExceptionStatus.inTRK(r2)
	cmpwi r2, 0
	bne TRKExceptionHandler
	lis r2, gTRKCPUState@h
	ori r2, r2, gTRKCPUState@l
	stw r0, ProcessorState_PPC.Default.GPR[0](r2)
	stw r1, ProcessorState_PPC.Default.GPR[1](r2)
	mfsprg r0, 1
	stw r0, ProcessorState_PPC.Default.GPR[2](r2)
	sth r3, ProcessorState_PPC.Extended1.exceptionID(r2)
	sth r3, (ProcessorState_PPC.Extended1.exceptionID + 2)(r2)
	mfsprg r0, 2
	stw r0, ProcessorState_PPC.Default.GPR[3](r2)
	stmw r4, ProcessorState_PPC.Default.GPR[4](r2)
	mfsrr0 r27
	mflr r28
	mfsprg r29, 3
	mfctr r30
	mfxer r31
	stmw r27, ProcessorState_PPC.Default.PC(r2)
	bl TRKSaveExtended1Block
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	li r3, 1
	stb r3, TRKExceptionStatus.inTRK(r2)
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	lwz r0, TRKState.msr(r2)
	sync
	mtmsr r0
	sync
	lwz r0, TRKState.lr(r2)
	mtlr r0
	lwz r0, TRKState.ctr(r2)
	mtctr r0
	lwz r0, TRKState.xer(r2)
	mtxer r0
	lwz r0, TRKState.dsisr(r2)
	mtdsisr r0
	lwz r0, TRKState.dar(r2)
	mtdar r0
	lmw r3, TRKState.gpr[3](r2)
	lwz r0, TRKState.gpr[0](r2)
	lwz r1, TRKState.gpr[1](r2)
	lwz r2, TRKState.gpr[2](r2)
	b TRKPostInterruptEvent
}

static asm void TRKExceptionHandler(u16) {
	nofralloc
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	sth r3, TRKExceptionStatus.exceptionInfo.exceptionID(r2)
	mfsrr0 r3
	stw r3, TRKExceptionStatus.exceptionInfo.PC(r2)
	lhz r3, TRKExceptionStatus.exceptionInfo.exceptionID(r2)
	cmpwi r3, 0x200
	beq skipFaultingInstruction
	cmpwi r3, 0x300
	beq skipFaultingInstruction
	cmpwi r3, 0x400
	beq skipFaultingInstruction
	cmpwi r3, 0x600
	beq skipFaultingInstruction
	cmpwi r3, 0x700
	beq skipFaultingInstruction
	cmpwi r3, 0x800
	beq skipFaultingInstruction
	cmpwi r3, 0x1000
	beq skipFaultingInstruction
	cmpwi r3, 0x1100
	beq skipFaultingInstruction
	cmpwi r3, 0x1200
	beq skipFaultingInstruction
	cmpwi r3, 0x1300
	beq skipFaultingInstruction
	b recordException
skipFaultingInstruction:
	mfsrr0 r3
	addi r3, r3, 0x4
	mtsrr0 r3
recordException:
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	li r3, 0x1
	stb r3, TRKExceptionStatus.exceptionDetected(r2)
	mfsprg r3, 3
	mtcrf 0xff, r3
	mfsprg r2, 1
	mfsprg r3, 2
	rfi
}

void TRKPostInterruptEvent(void) {
    int eventType;
    int instruction;
    size_t registerSize;
    TRKEvent event;

    if (gTRKState.inputActivated) {
        gTRKState.inputActivated = FALSE;
    } else {
        switch ((int)(gTRKCPUState.Extended1.exceptionID & 0xFFFF)) {
        case 0x700:
        case 0xd00:
            registerSize = 4;
            TRKTargetReadInstruction(&instruction, gTRKCPUState.Default.PC);

            if (instruction == 0xfe00000) {
                eventType = NUBEVENT_Support;
            } else {
                eventType = NUBEVENT_Breakpoint;
            }
            break;
        default:
            eventType = NUBEVENT_Exception;
            break;
        }

        TRKConstructEvent(&event, (NubEventType)eventType);
        TRKPostEvent(&event);
    }
}

asm void TRKSwapAndGo() {
	nofralloc
	lis r3, gTRKState@h
	ori r3, r3, gTRKState@l
	stmw r0, TRKState.gpr[0](r3)
	mfmsr r0
	stw r0, TRKState.msr(r3)
	mflr r0
	stw r0, TRKState.lr(r3)
	mfctr r0
	stw r0, TRKState.ctr(r3)
	mfxer r0
	stw r0, TRKState.xer(r3)
	mfdsisr r0
	stw r0, TRKState.dsisr(r3)
	mfdar r0
	stw r0, TRKState.dar(r3)
	li r1, -0x7ffe
	nor r1, r1, r1
	mfmsr r3
	and r3, r3, r1
	mtmsr r3
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	lwz r2, TRKState.inputPendingPtr(r2)
	lbz r2, TRKState.gpr[0](r2)
	cmpwi r2, 0
	beq restoreTargetState
	lis r2, gTRKState@h
	ori r2, r2, gTRKState@l
	li r3, 1
	stb r3, TRKState.inputActivated(r2)
	b TRKInterruptHandlerEnableInterrupts
restoreTargetState:
	lis r2, gTRKExceptionStatus@h
	ori r2, r2, gTRKExceptionStatus@l
	li r3, 0
	stb r3, 0xc(r2)
	bl TRKRestoreExtended1Block
	lis r2, gTRKCPUState@h
	ori r2, r2, gTRKCPUState@l
	lmw r27, ProcessorState_PPC.Default.PC(r2)
	mtsrr0 r27
	mtlr r28
	mtcrf 0xff, r29
	mtctr r30
	mtxer r31
	lmw r3, ProcessorState_PPC.Default.GPR[3](r2)
	lwz r0, ProcessorState_PPC.Default.GPR[0](r2)
	lwz r1, ProcessorState_PPC.Default.GPR[1](r2)
	lwz r2, ProcessorState_PPC.Default.GPR[2](r2)
	rfi
}

asm void TRKInterruptHandlerEnableInterrupts(void) {
	nofralloc;
	lis       r2, gTRKState@h
	ori       r2, r2, gTRKState@l
	lwz       r0, TRKState.msr(r2)
	sync
	mtmsr     r0
	sync
	lwz       r0, TRKState.lr(r2)
	mtlr      r0
	lwz       r0, TRKState.ctr(r2)
	mtctr     r0
	lwz       r0, TRKState.xer(r2)
	mtxer     r0
	lwz       r0, TRKState.dsisr(r2)
	mtdsisr   r0
	lwz       r0, TRKState.dar(r2)
	mtdar     r0
	lmw       r3, TRKState.gpr[3](r2)
	lwz       r0, TRKState.gpr[0](r2)
	lwz       r1, TRKState.gpr[1](r2)
	lwz       r2, TRKState.gpr[2](r2)
	b         TRKPostInterruptEvent
}

DSError TRKTargetInterrupt(TRKEvent* event) {
    DSError error = DS_NoError;
    switch (event->eventType) {
    case NUBEVENT_Breakpoint:
    case NUBEVENT_Exception:
        if (TRKTargetCheckStep() == FALSE) {
            TRKTargetSetStopped(TRUE);
            error = TRKDoNotifyStopped(DSMSG_NotifyStopped);
        }
        break;
    default:
        break;
    }

    return error;
}

DSError TRKTargetAddStopInfo(register TRKBuffer* buffer) {
    DSError error;
    CommandReply reply;
    u32 instruction;
    u32 queueValue;
    int threadCount;
    int currentThread;
    u8 instructions[0x400];
    size_t size;
    int i;
    TRK_memset(&reply, 0, 0x40);
    reply.messageLength = 0x4E8;
    reply.commandID.b = 0x90;
    reply.replyError.r = gTRKCPUState.Default.PC;
    GetThreadInfo(&threadCount, &currentThread);
    *(u32*)(reply.payload + 4) = threadCount;
    *(u32*)(reply.payload + 8) = *(u32*)ConvertAddress(0xE4);
    TRKTargetReadInstruction(&instruction, gTRKCPUState.Default.PC);
    reply.parameter.instruction = instruction;
    *(u32*)reply.payload = gTRKCPUState.Extended1.exceptionID & 0xFFFF;
    error = TRKAppendBuffer_ui8(buffer, (u8*)&reply, 0x40);
    if (error == DS_NoError) {
        for (i = 0; i < 32; i++) TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.GPR[i]);
        TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.PC);
        TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.LR);
        TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.CR);
        TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.CTR);
        error = TRKAppendBuffer1_ui32(buffer, gTRKCPUState.Default.XER);
    }
    if (error == DS_NoError) {
        queueValue = *(u32*)ConvertAddress(0xD4);
        TRKAppendBuffer1_ui32(buffer, queueValue);
        queueValue = *(u32*)ConvertAddress(0xD8);
        TRKAppendBuffer1_ui32(buffer, queueValue);
        queueValue = *(u32*)ConvertAddress(0xDC);
        TRKAppendBuffer1_ui32(buffer, queueValue);
        queueValue = *(u32*)ConvertAddress(0xE0);
        TRKAppendBuffer1_ui32(buffer, queueValue);
        queueValue = *(u32*)ConvertAddress(0xE4);
        error = TRKAppendBuffer1_ui32(buffer, queueValue);
    }
    if (error == DS_NoError) {
        size = sizeof(instructions);
        error = TRKTargetAccessMemory(instructions, gTRKCPUState.Default.PC & ~0x3FF, &size, 0, TRUE);
        TRKAppendBuffer(buffer, instructions, sizeof(instructions));
    }
    return error;
}

void TRKTargetAddExceptionInfo(TRKBuffer* b) {
    size_t instructionSize;
    u32 instruction;
    CommandReply reply;

    TRK_memset(&reply, 0, 0x40);

    reply.messageLength = 0x40;
    reply.commandID.b = 0x91;
    reply.replyError.r = gTRKExceptionStatus.exceptionInfo.PC;

    TRKTargetReadInstruction(&instruction, gTRKExceptionStatus.exceptionInfo.PC);

    reply.parameter.instruction = instruction;
    *(u32*)reply.payload = gTRKExceptionStatus.exceptionInfo.exceptionID;

    TRKAppendBuffer_ui8(b, (u8*)&reply, 0x40);
}

static BOOL TRKTargetCheckStep() {
    if (gTRKStepStatus.active) {
        TRKTargetEnableTrace(FALSE);

        if (TRKTargetStepDone()) {
            gTRKStepStatus.active = FALSE;
        } else {
            TRKTargetDoStep();
        }
    }

    return gTRKStepStatus.active;
}

DSError TRKTargetSingleStep(u32 count, BOOL stepOver) {
    DSError error = DS_NoError;

    if (stepOver) {
        error = DS_UnsupportedError;
    } else {
        gTRKStepStatus.type = DSSTEP_IntoCount;
        gTRKStepStatus.count = count;
        error = TRKTargetDoStep();
    }

    return error;
}

DSError TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver) {
    DSError error = DS_NoError;

    if (stepOver) {
        error = DS_UnsupportedError;
    } else {
        gTRKStepStatus.type = DSSTEP_IntoRange;
        gTRKStepStatus.rangeStart = rangeStart;
        gTRKStepStatus.rangeEnd = rangeEnd;
        error = TRKTargetDoStep();
    }

    return error;
}

u32 TRKTargetGetPC() {
    return gTRKCPUState.Default.PC;
}

DSError TRKTargetSupportRequest() {
    DSIOResult ioResult;
    size_t* length;
    MessageCommandID commandId;
    DSError error;
    u32 filePosition;
    TRKEvent event;

    commandId = (MessageCommandID)gTRKCPUState.Default.GPR[3];
    if (commandId != DSMSG_ReadFile && commandId != DSMSG_WriteFile && commandId != DSMSG_OpenFile && commandId != DSMSG_CloseFile &&
        commandId != DSMSG_PositionFile) {
        TRKConstructEvent(&event, NUBEVENT_Exception);
        TRKPostEvent(&event);
        return DS_NoError;
    } else if (commandId == DSMSG_OpenFile) {
        error = HandleOpenFileSupportRequest((const char*)gTRKCPUState.Default.GPR[4], gTRKCPUState.Default.GPR[5] & 0xff, (u32*)gTRKCPUState.Default.GPR[6], &ioResult);

        if (ioResult == DS_IONoError && error != DS_NoError) {
            ioResult = DS_IOError;
        }

        gTRKCPUState.Default.GPR[3] = ioResult;
    } else if (commandId == DSMSG_CloseFile) {
        error = HandleCloseFileSupportRequest(gTRKCPUState.Default.GPR[4], &ioResult);

        if (ioResult == DS_IONoError && error != DS_NoError) {
            ioResult = DS_IOError;
        }

        gTRKCPUState.Default.GPR[3] = ioResult;
    } else if (commandId == DSMSG_PositionFile) {
        filePosition = *(u32*)gTRKCPUState.Default.GPR[5];
        error = HandlePositionFileSupportRequest(gTRKCPUState.Default.GPR[4], &filePosition, (u8)gTRKCPUState.Default.GPR[6], &ioResult);

        if (ioResult == DS_IONoError && error != DS_NoError) {
            ioResult = DS_IOError;
        }

        gTRKCPUState.Default.GPR[3] = ioResult;
        *(u32*)gTRKCPUState.Default.GPR[5] = filePosition;
    } else {
        length = (size_t*)gTRKCPUState.Default.GPR[5];
        error =
            TRKSuppAccessFile(gTRKCPUState.Default.GPR[4], (u8*)gTRKCPUState.Default.GPR[6], length, &ioResult, TRUE, commandId == DSMSG_ReadFile);

        if (ioResult == DS_IONoError && error != DS_NoError) {
            ioResult = DS_IOError;
        }

        gTRKCPUState.Default.GPR[3] = ioResult;

        if (commandId == DSMSG_ReadFile) {
            TRK_flush_cache(gTRKCPUState.Default.GPR[6], *length);
        }
    }

    gTRKCPUState.Default.PC += 4;
    return error;
}

BOOL TRKTargetStopped() {
    return gTRKState.isStopped;
}

void TRKTargetSetStopped(unsigned int stopped) {
    gTRKState.isStopped = stopped;
}

DSError TRKTargetStop() {
    gTRKState.isStopped = TRUE;
    return DS_NoError;
}

DSError TRKPPCAccessSPR(void* value, u32 spr, BOOL read) {

    u32 accessFunction[10] = {INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP};

    if (read) {
        accessFunction[0] = INSTR_MFSPR(4, spr);
        accessFunction[1] = (u32)INSTR_STW(4, 0, 3);
    } else {
        accessFunction[0] = (u32)INSTR_LWZ(4, 0, 3);
        accessFunction[1] = INSTR_MTSPR(spr, 4);
    }

    return TRKPPCAccessSpecialReg(value, accessFunction, read);
}

DSError TRKPPCAccessPairedSingleRegister(void* value, u32 psr, BOOL read) {
    u32 instructionData[] = {INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP};

    if (read) {
        instructionData[0] = INSTR_PSQ_ST(psr, 0, 3, 0, 0);
    } else {
        instructionData[0] = INSTR_PSQ_L(psr, 0, 3, 0, 0);
    }

    return TRKPPCAccessSpecialReg(value, instructionData, read);
}

asm void ReadFPSCR(register f64*) {
	nofralloc
	stwu	r1, -0x40(r1)
	stfd	f31, 0x10(r1)
	psq_st	f31, 0x20(r1),0,0
	mffs	f31
	stfd	f31, 0x0(r3)
	psq_l	f31, 0x20(r1),0,0
	lfd		f31, 0x10(r1)
	addi	r1, r1, 0x40
	blr
}

asm void WriteFPSCR(register f64*) {
	nofralloc
	stwu	r1, -0x40(r1)
	stfd	f31, 0x10(r1)
	psq_st	f31, 0x20(r1), 0, 0
	lfd		f31, 0(r3)
	mtfsf	0xff, f31
	psq_l	f31, 0x20(r1), 0, 0
	lfd		f31, 0x10(r1)
	addi	r1,	r1, 0x40
	blr
}

DSError TRKPPCAccessFPRegister(void* value, u32 fpr, BOOL read) {
    DSError error = DS_NoError;
    u32 instructions[] = {INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP, INSTR_NOP};

    if (fpr < 0x20) {
        if (read) {
            instructions[0] = INSTR_STFD(fpr, 0, 3);
        } else {
            instructions[0] = INSTR_LFD(fpr, 0, 3);
        }

        error = TRKPPCAccessSpecialReg(value, instructions, read);
    } else if (fpr == 0x20) {
        if (read) {
            ReadFPSCR(value);
        } else {
            WriteFPSCR(value);
        }

        *(u64*)value &= 0xFFFFFFFF;
    } else if (fpr == 0x21) {
        if (!read) {
            *(u32*)value = *((u32*)(value) + 1);
        }

        error = TRKPPCAccessSPR(value, SPR_FPECR, read);
        if (read) {
            DSFetch_u64(value) = DSFetch_u32(value) & 0xffffffffLL;
        }
    }

    return error;
}

DSError TRKPPCAccessSpecialReg(void* value, u32* accessFunction, BOOL read) {
    typedef void (*asm_access_type)(void*, void*);

    asm_access_type asm_access;

    accessFunction[9] = INSTR_BLR;

    asm_access = (asm_access_type)accessFunction;

    TRK_flush_cache((u32)accessFunction, (sizeof(accessFunction) * 10));
    (*asm_access)((u32*)value, (void*)&TRKvalue128_temp);

    return DS_NoError;
}

void TRKTargetSetInputPendingPtr(void* ptr) {
    gTRKState.inputPendingPtr = ptr;
}

u32 ConvertAddress(u32 address) {
    return address | 0x80000000;
}

static void GetThreadInfo(int* count, int* currentIndex) {
    int i;
    OSThread* thread;
    *count = 1;
    *currentIndex = 0;
    thread = *(OSThread**)0x800000DC;
    if (thread == (OSThread*)-1 || thread == NULL || thread == (OSThread*)0x80000000) return;
    for (i = 0; thread != NULL;) {
        if (thread == *(OSThread**)0x800000E4) *currentIndex = i;
        thread = (OSThread*)ConvertAddress((u32)thread->linkActive.next);
        i++;
        if (thread == (OSThread*)-1 || thread == NULL || thread == (OSThread*)0x80000000) break;
    }
    *count = i;
}
