#include "private/axfx.h"
#include "revolution/axfx.h"
#include "revolution/os/OSInterrupt.h"

#include <math.h>
#include <string.h>

#define EARLY_MODE_MAX 8
#define FUSED_MODE_MAX 6

static BOOL __AllocDelayLine(AXFX_REVERBSTD_EXP* reverb);
static void __BzeroDelayLines(AXFX_REVERBSTD_EXP* reverb);
static void __FreeDelayLine(AXFX_REVERBSTD_EXP* reverb);
static BOOL __InitParams(AXFX_REVERBSTD_EXP* reverb);

static u32 __EarlySizeTable[EARLY_MODE_MAX] = {163, 317, 479, 641, 797, 967, 1123, 1283};

static u32 __FilterSizeTable[FUSED_MODE_MAX + 1][4] = {
    {1789, 1999, 433, 149}, {149, 293, 251, 103},   {947, 1361, 433, 137},  {1279, 1531, 509, 149},
    {1531, 1847, 563, 179}, {1823, 2357, 571, 137}, {1823, 2357, 571, 179},
};

u32 AXFXReverbStdExpGetMemSize(AXFX_REVERBSTD_EXP* reverb) {
    s32 i;
    s32 memSize = 0;

    memSize += __EarlySizeTable[EARLY_MODE_MAX - 1];
    memSize += (s32)(AXFX_SAMPLE_RATE * reverb->preDelayTimeMax);

    for (i = 0; i < 2; i++) {
        memSize += __FilterSizeTable[FUSED_MODE_MAX][i];
    }

    for (i = 0; i < 2; i++) {
        memSize += __FilterSizeTable[FUSED_MODE_MAX][i + 2];
    }

    return memSize * sizeof(f32) * AXFX_STEREO_CHANNEL_MAX;
}

