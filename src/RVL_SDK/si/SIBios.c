#include "revolution/si.h"
#include "revolution/vi.h"
#include "private/flipper.h"

#define ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))

typedef struct SIControl {
    s32 chan;
    u32 poll;
    u32 inputBytes;
    void* input;
    SICallback callback;
} SIControl;

typedef union SIComcsr {
    u32 val;
    struct {
        u32 tcint : 1;
        u32 tcintmsk : 1;
        u32 comerr : 1;
        u32 rdstint : 1;
        u32 rdstintmsk : 1;
        u32 pad2 : 4;
        u32 outlngth : 7;
        u32 pad1 : 1;
        u32 inlngth : 7;
        u32 pad0 : 5;
        u32 channel : 2;
        u32 tstart : 1;
    } f;
} SIComcsr;

const char* __SIVersion = "<< RVL_SDK - SI \trelease build: Feb 27 2009 10:04:44 (0x4302_145) >>";

static SIControl Si = {-1, 0, 0, NULL, NULL};

static SIPacket Packet[SI_MAX_CHAN];
static OSAlarm Alarm[SI_MAX_CHAN];

static u32 Type[SI_MAX_CHAN] = {SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE};

static OSTime TypeTime[SI_MAX_CHAN];
static OSTime XferTime[SI_MAX_CHAN];
static SITypeAndStatusCallback TypeCallback[SI_MAX_CHAN][4];
static __OSInterruptHandler RDSTHandler[4];
u32 __PADFixBits;

static BOOL InputBufferValid[SI_MAX_CHAN];
static u32 InputBuffer[SI_MAX_CHAN][2];
static vu32 InputBufferVcount[SI_MAX_CHAN];

static BOOL __SITransfer(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes, SICallback callback);
static void GetTypeCallback(s32 chan, u32 error, OSContext* context);

BOOL SIBusy(void) {
    return Si.chan != -1 ? TRUE : FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return Packet[chan].chan != -1 || Si.chan == chan;
}

static void SIClearTCInterrupt(void) {
    u32 reg;

    reg = __SIRegs[0xD];
    reg |= 0x80000000;
    reg &= ~0x00000001;
    __SIRegs[0xD] = reg;
}

static u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u8* input;

    sr = __SIRegs[0xE];
    SIClearTCInterrupt();

    if (Si.chan != -1) {
        XferTime[Si.chan] = __OSGetSystemTime();

        input = Si.input;
        rLen = Si.inputBytes / 4;
        for (i = 0; i < rLen; i++) {
            *(u32*)input = __SIRegs[0x20 + i];
            input += 4;
        }

        rLen = Si.inputBytes & 3;
        if (rLen) {
            u32 temp = __SIRegs[0x20 + i];
            for (i = 0; i < rLen; i++) {
                *input++ = (u8)((temp >> ((3 - i) * 8)) & 0xFF);
            }
        }

        if (__SIRegs[0xD] & 0x20000000) {
            sr >>= 8 * (3 - Si.chan);
            sr &= 0xF;

            if ((sr & SI_ERROR_NO_RESPONSE) && !(Type[Si.chan] & SI_ERROR_BUSY)) {
                Type[Si.chan] = SI_ERROR_NO_RESPONSE;
            }

            if (sr == 0) {
                sr = SI_ERROR_COLLISION;
            }
        } else {
            TypeTime[Si.chan] = __OSGetSystemTime();
            sr = 0;
        }

        Si.chan = -1;
    }

    return sr;
}

static void SITransferNext(s32 chan) {
    int i;
    SIPacket* packet;

    for (i = 0; i < SI_MAX_CHAN; ++i) {
        ++chan;
        chan %= SI_MAX_CHAN;
        packet = &Packet[chan];

        if (packet->chan != -1 && packet->fire <= __OSGetSystemTime()) {
            if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input, packet->inputBytes, packet->callback)) {
                OSCancelAlarm(&Alarm[chan]);
                packet->chan = -1;
            }
            break;
        }
    }
}

