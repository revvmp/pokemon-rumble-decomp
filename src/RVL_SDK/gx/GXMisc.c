#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/gx/GXState.h"
#include "revolution/gx/shortcut_pe_misc.h"
#include "revolution/base/PPCArch.h"
#include "revolution/os.h"
#include "private/flipper.h"


static GXDrawSyncCallback TokenCB;
static GXDrawDoneCallback DrawDoneCB;
static volatile GXBool DrawDone;
static OSThreadQueue FinishQueue;

void GXSetMisc(GXMiscToken token, u32 value) {
    switch (token) {
    case GX_MT_NULL:
        break;
    case GX_MT_XF_FLUSH:
        gx->vNum = value;
        gx->vNumNot = !gx->vNum;
        gx->bpSentNot = GX_TRUE;
        if (gx->vNum > 0) {
            gx->dirtyState |= GX_DIRTY_VCD;
        }
        break;
    case GX_MT_DL_SAVE_CONTEXT:
        gx->dlSaveContext = (value != 0);
        break;
    case GX_MT_ABORT_WAIT_COPYOUT:
        gx->abtWaitPECopy = (value != 0);
        break;
    }
}

void GXFlush(void) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }

    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    PPCSync();
}

void GXResetWriteGatherPipe(void) {
    while (PPCMfwpar() & 1) {
    }

    PPCMtwpar(OSUncachedToPhysical(0xC008000));
}

static void __GXAbortWait(u32 clocks) {
    OSTime time0;
    OSTime time1;

    time0 = OSGetTime();
    do {
        time1 = OSGetTime();
    } while (time1 - time0 <= clocks / 4);
}

static void __GXAbortWaitPECopyDone(void) {
    u32 peCount0;
    u32 peCount1;

    peCount0 = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);
    do {
        peCount1 = peCount0;
        __GXAbortWait(32);
        peCount0 = GX_MEM_COUNTER_READ_U32(MEM_PE_REQCOUNT);
    } while (peCount0 != peCount1);
}

void __GXAbort(void) {
    if (gx->abtWaitPECopy && __GXIsGPFifoReady()) {
        __GXAbortWaitPECopyDone();
    }

    __PIRegs[PI_REG_CPABT / 4] = 1;
    __GXAbortWait(200);
    __PIRegs[PI_REG_CPABT / 4] = 0;
    __GXAbortWait(20);
}

void GXAbortFrame(void) {
    __GXAbort();

    if (__GXIsGPFifoReady()) {
        __GXCleanGPFifo();
        __GXInitRevisionBits();
        gx->dirtyState = 0;
        GXFlush();
    }
}

void GXSetDrawSync(u16 token) {
    BOOL enabled;
    u32 reg;

    enabled = OSDisableInterrupts();
    reg = PE_TOKEN_INT(token, 0x48);
    GX_WRITE_RA_REG(reg);
    SC_PE_TOKEN_SET_TOKEN(reg, token);
    SC_PE_TOKEN_SET_RID(reg, 0x47);
    GX_WRITE_RA_REG(reg);
    GXFlush();
    OSRestoreInterrupts(enabled);
    BP_SENT()
}

void GXSetDrawDone(void) {
    u32 reg;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    reg = PE_FINISH(2, 0x45);
    GX_WRITE_RA_REG(reg);
    GXFlush();
    DrawDone = GX_FALSE;
    OSRestoreInterrupts(enabled);
    BP_SENT()
}

void GXWaitDrawDone(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    while (!DrawDone) {
        OSSleepThread(&FinishQueue);
    }
    OSRestoreInterrupts(enabled);
}

void GXDrawDone(void) {
    GXSetDrawDone();
    GXWaitDrawDone();
}

void GXPixModeSync(void) {
    GX_WRITE_RA_REG(gx->peCtrl);
    BP_SENT()
}

void GXTexModeSync(void) {
    u32 reg;

    reg = TX_LOADBLOCK3(0, 0, 0x63);
    GX_WRITE_RA_REG(reg);
    BP_SENT()
}

void GXPokeAlphaMode(GXCompare func, u8 threshold) {
    u32 reg;

    reg = (u32)(func << 8 | threshold);
    GX_PE_REG_WRITE_U16(PE_REG_ALPHA_MODE, reg);
}

void GXPokeAlphaRead(GXAlphaReadMode mode) {
    u32 reg;

    reg = 0;
    SC_PE_PI_CTL_SET_AFMT(reg, mode);
    SC_PE_PI_CTL_SET_ZFMT(reg, 1);
    GX_PE_REG_WRITE_U16(PE_REG_PI_CTL, reg);
}

void GXPokeAlphaUpdate(GXBool updateEnable) {
    u32 reg;

    reg = GX_PE_REG_READ_U16(PE_REG_CMODE0);
    SC_PE_CMODE0_SET_ALPHA_MASK(reg, updateEnable);
    GX_PE_REG_WRITE_U16(PE_REG_CMODE0, reg);
}