BOOL AXFXReverbStdExpInit(AXFX_REVERBSTD_EXP* reverb) {
    u32 i;
    BOOL result = TRUE;
    BOOL enabled = OSDisableInterrupts();

    reverb->active = 1;

    if (reverb->preDelayTimeMax < 0.0f) {
        AXFXReverbStdExpShutdown(reverb);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    reverb->earlyMaxLength = __EarlySizeTable[EARLY_MODE_MAX - 1];
    reverb->preDelayMaxLength = (u32)(AXFX_SAMPLE_RATE * reverb->preDelayTimeMax);

    for (i = 0; i < 2; i++) {
        reverb->combMaxLength[i] = __FilterSizeTable[FUSED_MODE_MAX][i];
    }

    for (i = 0; i < 2; i++) {
        reverb->allpassMaxLength[i] = __FilterSizeTable[FUSED_MODE_MAX][i + 2];
    }

    result = __AllocDelayLine(reverb);
    if (!result) {
        AXFXReverbStdExpShutdown(reverb);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    __BzeroDelayLines(reverb);

    result = __InitParams(reverb);
    if (!result) {
        AXFXReverbStdExpShutdown(reverb);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    reverb->active &= ~1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

BOOL AXFXReverbStdExpSettings(AXFX_REVERBSTD_EXP* reverb) {
    BOOL result = TRUE;
    BOOL enabled = OSDisableInterrupts();

    reverb->active |= 1;

    AXFXReverbStdExpShutdown(reverb);
    result = AXFXReverbStdExpInit(reverb);
    if (!result) {
        AXFXReverbStdExpShutdown(reverb);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    reverb->active |= 2;
    reverb->active &= ~1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

BOOL AXFXReverbStdExpSettingsUpdate(AXFX_REVERBSTD_EXP* reverb) {
    BOOL result = TRUE;
    BOOL enabled = OSDisableInterrupts();

    reverb->active |= 1;

    __BzeroDelayLines(reverb);

    result = __InitParams(reverb);
    if (!result) {
        AXFXReverbStdExpShutdown(reverb);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    reverb->active |= 2;
    reverb->active &= ~1;

    OSRestoreInterrupts(enabled);

    return TRUE;
}

void AXFXReverbStdExpShutdown(AXFX_REVERBSTD_EXP* reverb) {
    BOOL enabled = OSDisableInterrupts();

    reverb->active |= 1;

    __FreeDelayLine(reverb);

    OSRestoreInterrupts(enabled);
}

void AXFXReverbStdExpCallback(AXFX_BUS* bus, AXFX_REVERBSTD_EXP* reverb) {
    u32 i, j;
    s32* busData[AXFX_STEREO_CHANNEL_MAX];
    s32* busInData[AXFX_STEREO_CHANNEL_MAX];
    s32* busOutData[AXFX_STEREO_CHANNEL_MAX];
    u32 earlyPos;
    u32 preDelayPos;
    u32 combPos0;
    u32 combPos1;
    u32 allpassPos0;
    u32 allpassPos1;
    f32 input;
    f32 output;
    f32* earlyLine;
    f32 earlyOut;
    f32 earlyCoef;
    f32* preDelayLine;
    f32 preDelayOut;
    f32* combLine0;
    f32* combLine1;
    f32 combOut0;
    f32 combOut1;
    f32 combOut;
    f32 combCoef0;
    f32 combCoef1;
    f32 allpassDelayed;
    f32 allpassIn;
    f32 allpassOut0;
    f32 allpassOut1;
    f32 allpassCoef;
    f32 lpfOut;
    f32 lpfInCoef;
    f32 lpfCoef;
    f32 earlyGain;
    f32 fusedGain;
    f32* allpassLine;

    if (reverb->active != 0) {
        reverb->active &= ~2;
        return;
    }

    busData[AXFX_CHANNEL_L] = bus->left;
    busData[AXFX_CHANNEL_R] = bus->right;
    busData[AXFX_CHANNEL_S] = bus->surround;

    if (reverb->busIn != NULL) {
        busInData[AXFX_CHANNEL_L] = reverb->busIn->left;
        busInData[AXFX_CHANNEL_R] = reverb->busIn->right;
        busInData[AXFX_CHANNEL_S] = reverb->busIn->surround;
    }

    if (reverb->busOut != NULL) {
        busOutData[AXFX_CHANNEL_L] = reverb->busOut->left;
        busOutData[AXFX_CHANNEL_R] = reverb->busOut->right;
        busOutData[AXFX_CHANNEL_S] = reverb->busOut->surround;
    }

    lpfInCoef = 1.0f - reverb->lpfCoef;
    lpfCoef = reverb->lpfCoef;
    earlyCoef = reverb->earlyCoef;
    combCoef0 = reverb->combCoef[0];
    combCoef1 = reverb->combCoef[1];
    allpassCoef = reverb->allpassCoef;
    earlyGain = 0.6f * reverb->earlyGain;
    fusedGain = 0.6f * reverb->fusedGain;

    for (i = 0; i < AXFX_SAMPLES_PER_FRAME; i++) {
        earlyPos = reverb->earlyPos;
        preDelayPos = reverb->preDelayPos;
        combPos0 = reverb->combPos[0];
        combPos1 = reverb->combPos[1];
        allpassPos0 = reverb->allpassPos[0];
        allpassPos1 = reverb->allpassPos[1];

        for (j = 0; j < AXFX_STEREO_CHANNEL_MAX; j++) {
            if (reverb->busIn != NULL) {
                input = *busData[j] + *busInData[j]++;
            } else {
                input = *busData[j];
            }

            earlyLine = reverb->earlyLine[j];
            earlyOut = earlyLine[earlyPos];
            earlyLine[earlyPos] = input + (earlyOut * earlyCoef);

            if (reverb->preDelayLength != 0) {
                preDelayLine = reverb->preDelayLine[j];
                preDelayOut = preDelayLine[preDelayPos];
                preDelayLine[preDelayPos] = input;
            } else {
                preDelayOut = input;
            }

            combLine0 = reverb->combLine[j][0];
            combOut0 = combLine0[combPos0];
            combLine0[combPos0] = preDelayOut + (combOut0 * combCoef0);

            combLine1 = reverb->combLine[j][1];
            combOut1 = combLine1[combPos1];
            combLine1[combPos1] = preDelayOut + (combOut1 * combCoef1);

            combOut = combOut0 + combOut1;

            allpassLine = reverb->allpassLine[j][0];
            allpassDelayed = allpassLine[allpassPos0];
            allpassIn = combOut + (allpassDelayed * allpassCoef);
            allpassLine[allpassPos0] = allpassIn;
            allpassOut0 = allpassDelayed - (allpassIn * allpassCoef);

            lpfOut = (lpfInCoef * allpassOut0) + (lpfCoef * reverb->lastLpfOut[j]);
            reverb->lastLpfOut[j] = lpfOut;

            allpassLine = reverb->allpassLine[j][1];
            allpassDelayed = allpassLine[allpassPos1];
            allpassIn = lpfOut + (allpassDelayed * allpassCoef);
            allpassLine[allpassPos1] = allpassIn;
            allpassOut1 = allpassDelayed - (allpassIn * allpassCoef);

            output = (earlyOut * earlyGain) + (allpassOut1 * fusedGain);

            *busData[j]++ = output * reverb->outGain;

            if (reverb->busOut != NULL) {
                *busOutData[j]++ = output * reverb->sendGain;
            }
        }

        if (++reverb->earlyPos >= reverb->earlyLength) {
            reverb->earlyPos = 0;
        }

        if (reverb->preDelayLength != 0) {
            if (++reverb->preDelayPos >= reverb->preDelayLength) {
                reverb->preDelayPos = 0;
            }
        }

        if (++reverb->combPos[0] >= reverb->combLength[0]) {
            reverb->combPos[0] = 0;
        }

        if (++reverb->combPos[1] >= reverb->combLength[1]) {
            reverb->combPos[1] = 0;
        }

        if (++reverb->allpassPos[0] >= reverb->allpassLength[0]) {
            reverb->allpassPos[0] = 0;
        }

        if (++reverb->allpassPos[1] >= reverb->allpassLength[1]) {
            reverb->allpassPos[1] = 0;
        }
    }
}

static BOOL __AllocDelayLine(AXFX_REVERBSTD_EXP* reverb) {
    u32 i, j;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        reverb->earlyLine[i] = __AXFXAlloc(reverb->earlyMaxLength * sizeof(f32));
        if (reverb->earlyLine[i] == NULL) {
            return FALSE;
        }

        if (reverb->preDelayMaxLength != 0) {
            reverb->preDelayLine[i] = __AXFXAlloc(reverb->preDelayMaxLength * sizeof(f32));
            if (reverb->preDelayLine[i] == NULL) {
                return FALSE;
            }
        } else {
            reverb->preDelayLine[i] = NULL;
        }

        for (j = 0; j < 2; j++) {
            reverb->combLine[i][j] = __AXFXAlloc(reverb->combMaxLength[j] * sizeof(f32));
            if (reverb->combLine[i][j] == NULL) {
                return FALSE;
            }
        }

        for (j = 0; j < 2; j++) {
            reverb->allpassLine[i][j] = __AXFXAlloc(reverb->allpassMaxLength[j] * sizeof(f32));
            if (reverb->allpassLine[i][j] == NULL) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static void __BzeroDelayLines(AXFX_REVERBSTD_EXP* reverb) {
    u32 i, j;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        if (reverb->earlyLine[i] != NULL) {
            memset(reverb->earlyLine[i], 0, reverb->earlyMaxLength * sizeof(f32));
        }

        if (reverb->preDelayLine[i] != NULL) {
            memset(reverb->preDelayLine[i], 0, reverb->preDelayMaxLength * sizeof(f32));
        }

        for (j = 0; j < 2; j++) {
            if (reverb->combLine[i][j] != NULL) {
                memset(reverb->combLine[i][j], 0, reverb->combMaxLength[j] * sizeof(f32));
            }
        }

        for (j = 0; j < 2; j++) {
            if (reverb->allpassLine[i][j] != NULL) {
                memset(reverb->allpassLine[i][j], 0, reverb->allpassMaxLength[j] * sizeof(f32));
            }
        }
    }
}

static void __FreeDelayLine(AXFX_REVERBSTD_EXP* reverb) {
    u32 i, j;

    for (i = 0; i < AXFX_STEREO_CHANNEL_MAX; i++) {
        if (reverb->earlyLine[i] != NULL) {
            __AXFXFree(reverb->earlyLine[i]);
            reverb->earlyLine[i] = NULL;
        }

        if (reverb->preDelayLine[i] != NULL) {
            __AXFXFree(reverb->preDelayLine[i]);
            reverb->preDelayLine[i] = NULL;
        }

        for (j = 0; j < 2; j++) {
            if (reverb->combLine[i][j] != NULL) {
                __AXFXFree(reverb->combLine[i][j]);
                reverb->combLine[i][j] = NULL;
            }
        }

        for (j = 0; j < 2; j++) {
            if (reverb->allpassLine[i][j] != NULL) {
                __AXFXFree(reverb->allpassLine[i][j]);
                reverb->allpassLine[i][j] = NULL;
            }
        }
    }
}

static BOOL __InitParams(AXFX_REVERBSTD_EXP* reverb) {
    u32 i, j;

    if (reverb->earlyMode >= EARLY_MODE_MAX) {
        return FALSE;
    }

    if (reverb->preDelayTime < 0.0f || reverb->preDelayTime > reverb->preDelayTimeMax) {
        return FALSE;
    }

    if (reverb->fusedMode >= FUSED_MODE_MAX) {
        return FALSE;
    }

    if (reverb->fusedTime < 0.0f) {
        return FALSE;
    }

    if (reverb->coloration < 0.0f || reverb->coloration > 1.0f) {
        return FALSE;
    }

    if (reverb->damping < 0.0f || reverb->damping > 1.0f) {
        return FALSE;
    }

    if (reverb->earlyGain < 0.0f || reverb->earlyGain > 1.0f) {
        return FALSE;
    }

    if (reverb->fusedGain < 0.0f || reverb->fusedGain > 1.0f) {
        return FALSE;
    }

    if (reverb->outGain < 0.0f || reverb->outGain > 1.0f) {
        return FALSE;
    }

    if (reverb->sendGain < 0.0f || reverb->sendGain > 1.0f) {
        return FALSE;
    }

    reverb->earlyPos = 0;
    reverb->earlyLength = __EarlySizeTable[reverb->earlyMode];

    if (reverb->earlyMode <= 3) {
        reverb->earlyCoef = -0.33f;
    } else {
        reverb->earlyCoef = 0.33f;
    }

    reverb->preDelayPos = 0;
    reverb->preDelayLength = reverb->preDelayTime * AXFX_SAMPLE_RATE;

    for (i = 0; i < 2; i++) {
        reverb->combPos[i] = 0;
        reverb->combLength[i] = __FilterSizeTable[reverb->fusedMode][i];
        reverb->combCoef[i] = powf(10.0f, (reverb->combLength[i] * -3.0f) / (reverb->fusedTime * AXFX_SAMPLE_RATE));
    }

    for (i = 0; i < 2; i++) {
        reverb->allpassPos[i] = 0;
        reverb->allpassLength[i] = __FilterSizeTable[reverb->fusedMode][i + 2];
    }

    reverb->allpassCoef = reverb->coloration;
    reverb->lpfCoef = 1.0f - reverb->damping;

    if (reverb->lpfCoef > 0.95f) {
        reverb->lpfCoef = 0.95f;
    }

    for (j = 0; j < AXFX_STEREO_CHANNEL_MAX; j++) {
        reverb->lastLpfOut[j] = 0.0f;
    }

    return TRUE;
}
