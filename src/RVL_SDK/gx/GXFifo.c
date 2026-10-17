#include <RVL_SDK/gx/GXFifo.h>

typedef struct OSThread OSThread;
typedef struct OSContext OSContext;
typedef void (*OSInterruptHandler)(s16 interrupt, OSContext* context);

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL enabled);
extern void OSReport(const char* format, ...);
extern OSInterruptHandler __OSSetInterruptHandler(s16 interrupt, OSInterruptHandler handler);
extern u32 __OSUnmaskInterrupts(u32 mask);
extern OSThread* OSGetCurrentThread(void);
extern void* memset(void* destination, int value, u32 size);
extern void GXCPInterruptHandler(s16 interrupt, OSContext* context);

#define GX_FALSE 0
#define GX_TRUE 1
#define OSRoundDown32B(value) ((u32)(value) & ~(32 - 1))
#define __OS_INTERRUPT_PI_CP 17
#define OS_INTERRUPTMASK_PI_CP 0x4000

extern __GXFifoObj CPUFifo;
extern __GXFifoObj GPFifo;
extern GXBool CPUFifoReady;
extern GXBool GPFifoReady;
extern OSThread* __GXCurrentThread;
extern volatile BOOL GXOverflowSuspendInProgress;
extern GXBreakPtCallback BreakPointCB;

static void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);

static void GXInitFifoLimits(GXFifoObj *fifo, u32 hi, u32 lo) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;
    realFifo->hiWatermark = hi;
    realFifo->loWatermark = lo;
}

void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;
    realFifo->base = base;
    realFifo->top = (void*)((u32)base + size - 4);
    realFifo->size = size;
    realFifo->count = 0;
    GXInitFifoLimits(fifo, size - (16 * 1024), OSRoundDown32B(size/2));
    GXInitFifoPtrs(fifo, base, base);
}

static void GXInitFifoPtrs(GXFifoObj* fifo, void *readPtr, void* writePtr) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;
    BOOL en = OSDisableInterrupts();
    realFifo->rdPtr = readPtr;
    realFifo->wrPtr = writePtr;
    realFifo->count = (u8*)writePtr - (u8*)readPtr;
    if (realFifo->count < 0) {
        realFifo->count += realFifo->size;
    }

    OSRestoreInterrupts(en);
}

GXBool CPGPLinkCheck(void) {
    u32 check = 0;
    s32 range1, range2, overlap;

    if (!CPUFifoReady || !GPFifoReady) {
        return GX_FALSE;
    }

    if (CPUFifo.base == GPFifo.base) {
        ++check;
    }

    if (CPUFifo.top == GPFifo.top) {
        ++check;
    }

    if (check == 2) {
        if (CPUFifo.hiWatermark == GPFifo.hiWatermark) {
            ++check;
        }

        if (CPUFifo.loWatermark == GPFifo.loWatermark) {
            ++check;
        }

        return GX_TRUE;
    }

    range1 = (s32)((u8*)CPUFifo.top - (u8*)GPFifo.base);
    range2 = (s32)((u8*)GPFifo.top - (u8*)CPUFifo.base);
    overlap = ((range1 > 0) && (range2 > 0) || (range1 < 0 && (range2 < 0)));

    if (overlap) {
        OSReport("CPUFifo: %08X - %08X\n", (u32)CPUFifo.base, (u32)CPUFifo.top);
        OSReport("GP Fifo: %08X - %08X\n", (u32)GPFifo.base, (u32)GPFifo.top);
    }

    return GX_FALSE;
}

GXBool __GXIsGPFifoReady(void) {
    return GPFifoReady;
}

GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb) {
    GXBreakPtCallback oldcb = BreakPointCB;
    BOOL enabled = OSDisableInterrupts();
    BreakPointCB = cb;
    OSRestoreInterrupts(enabled);
    return oldcb;
}

void __GXFifoInit() {
    __OSSetInterruptHandler(__OS_INTERRUPT_PI_CP, GXCPInterruptHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_CP);
    __GXCurrentThread = OSGetCurrentThread();
    GXOverflowSuspendInProgress = FALSE;
    memset(&CPUFifo, 0, sizeof(__GXFifoObj));
    memset(&GPFifo, 0, sizeof(__GXFifoObj));
    CPUFifoReady = GX_FALSE;
    GPFifoReady  = GX_FALSE;
}