void GXPokeBlendMode(GXBlendMode type, GXBlendFactor srcFactor, GXBlendFactor dstFactor, GXLogicOp op) {
    u32 reg;

    reg = GX_PE_REG_READ_U16(PE_REG_CMODE0);
    SC_PE_CMODE0_SET_BLEND_ENABLE(reg, (type == GX_BM_BLEND) || (type == GX_BM_SUBTRACT));
    SC_PE_CMODE0_SET_BLENDOP(reg, type == GX_BM_SUBTRACT);
    SC_PE_CMODE0_SET_LOGICOP_ENABLE(reg, type == GX_BM_LOGIC);
    SC_PE_CMODE0_SET_LOGICOP(reg, op);
    SC_PE_CMODE0_SET_SFACTOR(reg, srcFactor);
    SC_PE_CMODE0_SET_DFACTOR(reg, dstFactor);
    SC_PE_CMODE0_SET_RID(reg, 0x41);
    GX_PE_REG_WRITE_U16(PE_REG_CMODE0, reg);
}

void GXPokeColorUpdate(GXBool updateEnable) {
    u32 reg;

    reg = GX_PE_REG_READ_U16(PE_REG_CMODE0);
    SC_PE_CMODE0_SET_COLOR_MASK(reg, updateEnable);
    GX_PE_REG_WRITE_U16(PE_REG_CMODE0, reg);
}

void GXPokeDstAlpha(GXBool enable, u8 alpha) {
    u32 reg;

    reg = 0;
    SC_PE_CMODE1_SET_CONSTANT_ALPHA(reg, alpha);
    SC_PE_CMODE1_SET_CONSTANT_ALPHA_ENABLE(reg, enable);
    GX_PE_REG_WRITE_U16(PE_REG_CMODE1, reg);
}

void GXPokeDither(GXBool dither) {
    u32 reg;

    reg = GX_PE_REG_READ_U16(PE_REG_CMODE0);
    SC_PE_CMODE0_SET_DITHER_ENABLE(reg, dither);
    GX_PE_REG_WRITE_U16(PE_REG_CMODE0, reg);
}

void GXPokeZMode(GXBool compareEnable, GXCompare func, GXBool updateEnable) {
    u32 reg;

    reg = 0;
    SC_PE_ZMODE_SET_ENABLE(reg, compareEnable);
    SC_PE_ZMODE_SET_FUNC(reg, func);
    SC_PE_ZMODE_SET_MASK(reg, updateEnable);
    GX_PE_REG_WRITE_U16(PE_REG_ZMODE, reg);
}

void GXPeekARGB(u16 x, u16 y, u32* color) {
    u32 addr;

    addr = (u32)OSPhysicalToUncached(0x8000000);
    SC_PE_PI_EFB_ADDR_SET_X(addr, x);
    SC_PE_PI_EFB_ADDR_SET_Y(addr, y);
    SC_PE_PI_EFB_ADDR_SET_TYPE(addr, 0);
    *color = *(u32*)addr;
}

void GXPeekZ(u16 x, u16 y, u32* z) {
    u32 addr;

    addr = (u32)OSPhysicalToUncached(0x8000000);
    SC_PE_PI_EFB_ADDR_SET_X(addr, x);
    SC_PE_PI_EFB_ADDR_SET_Y(addr, y);
    SC_PE_PI_EFB_ADDR_SET_TYPE(addr, 1);
    *z = *(u32*)addr;
}

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback callback) {
    GXDrawSyncCallback oldCallback;
    BOOL enabled;

    oldCallback = TokenCB;
    enabled = OSDisableInterrupts();
    TokenCB = callback;
    OSRestoreInterrupts(enabled);
    return oldCallback;
}

static void GXTokenInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u16 token;
    OSContext exceptionContext;
    u32 reg;

    token = (u16)GX_PE_REG_READ_U16(PE_REG_TOKEN);
    if (TokenCB) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        TokenCB(token);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }

    reg = GX_PE_REG_READ_U16(PE_REG_INTRCTL);
    SC_PE_INTRCTL_SET_INT0CLR(reg, 1);
    GX_PE_REG_WRITE_U16(PE_REG_INTRCTL, reg);
}

GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback callback) {
    GXDrawDoneCallback oldCallback;
    BOOL enabled;

    oldCallback = DrawDoneCB;
    enabled = OSDisableInterrupts();
    DrawDoneCB = callback;
    OSRestoreInterrupts(enabled);
    return oldCallback;
}

static void GXFinishInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;
    u32 reg;

    reg = GX_PE_REG_READ_U16(PE_REG_INTRCTL);
    SC_PE_INTRCTL_SET_INT1CLR(reg, 1);
    GX_PE_REG_WRITE_U16(PE_REG_INTRCTL, reg);

    DrawDone = GX_TRUE;

    if (DrawDoneCB) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        DrawDoneCB();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }

    OSWakeupThread(&FinishQueue);
}

void __GXPEInit(void) {
    u32 reg;

    __OSSetInterruptHandler(__OS_INTERRUPT_PI_PE_TOKEN, GXTokenInterruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_PI_PE_FINISH, GXFinishInterruptHandler);

    OSInitThreadQueue(&FinishQueue);

    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_PE_TOKEN);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_PE_FINISH);

    reg = GX_PE_REG_READ_U16(PE_REG_INTRCTL);
    SC_PE_INTRCTL_SET_INT0CLR(reg, 1);
    SC_PE_INTRCTL_SET_INT1CLR(reg, 1);
    SC_PE_INTRCTL_SET_INT0EN(reg, 1);
    SC_PE_INTRCTL_SET_INT1EN(reg, 1);
    GX_PE_REG_WRITE_U16(PE_REG_INTRCTL, reg);
}
