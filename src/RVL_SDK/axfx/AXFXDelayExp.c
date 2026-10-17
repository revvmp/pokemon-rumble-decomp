#include <revolution/os/OSThread.h>
#include <revolution/os.h>
#include <revolution/axfx.h>
#include <revolution/os.h>
#include <string.h>

static BOOL __AllocDelayLine(AXFX_DELAY_EXP* fx);
static void __FreeDelayLine(AXFX_DELAY_EXP* fx);
static BOOL __InitParams(AXFX_DELAY_EXP* fx) NO_INLINE;

u32 AXFXDelayExpGetMemSize(AXFX_DELAY_EXP* fx) {
    return (s32)(32.0f * fx->maxDelay) * 12;
}

BOOL AXFXDelayExpInit(AXFX_DELAY_EXP* fx) {
    BOOL enabled = OSDisableInterrupts();
    fx->active = 1;
    if (fx->maxDelay <= 0.0f) {
        AXFXDelayExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    fx->maxLength = (u32)(32.0f * fx->maxDelay);
    if (fx->maxLength == 0) {
        fx->maxLength = 1;
    }
    if (!__AllocDelayLine(fx)) {
        AXFXDelayExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    if (!__InitParams(fx)) {
        AXFXDelayExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL AXFXDelayExpSettings(AXFX_DELAY_EXP* fx) {
    BOOL enabled = OSDisableInterrupts();
    fx->active |= 1;
    AXFXDelayExpShutdown(fx);
    if (!AXFXDelayExpInit(fx)) {
        AXFXDelayExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL AXFXDelayExpSettingsUpdate(AXFX_DELAY_EXP* fx) {
    BOOL enabled = OSDisableInterrupts();
    fx->active |= 1;
    if (!__InitParams(fx)) {
        AXFXDelayExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void AXFXDelayExpShutdown(AXFX_DELAY_EXP* fx) {
    BOOL enabled = OSDisableInterrupts();
    fx->active |= 1;
    __FreeDelayLine(fx);
    OSRestoreInterrupts(enabled);
}

void AXFXDelayExpCallback(AXFX_BUS* bus, AXFX_DELAY_EXP* fx) {
    s32* inputLeft;
    s32* inputRight;
    s32* inputSurround;
    s32* sendLeft;
    s32* sendRight;
    s32* sendSurround;
    s32 lpfCoef;
    s32* left;
    s32* right;
    s32* surround;
    s32 delayedLeft;
    s32 delayedRight;
    s32 delayedSurround;
    s32 filteredLeft;
    s32 filteredRight;
    s32 filteredSurround;
    s32 inverseLpf;
    u32 i;

    if (fx->active) {
        fx->active &= ~2;
        return;
    }
    left = bus->left;
    right = bus->right;
    surround = bus->surround;
    lpfCoef = fx->lpfCoef;
    inverseLpf = 128 - lpfCoef;
    if (fx->busIn) {
        inputLeft = fx->busIn->left;
        inputRight = fx->busIn->right;
        inputSurround = fx->busIn->surround;
    }
    if (fx->busOut) {
        sendLeft = fx->busOut->left;
        sendRight = fx->busOut->right;
        sendSurround = fx->busOut->surround;
    }
    for (i = 0; i < 96; i++) {
        delayedLeft = fx->lineL[fx->curPos];
        delayedRight = fx->lineR[fx->curPos];
        delayedSurround = fx->lineS[fx->curPos];
        if (fx->busIn) {
            filteredLeft = inverseLpf * (*left + *inputLeft++) + lpfCoef * fx->lastLpfOut[0];
            filteredRight = inverseLpf * (*right + *inputRight++) + lpfCoef * fx->lastLpfOut[1];
            filteredSurround = inverseLpf * (*surround + *inputSurround++) + lpfCoef * fx->lastLpfOut[2];
        } else {
            filteredLeft = inverseLpf * *left + lpfCoef * fx->lastLpfOut[0];
            filteredRight = inverseLpf * *right + lpfCoef * fx->lastLpfOut[1];
            filteredSurround = inverseLpf * *surround + lpfCoef * fx->lastLpfOut[2];
        }
        filteredLeft >>= 7;
        filteredRight >>= 7;
        filteredSurround >>= 7;
        fx->lastLpfOut[0] = filteredLeft;
        fx->lastLpfOut[1] = filteredRight;
        fx->lastLpfOut[2] = filteredSurround;
        fx->lineL[fx->curPos] = filteredLeft + ((delayedLeft * fx->feedbackGain) >> 7);
        fx->lineR[fx->curPos] = filteredRight + ((delayedRight * fx->feedbackGain) >> 7);
        fx->lineS[fx->curPos++] = filteredSurround + ((delayedSurround * fx->feedbackGain) >> 7);
        if (fx->curPos >= fx->length) {
            fx->curPos = 0;
        }
        *left++ = (delayedLeft * fx->outGainCalc) >> 7;
        *right++ = (delayedRight * fx->outGainCalc) >> 7;
        *surround++ = (delayedSurround * fx->outGainCalc) >> 7;
        if (fx->busOut) {
            *sendLeft++ = (delayedLeft * fx->sendGainCalc) >> 7;
            *sendRight++ = (delayedRight * fx->sendGainCalc) >> 7;
            *sendSurround++ = (delayedSurround * fx->sendGainCalc) >> 7;
        }
    }
}

static BOOL __AllocDelayLine(AXFX_DELAY_EXP* fx) {
    fx->lineL = __AXFXAlloc(fx->maxLength * sizeof(s32));
    if (!fx->lineL) {
        return FALSE;
    }
    fx->lineR = __AXFXAlloc(fx->maxLength * sizeof(s32));
    if (!fx->lineR) {
        return FALSE;
    }
    fx->lineS = __AXFXAlloc(fx->maxLength * sizeof(s32));
    return fx->lineS != NULL;
}

static void __FreeDelayLine(AXFX_DELAY_EXP* fx) {
    if (fx->lineL) {
        __AXFXFree(fx->lineL);
        fx->lineL = NULL;
    }
    if (fx->lineR) {
        __AXFXFree(fx->lineR);
        fx->lineR = NULL;
    }
    if (fx->lineS) {
        __AXFXFree(fx->lineS);
        fx->lineS = NULL;
    }
}

static BOOL __InitParams(AXFX_DELAY_EXP* fx) {
    f32 lpf;
    if (fx->delay > fx->maxDelay) {
        return FALSE;
    }
    if (fx->feedback < 0.0f || fx->feedback >= 1.0f) {
        return FALSE;
    }
    if (fx->lpf < 0.0f || fx->lpf > 1.0f) {
        return FALSE;
    }
    if (fx->outGain < 0.0f || fx->outGain > 1.0f) {
        return FALSE;
    }
    if (fx->sendGain < 0.0f || fx->sendGain > 1.0f) {
        return FALSE;
    }
    if (!fx->lineL) {
        return FALSE;
    }
    if (!fx->lineR) {
        return FALSE;
    }
    if (!fx->lineS) {
        return FALSE;
    }
    memset(fx->lineL, 0, fx->maxLength * sizeof(s32));
    memset(fx->lineR, 0, fx->maxLength * sizeof(s32));
    memset(fx->lineS, 0, fx->maxLength * sizeof(s32));
    fx->length = (u32)(32.0f * fx->delay);
    if (fx->length == 0) {
        fx->length = 1;
    }
    fx->curPos = 0;
    fx->feedbackGain = (s32)(128.0f * fx->feedback);
    lpf = 1.0f - fx->lpf;
    if (lpf > 0.95f) {
        lpf = 0.95f;
    }
    fx->lpfCoef = (s32)(128.0f * lpf);
    fx->lastLpfOut[0] = 0;
    fx->lastLpfOut[1] = 0;
    fx->lastLpfOut[2] = 0;
    fx->outGainCalc = (s32)(128.0f * fx->outGain);
    fx->sendGainCalc = (s32)(128.0f * fx->sendGain);
    return TRUE;
}
