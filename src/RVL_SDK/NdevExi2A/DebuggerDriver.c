#include "NdevExi2A/DebuggerDriver.h"
#include "private/flipper.h"

static u32 __DBRecvDataSize;
static u32 __DBRecvMail;
static u8 __DBEXIInputFlag;
static __OSInterruptHandler __DBDbgCallback;
static __OSInterruptHandler __DBMtrCallback;

static u8 __DBReadUSB_CSR(void);
static void __DBWaitForSendMail(void);

void __DBMtrHandler(__OSInterrupt type, OSContext* ctx) {
    __DBEXIInputFlag = TRUE;

    if (__DBMtrCallback != NULL) {
        __DBMtrCallback(0, ctx);
    }
}

void __DBIntrHandler(__OSInterrupt type, OSContext* ctx) {
    __PIRegs[0] = 0x1000;

    if (__DBDbgCallback != NULL) {
        __DBDbgCallback(type, ctx);
    }
}

void DBInitComm(u8** flagOut, __OSInterruptHandler handler) {
    BOOL enabled = OSDisableInterrupts();

    *flagOut = &__DBEXIInputFlag;
    __DBMtrCallback = handler;
    __DBEXIInit();

    OSRestoreInterrupts(enabled);
}

static u8 __DBReadUSB_CSR(void) {
    u8 csr;
    __DBEXIReadReg(DB_USB_CSR, &csr, sizeof(csr));
    return csr;
}

static void __DBCheckMailBox(void) {
    u8 csr = __DBReadUSB_CSR();

    if (!(csr & DB_CSR_RECV_EMPTY)) {
        u32 mail;
        __DBReadMailbox(&mail);

        if (ODEMUIsValidMail(mail)) {
            __DBRecvMail = mail;
            __DBRecvDataSize = ODEMUGetSize(mail);
            __DBEXIInputFlag = TRUE;
        }
    }
}

void DBInitInterrupts(void) {
    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_2);
    __OSMaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
    __DBDbgCallback = __DBMtrHandler;
    __OSSetInterruptHandler(__OS_INTERRUPT_PI_DEBUG, __DBIntrHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
}

u32 DBQueryData(void) {
    __DBEXIInputFlag = FALSE;

    if (__DBRecvDataSize == 0) {
        BOOL enabled = OSDisableInterrupts();
        __DBCheckMailBox();
        OSRestoreInterrupts(enabled);
    }

    return __DBRecvDataSize;
}

BOOL DBRead(void* dst, u32 size) {
    BOOL enabled = OSDisableInterrupts();

    __DBRead(ODEMUGetPc2NngcOffset(__DBRecvMail) + DB_RECV_OFFSET, dst, ROUND_UP(size, 4));
    __DBRecvDataSize = 0;
    __DBEXIInputFlag = FALSE;

    OSRestoreInterrupts(enabled);

    return FALSE;
}

BOOL DBWrite(const void* src, u32 size) {
    static u8 l_byOffsetCounter = 0x80;

    BOOL enabled = OSDisableInterrupts();
    u32 ofs;
    u32 mail;

    __DBWaitForSendMail();

    ofs = 0;
    if (!(++l_byOffsetCounter & 0x1)) {
        ofs = 0;
    } else {
        ofs = DB_PAGE_SIZE;
    }

    while (__DBWrite(ofs, src, ROUND_UP(size, 4)) == FALSE) {
    }

    __DBWaitForSendMail();

    mail = ODEMUGenMailData(l_byOffsetCounter, size);
    while (__DBWriteMailbox(mail) == FALSE) {
    }

    __DBWaitForSendMail();

    OSRestoreInterrupts(enabled);

    return FALSE;
}

static void __DBWaitForSendMail(void) {
    while (__DBReadUSB_CSR() & DB_CSR_SEND_BUSY) {
    }
}

void DBOpen(void) {}

void DBClose(void) {}