static void SIInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 reg;

    reg = __SIRegs[0xD];

    if ((reg & 0xC0000000) == 0xC0000000) {
        s32 chan;
        u32 sr;
        SICallback callback;

        chan = Si.chan;
        sr = CompleteTransfer();
        callback = Si.callback;
        Si.callback = NULL;

        SITransferNext(chan);

        if (callback) {
            callback(chan, sr, context);
        }

        sr = __SIRegs[0xE];
        sr &= 0x0F000000 >> (8 * chan);
        __SIRegs[0xE] = sr;

        if (Type[chan] == SI_ERROR_BUSY && !SIIsChanBusy(chan)) {
            static u32 cmdTypeAndStatus = 0;
            SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback, OSMicrosecondsToTicks(65));
        }
    }

    if ((reg & 0x18000000) == 0x18000000) {
        int i;
        u32 vcount;
        u32 x;

        vcount = VIGetCurrentLine() + 1;
        x = (Si.poll & 0x03FF0000) >> 16;

        for (i = 0; i < SI_MAX_CHAN; ++i) {
            if (SIGetResponseRaw(i)) {
                InputBufferVcount[i] = vcount;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; ++i) {
            if (!(Si.poll & (0x80000000 >> (31 - 7 + i)))) {
                continue;
            }

            if (InputBufferVcount[i] == 0 || InputBufferVcount[i] + (x / 2) < vcount) {
                return;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; ++i) {
            InputBufferVcount[i] = 0;
        }

        for (i = 0; i < 4; ++i) {
            if (RDSTHandler[i]) {
                RDSTHandler[i](interrupt, context);
            }
        }
    }
}

BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL rc;
    u32 reg;
    int i;

    enabled = OSDisableInterrupts();
    reg = __SIRegs[0xD];
    rc = (reg & 0x08000000) ? TRUE : FALSE;

    if (enable) {
        reg |= 0x08000000;
        for (i = 0; i < SI_MAX_CHAN; ++i) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~0x08000000;
    }

    reg &= ~0x80000001;
    __SIRegs[0xD] = reg;
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL SIUnregisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            RDSTHandler[i] = NULL;

            for (i = 0; i < 4; ++i) {
                if (RDSTHandler[i]) {
                    break;
                }
            }

            if (i == 4) {
                SIEnablePollingInterrupt(FALSE);
            }

            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SIInit(void) {
    static BOOL Initialized = FALSE;

    if (Initialized) {
        return;
    }

    OSRegisterVersion(__SIVersion);

    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = -1;

    Si.poll = 0;
    SISetSamplingRate(0);

    while (__SIRegs[0xD] & 1) {
    }

    __SIRegs[0xD] = 0x80000000;

    __OSSetInterruptHandler(OS_INTERRUPT_PI_SI, SIInterruptHandler);
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_SI);

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);

    Initialized = TRUE;
}

static BOOL __SITransfer(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes, SICallback callback) {
    BOOL enabled;
    u32 rLen;
    u32 i;
    u32 sr;
    SIComcsr comcsr;

    enabled = OSDisableInterrupts();

    if (Si.chan != -1) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    sr = __SIRegs[0xE];
    sr &= 0x0F000000 >> (8 * chan);
    __SIRegs[0xE] = sr;

    Si.chan = chan;
    Si.callback = callback;
    Si.inputBytes = inputBytes;
    Si.input = input;

    rLen = ROUND(outputBytes, 4) / 4;
    for (i = 0; i < rLen; i++) {
        __SIRegs[0x20 + i] = ((u32*)output)[i];
    }

    comcsr.val = __SIRegs[0xD];
    comcsr.f.tcint = 1;
    comcsr.f.tcintmsk = callback ? 1 : 0;
    comcsr.f.outlngth = (outputBytes == SI_MAX_COMCSR_OUTLNGTH) ? 0 : outputBytes;
    comcsr.f.inlngth = (inputBytes == SI_MAX_COMCSR_INLNGTH) ? 0 : inputBytes;
    comcsr.f.channel = chan;
    comcsr.f.tstart = 1;
    __SIRegs[0xD] = comcsr.val;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;
    int chanShift;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[0xE];
    chanShift = 8 * (3 - chan);
    sr >>= chanShift;

    if (sr & SI_ERROR_NO_RESPONSE) {
        if (!(Type[chan] & SI_ERROR_BUSY)) {
            Type[chan] = SI_ERROR_NO_RESPONSE;
        }
    }

    OSRestoreInterrupts(enabled);
    return sr;
}

static BOOL SIGetResponseRaw(s32 chan) {
    u32 sr;

    sr = SIGetStatus(chan);
    if (sr & SI_ERROR_RDST) {
        InputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        InputBuffer[chan][1] = __SIRegs[3 * chan + 2];
        InputBufferValid[chan] = TRUE;
        return TRUE;
    }

    return FALSE;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

void SITransferCommands(void) {
    __SIRegs[0xE] = 0x80000000;
}

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;

    enabled = OSDisableInterrupts();
    Si.poll &= ~(0x03FF0000 | 0x0000FF00);
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[0xC] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();

    poll >>= 24;
    en = poll & 0xF0;
    poll &= (en >> 4) | 0x03FFFFF0;

    Si.poll &= ~(en >> 4);
    Si.poll |= poll;
    poll = Si.poll;

    __SIRegs[0xE] = 0x80000000;
    __SIRegs[0xC] = poll;

    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();

    poll >>= 24;
    poll &= 0xF0;
    poll = Si.poll & ~poll;

    __SIRegs[0xC] = poll;
    Si.poll = poll;

    OSRestoreInterrupts(enabled);
    return poll;
}

