#ifndef PPC_GENERIC_TARGIMPL_H
#define PPC_GENERIC_TARGIMPL_H

#include <stddef.h>

#include <MetroTRK/Portable/nubevent.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PPC_SystemReset = 0x100,
    PPC_MachineCheck = 0x200,
    PPC_DataStorage = 0x300,
    PPC_InstructionStorage = 0x400,
    PPC_ExternalInterrupt = 0x500,
    PPC_Alignment = 0x600,
    PPC_Program = 0x700,
    PPC_FloatingPointUnavaiable = 0x800,
    PPC_Decrementer = 0x900,
    PPC_SystemCall = 0xC00,
    PPC_Trace = 0xD00,
    PPC_PerformanceMonitor = 0xF00,
    PPC_InstructionAddressBreakpoint = 0x1300,
    PPC_SystemManagementInterrupt = 0x1400,
    PPC_ThermalManagementInterrupt = 0x1700,
} PPCExceptionID;

typedef enum {
    VALIDMEM_Readable,
    VALIDMEM_Writeable,
} ValidMemoryOptions;

typedef struct Default_PPC {
    u32 GPR[32];
    u32 PC;
    u32 LR;
    u32 CR;
    u32 CTR;
    u32 XER;
} Default_PPC;

typedef struct Float_PPC {
    u64 FPR[32];
    u64 FPSCR;
    u64 FPECR;
} Float_PPC;

typedef struct Extended1_PPC_6xx_7xx {
    u32 SR[16];
    u32 TBL;
    u32 TBU;
    u32 HID0;
    u32 HID1;
    u32 MSR;
    u32 PVR;
    u32 IBAT0U;
    u32 IBAT0L;
    u32 IBAT1U;
    u32 IBAT1L;
    u32 IBAT2U;
    u32 IBAT2L;
    u32 IBAT3U;
    u32 IBAT3L;
    u32 DBAT0U;
    u32 DBAT0L;
    u32 DBAT1U;
    u32 DBAT1L;
    u32 DBAT2U;
    u32 DBAT2L;
    u32 DBAT3U;
    u32 DBAT3L;
    u32 DMISS;
    u32 DCMP;
    u32 HASH1;
    u32 HASH2;
    u32 IMISS;
    u32 ICMP;
    u32 RPA;
    u32 SDR1;
    u32 DAR;
    u32 DSISR;
    u32 SPRG0;
    u32 SPRG1;
    u32 SPRG2;
    u32 SPRG3;
    u32 DEC;
    u32 IABR;
    u32 EAR;
    u32 DABR;
    u32 PMC1;
    u32 PMC2;
    u32 PMC3;
    u32 PMC4;
    u32 SIA;
    u32 MMCR0;
    u32 MMCR1;
    u32 THRM1;
    u32 THRM2;
    u32 THRM3;
    u32 ICTC;
    u32 L2CR;
    u32 UMMCR2;
    u32 UBAMR;
    u32 UMMCR0;
    u32 UPMC1;
    u32 UPMC2;
    u32 USIA;
    u32 UMMCR1;
    u32 UPMC3;
    u32 UPMC4;
    u32 USDA;
    u32 MMCR2;
    u32 BAMR;
    u32 SDA;
    u32 MSSCR0;
    u32 MSSCR1;
    u32 PIR;
    u32 exceptionID;
    u32 GQR[8];
    u32 HID_G;
    u32 WPAR;
    u32 DMA_U;
    u32 DMA_L;
} Extended1_PPC_6xx_7xx;

typedef struct Extended2_PPC_6xx_7xx {
    u32 PSR[32][2];
} Extended2_PPC_6xx_7xx;

typedef struct ProcessorState_PPC_6xx_7xx {
    Default_PPC Default;
    Float_PPC Float;
    Extended1_PPC_6xx_7xx Extended1;
    Extended2_PPC_6xx_7xx Extended2;
    u32 transport_handler_saved_ra;
} ProcessorState_PPC_6xx_7xx;

typedef ProcessorState_PPC_6xx_7xx ProcessorState_PPC;
extern ProcessorState_PPC gTRKCPUState;

typedef struct TRKState {
    u32 gpr[32];
    u32 lr;
    u32 ctr;
    u32 xer;
    u32 msr;
    u32 dar;
    u32 dsisr;
    BOOL isStopped;
    BOOL inputActivated;
    void* inputPendingPtr;
} TRKState;
extern Default_PPC gTRKSaveState;
extern u8 TRKvalue128_temp[16];
extern TRKState gTRKState;

void TRKSaveExtended1Block(void);
u32 __TRK_get_MSR(void);

DSError TRKValidMemory32(const void* address, size_t length, ValidMemoryOptions accessMode);
void TRK_ppc_memcpy(void* destination, const void* source, int count, u32 destinationMSR, u32 sourceMSR);
void __TRK_set_MSR(u32 msr);
void TRKInterruptHandler(void);
void TRKPostInterruptEvent(void);
void TRKSwapAndGo(void);
void TRKInterruptHandlerEnableInterrupts(void);
void TRKTargetSetStopped(unsigned int stopped);
DSError TRKPPCAccessSPR(void* value, u32 spr, BOOL read);
DSError TRKPPCAccessPairedSingleRegister(void* value, u32 registerNumber, BOOL read);
DSError TRKPPCAccessFPRegister(void* value, u32 registerNumber, BOOL read);
void ReadFPSCR(f64* value);
void WriteFPSCR(f64* value);
DSError TRKPPCAccessSpecialReg(void* value, u32* accessFunction, BOOL read);
void TRKTargetSetInputPendingPtr(void* inputPending);
u32* ConvertAddress(u32 address);
void GetThreadInfo(u32* threadCount, u32* currentThreadIndex);

DSError TRKTargetAccessMemory(void* buffer, u32 address, size_t* length, MemoryAccessOptions accessMode, BOOL read);
DSError TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
DSError TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer* buffer, size_t* length, BOOL read);
u32 TRKTargetGetPC(void);
DSError TRKTargetSingleStep(u32 count, BOOL stepOver);
DSError TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver);
DSError TRKTargetStop(void);
DSError TRKTargetAddStopInfo(TRKBuffer* buffer);
void TRKTargetAddExceptionInfo(TRKBuffer* buffer);

DSError TRKTargetInterrupt(TRKEvent* event);
DSError TRKTargetSupportRequest(void);
BOOL TRKTargetStopped(void);

#ifdef __cplusplus
}
#endif

#endif
