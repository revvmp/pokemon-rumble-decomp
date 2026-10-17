#include <revolution/os/OSThread.h>
#include <revolution/os/OSContext.h>
#include <revolution/os/OSInterrupt.h>
#include <revolution/os/OSTime.h>
#include "private/flipper.h"
#include "revolution/ai.h"
#include "revolution/os.h"

static AIDCallback __AID_Callback;
static u8* __CallbackStack;
static u8* __OldStack;
static volatile BOOL __AI_init_flag = 0;
static volatile BOOL __AID_Active = 0;
static void __AIDHandler(__OSInterrupt, OSContext*);
static void __AICallbackStackSwitch(register void*);
static void __AI_SRC_INIT(void);

const char* __AIVersion = "<< RVL_SDK - AI \trelease build: Feb 27 2009 10:01:30 (0x4302_145) >>";

AIDCallback AIRegisterDMACallback(AIDCallback cb) {
    AIDCallback old;
    BOOL intr;

    old = __AID_Callback;
    intr = OSDisableInterrupts();
    __AID_Callback = cb;
    OSRestoreInterrupts(intr);
    return old;
}

void AIInitDMA(u32 start, u32 len) {
    BOOL intr;

    intr = OSDisableInterrupts();
    __DSPRegs[0x18] = (__DSPRegs[0x18] & ~0x1FFF) | (start >> 16);
    __DSPRegs[0x19] = (__DSPRegs[0x19] & ~0xFFE0) | (0xFFFF & start);
    __DSPRegs[0x1B] = (__DSPRegs[0x1B] & ~0x7FFF) | ((len >> 5) & 0xFFFF);
    OSRestoreInterrupts(intr);
}

void AIStartDMA(void) {
    __DSPRegs[0x1B] |= 0x8000;
}

u32 AIGetDMABytesLeft(void) {
    return (__DSPRegs[0x1D] & 0x7FFF) << 5;
}

u32 AIGetDMAStartAddr(void) {
    return (((__DSPRegs[0x18] & 0x1FFF) << 16) | (__DSPRegs[0x19] & 0xFFE0));
}

u32 AIGetDMALength(void) {
    return ((__DSPRegs[0x1B] & 0x7FFF) << 5);
}

static u32 AIGetDSPSampleRate(void) {
    return ((__AIRegs[0] & 0x40) >> 6) ^ 1;
}

static void AISetDSPSampleRate(u32 sample_rate) {
    BOOL intr;

    if (sample_rate != AIGetDSPSampleRate()) {
        __AIRegs[0] &= ~0x40;

        if (sample_rate == 0) {
            intr = OSDisableInterrupts();
            __AI_SRC_INIT();
            __AIRegs[0] |= 0x40;
            OSRestoreInterrupts(intr);
        }
    }
}

static OSTime bound_32KHz;
static OSTime bound_48KHz;
static OSTime min_wait;
static OSTime max_wait;
static OSTime buffer;

BOOL AICheckInit(void) {
    return __AI_init_flag;
}

void AIInit(u8* pStack) {
    u32 reg;
    u32 timerClock;

    if (__AI_init_flag == 1) {
        return;
    }

    OSRegisterVersion(__AIVersion);

    timerClock = OS_TIMER_CLOCK;
    bound_32KHz = ((31524 * (timerClock / 125000)) / 8000);
    bound_48KHz = ((42024 * (timerClock / 125000)) / 8000);
    min_wait = ((42000 * (timerClock / 125000)) / 8000);
    max_wait = ((63000 * (timerClock / 125000)) / 8000);
    buffer = ((3000 * (timerClock / 125000)) / 8000);

    reg = __AIRegs[0];
    reg &= ~(0x1 | 0x4 | 0x10);
    __AIRegs[0] = reg;
    __AIRegs[1] = 0;
    __AIRegs[3] = 0;
    __AIRegs[0] = (__AIRegs[0] & ~0x20) | (0x1 << 5);

    AISetDSPSampleRate(0);

    __AID_Callback = NULL;
    __CallbackStack = pStack;
    __OSSetInterruptHandler(5, __AIDHandler);
    __OSUnmaskInterrupts(0x4000000);
    __AI_init_flag = 1;
}

static void __AIDHandler(__OSInterrupt intr, OSContext* pContext) {
    OSContext context;
    u16 reg;

    reg = __DSPRegs[5];
    reg = (reg & ~(0x80 | 0x20) | 0x8);
    __DSPRegs[5] = reg;

    OSClearContext(&context);
    OSSetCurrentContext(&context);

    if (__AID_Callback) {
        if (!__AID_Active) {
            __AID_Active = 1;

            if (__CallbackStack != NULL) {
                __AICallbackStackSwitch(__AID_Callback);
            } else {
                (*__AID_Callback)();
            }

            __AID_Active = 0;
        }
    }

    OSClearContext(&context);
    OSSetCurrentContext(pContext);
}

asm static void __AICallbackStackSwitch(register void *cb) {
    fralloc
    lis     r5, __OldStack@ha
    addi    r5, r5, __OldStack@l
    stw     r1, 0(r5)
    lis     r5, __CallbackStack@ha
    addi    r5, r5, __CallbackStack@l
    lwz     r1, 0(r5)
    subi    r1, r1, 8
    mtlr    cb
    blrl
    lis     r5, __OldStack@ha
    addi    r5, r5, __OldStack@l
    lwz     r1, 0(r5)
    frfree
    blr
}

void __AI_SRC_INIT(void) {
    OSTime startTime = 0;
    OSTime endTime = 0;
    OSTime elapsedTicks = 0;
    OSTime waitTicks;
    u32 initialSampleCount;
    u32 nextSampleCount;
    u32 calibrated = 0;


    waitTicks = 0;

    while (calibrated == 0) {
        SET_FLAG(__AIRegs[0], 1, 0x20, 5);
        SET_FLAG(__AIRegs[0], 0, 2, 1);
        SET_FLAG(__AIRegs[0], 1, 1, 0);

        initialSampleCount = GET_FLAG(__AIRegs[2], 0x7FFFFFFF, 0);

        while (initialSampleCount == GET_FLAG(__AIRegs[2], 0x7FFFFFFF, 0)) {
        }

        startTime = OSGetTime();
        SET_FLAG(__AIRegs[0], 1, 2, 1);
        SET_FLAG(__AIRegs[0], 1, 1, 0);

        nextSampleCount = GET_FLAG(__AIRegs[2], 0x7FFFFFFF, 0);

        while (nextSampleCount == GET_FLAG(__AIRegs[2], 0x7FFFFFFF, 0)) {
        }

        endTime = OSGetTime();

        elapsedTicks = endTime - startTime;
        SET_FLAG(__AIRegs[0], 0, 2, 1);
        SET_FLAG(__AIRegs[0], 0, 1, 0);

        if (elapsedTicks < bound_32KHz - buffer) {
            waitTicks = min_wait;
            calibrated = 1;
        } else {
            if (elapsedTicks >= (bound_32KHz + buffer) && elapsedTicks < (bound_48KHz - buffer)) {
                waitTicks = max_wait;
                calibrated = 1;
            } else {
                calibrated = 0;
            }
        }
    }

    while ((endTime + waitTicks) > OSGetTime()) {
    }
}