BOOL SIGetResponse(s32 chan, void* data) {
    BOOL rc;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIGetResponseRaw(chan);
    rc = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;

    if (rc) {
        ((u32*)data)[0] = InputBuffer[chan][0];
        ((u32*)data)[1] = InputBuffer[chan][1];
    }

    OSRestoreInterrupts(enabled);
    return rc;
}

static void AlarmHandler(OSAlarm* alarm, OSContext* context) {
    s32 chan;
    SIPacket* packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];

    if (packet->chan != -1) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input, packet->inputBytes, packet->callback)) {
            packet->chan = -1;
        }
    }
}

BOOL SITransfer(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes, SICallback callback, OSTime delay) {
    BOOL enabled;
    SIPacket* packet = &Packet[chan];
    OSTime now;
    OSTime fire;

    enabled = OSDisableInterrupts();

    if (packet->chan != -1 || Si.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();

    if (delay == 0) {
        fire = now;
    } else {
        fire = XferTime[chan] + delay;
    }

    if (now < fire) {
        delay = fire - now;
        OSSetAlarm(&Alarm[chan], delay, AlarmHandler);
    } else if (__SITransfer(chan, output, outputBytes, input, inputBytes, callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->fire = fire;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static void CallTypeAndStatusCallback(s32 chan, u32 type) {
    SITypeAndStatusCallback callback;
    int i;

    for (i = 0; i < 4; ++i) {
        callback = TypeCallback[chan][i];
        if (callback) {
            TypeCallback[chan][i] = NULL;
            callback(chan, type);
        }
    }
}

static void GetTypeCallback(s32 chan, u32 error, OSContext* context) {
    static u32 cmdFixDevice[SI_MAX_CHAN];
    u32 type;
    u32 chanBit;
    BOOL fix;
    u32 id;

    Type[chan] &= ~SI_ERROR_BUSY;
    Type[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type[chan];

    chanBit = 0x80000000 >> chan;
    fix = __PADFixBits & chanBit;
    __PADFixBits &= ~chanBit;

    if ((error & (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_NO_RESPONSE | SI_ERROR_COLLISION)) ||
        (type & 0x18000000) != 0x08000000 || !(type & 0x80000000) || (type & 0x04000000)) {
        OSSetWirelessID(chan, 0);
        CallTypeAndStatusCallback(chan, Type[chan]);
        return;
    }

    id = (u32)OSGetWirelessID(chan) << 8;

    if (fix && (id & 0x00100000)) {
        cmdFixDevice[chan] = 0x4E100000 | (id & 0x00CFFF00);
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    }

    if (type & 0x00100000) {
        if ((id & 0x00CFFF00) != (type & 0x00CFFF00)) {
            if (!(id & 0x00100000)) {
                id = type & 0x00CFFF00;
                id |= 0x00100000;
                OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
            }

            cmdFixDevice[chan] = 0x4E000000 | id;
            Type[chan] = SI_ERROR_BUSY;
            SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
            return;
        }
    } else if (type & 0x40000000) {
        id = type & 0x00CFFF00;
        id |= 0x00100000;
        OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));

        cmdFixDevice[chan] = 0x4E000000 | id;
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    } else {
        OSSetWirelessID(chan, 0);
    }

    CallTypeAndStatusCallback(chan, Type[chan]);
}

u32 SIGetType(s32 chan) {
    static u32 cmdTypeAndStatus;
    BOOL enabled;
    u32 type;
    OSTime diff;

    enabled = OSDisableInterrupts();

    type = Type[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];

    if (Si.poll & (0x80 >> chan)) {
        if (type != SI_ERROR_NO_RESPONSE) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        } else {
            type = Type[chan] = SI_ERROR_BUSY;
        }
    } else if (diff <= OSMillisecondsToTicks(50) && type != SI_ERROR_NO_RESPONSE) {
        OSRestoreInterrupts(enabled);
        return type;
    } else if (diff <= OSMillisecondsToTicks(75)) {
        Type[chan] = SI_ERROR_BUSY;
    } else {
        type = Type[chan] = SI_ERROR_BUSY;
    }

    TypeTime[chan] = __OSGetSystemTime();

    SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback, OSMicrosecondsToTicks(65));

    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback) {
    BOOL enabled;
    u32 type;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);

    if (Type[chan] & SI_ERROR_BUSY) {
        int i;

        for (i = 0; i < 4; ++i) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }

            if (TypeCallback[chan][i] == NULL) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        callback(chan, type);
    }

    OSRestoreInterrupts(enabled);
    return type;
}
