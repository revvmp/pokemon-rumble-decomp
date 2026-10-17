#include "private/axfx.h"
#include "revolution/axfx.h"
#include "revolution/os/OSInterrupt.h"

#include <string.h>

#define DELAY_LINE_SIZE 3200
#define LFO_TABLE_SIZE 128

static BOOL __AllocDelay(AXFX_CHORUS_EXP* chorus);
static BOOL __InitDelay(AXFX_CHORUS_EXP* chorus);
static void __FreeDelay(AXFX_CHORUS_EXP* chorus);
static BOOL __InitParams(AXFX_CHORUS_EXP* chorus);
static void __CalcLFO(s32* lfoBuf, AXFX_CHORUS_EXP_LFO* lfo);

u32 AXFXChorusExpGetMemSize(AXFX_CHORUS_EXP* chorus) {
    return DELAY_LINE_SIZE * sizeof(f32) * AXFX_STEREO_CHANNEL_MAX;
}

BOOL AXFXChorusExpInit(AXFX_CHORUS_EXP* chorus) {
    BOOL result;
    BOOL enabled = OSDisableInterrupts();

    chorus->active = 1;

    result = __AllocDelay(chorus);
    if (result == FALSE) {
        AXFXChorusExpShutdown(chorus);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    result = __InitDelay(chorus);
    if (result == FALSE) {
        AXFXChorusExpShutdown(chorus);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    result = __InitParams(chorus);
    if (result == FALSE) {
        AXFXChorusExpShutdown(chorus);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    chorus->active &= ~1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

BOOL AXFXChorusExpSettingsUpdate(AXFX_CHORUS_EXP* chorus) {
    BOOL result;
    BOOL enabled = OSDisableInterrupts();

    chorus->active |= 1;

    result = __InitDelay(chorus);
    if (!result) {
        AXFXChorusExpShutdown(chorus);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    result = __InitParams(chorus);
    if (!result) {
        AXFXChorusExpShutdown(chorus);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    chorus->active |= 2;
    chorus->active &= ~1;

    OSRestoreInterrupts(enabled);

    return result;
}

void AXFXChorusExpShutdown(AXFX_CHORUS_EXP* chorus) {
    BOOL enabled = OSDisableInterrupts();

    chorus->active |= 1;

    __FreeDelay(chorus);

    OSRestoreInterrupts(enabled);
}

void AXFXChorusExpCallback(AXFX_BUS* bus, AXFX_CHORUS_EXP* chorus) {
    u32 i;
    u32 j;
    u32 k;
    s32 lfoBuf[AXFX_SAMPLES_PER_FRAME];
    s32* busData[AXFX_STEREO_CHANNEL_MAX];
    s32* busInData[AXFX_STEREO_CHANNEL_MAX];
    s32* busOutData[AXFX_STEREO_CHANNEL_MAX];
    s32 lfoValue;
    s32 outPos;
    s32 advance;
    u32 advanceSamples;
    s32 fraction;
    u32 readPos;
    f32* srcCoef;
    u32 srcCoefIndex;
    AXFX_CHORUS_EXP* pChorus;
    f32 input;
    f32 output;
    u32 histIndex;

    if (chorus->active != 0) {
        chorus->active &= ~2;
        return;
    }

    busData[AXFX_CHANNEL_L] = bus->left;
    busData[AXFX_CHANNEL_R] = bus->right;
    busData[AXFX_CHANNEL_S] = bus->surround;

    if (chorus->busIn != NULL) {
        busInData[AXFX_CHANNEL_L] = chorus->busIn->left;
        busInData[AXFX_CHANNEL_R] = chorus->busIn->right;
        busInData[AXFX_CHANNEL_S] = chorus->busIn->surround;
    }

    if (chorus->busOut != NULL) {
        busOutData[AXFX_CHANNEL_L] = chorus->busOut->left;
        busOutData[AXFX_CHANNEL_R] = chorus->busOut->right;
        busOutData[AXFX_CHANNEL_S] = chorus->busOut->surround;
    }

    pChorus = chorus;
    __CalcLFO(lfoBuf, &chorus->lfo);

    for (i = 0; i < AXFX_SAMPLES_PER_FRAME; i++) {
        lfoValue = lfoBuf[i];
        outPos = pChorus->delay.outPos + lfoValue;

        if (outPos >= (s32)pChorus->delay.sizeFP) {
            outPos -= pChorus->delay.sizeFP;
        } else if (outPos < 0) {
            outPos += pChorus->delay.sizeFP;
        }

        advance = outPos - pChorus->delay.lastPos;
        if (advance < 0) {
            advance += pChorus->delay.sizeFP;
        }

        advanceSamples = (advance & 0xFFFF0000) >> 16;
        fraction = advance & 0xFFFF;
        readPos = pChorus->delay.lastPos >> 16;
        histIndex = chorus->histIndex;

        while (advanceSamples--) {
            chorus->history[AXFX_CHANNEL_L][histIndex] = pChorus->delay.line[AXFX_CHANNEL_L][readPos];
            chorus->history[AXFX_CHANNEL_R][histIndex] = pChorus->delay.line[AXFX_CHANNEL_R][readPos];
            chorus->history[AXFX_CHANNEL_S][histIndex++] = pChorus->delay.line[AXFX_CHANNEL_S][readPos++];
            histIndex &= 3;

            if (readPos >= pChorus->delay.size) {
                readPos = 0;
            }
        }

        srcCoefIndex = (u32)(fraction & 0xFE00) >> 9;
        pChorus->delay.lastPos = outPos & 0xFFFF0000;
        srcCoef = __AXFXGetSrcCoef(srcCoefIndex);

        for (j = 0; j < AXFX_STEREO_CHANNEL_MAX; j++) {
            output = 0.0f;

            for (k = 0; k < 4; k++) {
                output += srcCoef[k] * chorus->history[j][histIndex++];
                histIndex &= 3;
            }

            if (chorus->busIn != NULL) {
                input = *busData[j] + *busInData[j]++;
            } else {
                input = *busData[j];
            }

            pChorus->delay.line[j][pChorus->delay.inPos] = input + (output * chorus->feedback);

            *busData[j]++ = output * chorus->outGain;

            if (chorus->busOut != NULL) {
                *busOutData[j]++ = output * chorus->sendGain;
            }
        }

        chorus->histIndex = histIndex;

        if (++pChorus->delay.inPos >= pChorus->delay.size) {
            pChorus->delay.inPos = 0;
        }

        if ((pChorus->delay.outPos += 0x10000) >= pChorus->delay.sizeFP) {
            pChorus->delay.outPos = 0;
        }
    }
}

static BOOL __AllocDelay(AXFX_CHORUS_EXP* chorus) {
    AXFX_CHORUS_EXP* pChorus = chorus;
    u32 i;

    pChorus->delay.size = DELAY_LINE_SIZE;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        pChorus->delay.line[i] = __AXFXAlloc(pChorus->delay.size * sizeof(f32));
        if (pChorus->delay.line[i] == NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL __InitDelay(AXFX_CHORUS_EXP* chorus) {
    AXFX_CHORUS_EXP* pChorus = chorus;
    u32 i;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        if (pChorus->delay.line[i] == NULL) {
            return FALSE;
        }

        memset(pChorus->delay.line[i], 0, pChorus->delay.size * sizeof(f32));
    }

    pChorus->delay.inPos = 0;
    pChorus->delay.outPos = pChorus->delay.size - (u32)(32.0f * chorus->delayTime);
    pChorus->delay.outPos <<= 16;
    pChorus->delay.lastPos = pChorus->delay.outPos;
    pChorus->delay.sizeFP = pChorus->delay.size << 16;

    return TRUE;
}

static void __FreeDelay(AXFX_CHORUS_EXP* chorus) {
    u32 i;
    AXFX_CHORUS_EXP* pChorus = chorus;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        if (pChorus->delay.line[i] != NULL) {
            __AXFXFree(pChorus->delay.line[i]);
        }

        pChorus->delay.line[i] = NULL;
    }
}

static BOOL __InitParams(AXFX_CHORUS_EXP* chorus) {
    u32 i, j;
    f32 gradient;
    f32 delaySamples;
    f32 stepSamples;
    f32 depthSamples;

    if (chorus->delayTime < 0.1f || chorus->delayTime > 50.0f) {
        return FALSE;
    }

    if (chorus->depth < 0.0f || chorus->depth > 1.0f) {
        return FALSE;
    }

    if (chorus->rate < 0.1f || chorus->rate > 2.0f) {
        return FALSE;
    }

    if (chorus->feedback < 0.0f || chorus->feedback >= 1.0f) {
        return FALSE;
    }

    if (chorus->outGain < 0.0f || chorus->outGain > 1.0f) {
        return FALSE;
    }

    if (chorus->sendGain < 0.0f || chorus->sendGain > 1.0f) {
        return FALSE;
    }

    chorus->lfo.table = __AXFXGetLfoSinTable();

    delaySamples = 32.0f * chorus->delayTime;
    depthSamples = delaySamples * chorus->depth;

    if (depthSamples >= delaySamples) {
        depthSamples -= 1.0f;

        if (depthSamples < 0.0f) {
            depthSamples = 0.0f;
        }
    }

    chorus->lfo.depthSamp = 65536.0f * depthSamples;
    chorus->lfo.phaseAdd = 65536.0f * ((256.0f * chorus->rate) / AXFX_SAMPLE_RATE);
    stepSamples = (AXFX_SAMPLE_RATE / chorus->rate) / 256.0f;
    chorus->lfo.stepSamp = 65536.0f * stepSamples;
    gradient = depthSamples / stepSamples;
    chorus->lfo.gradFactor = 65536.0f * gradient;
    chorus->lfo.phase = 0;
    chorus->lfo.sign = 0;
    chorus->lfo.lastNum = -1;
    chorus->lfo.lastValue = 0;
    chorus->lfo.grad = 0;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        for (j = 0; j < 4; j++) {
            chorus->history[i][j] = 0.0f;
        }
    }

    chorus->histIndex = 0;

    return TRUE;
}

static void __CalcLFO(s32* lfoBuf, AXFX_CHORUS_EXP_LFO* lfo) {
    u32 i;
    s32 nextValue;
    s32 value;
    s32 nextIndex;
    u32 index;
    s64 result;
    s64 grad;

    result = 0;

    for (i = 0; i < AXFX_SAMPLES_PER_FRAME; i++) {
        index = lfo->phase & 0xFFFF0000;

        if (index != lfo->lastNum) {
            lfo->lastNum = index;
            index = index >> 16;
            nextIndex = index + 1;
            nextIndex &= LFO_TABLE_SIZE - 1;
            value = lfo->table[index];
            nextValue = lfo->table[nextIndex];
            grad = nextValue - value;
            grad = grad * lfo->gradFactor;
            grad = grad >> 24;
            lfo->grad = grad;
            result = (s64)value * (s64)lfo->depthSamp;
            result = result >> 24;
        } else {
            result = lfo->lastValue + lfo->grad;
        }

        lfo->lastValue = result;

        if (lfo->sign >= 1) {
            result *= -1;
        }

        lfo->phase += lfo->phaseAdd;

        if ((lfo->phase & 0xFF800000) != 0) {
            lfo->phase &= 0x7FFFFF;
            lfo->sign ^= 1;
        }

        lfoBuf[i] = result;
    }
}
