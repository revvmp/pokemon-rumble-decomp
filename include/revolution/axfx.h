#ifndef REVOLUTION_AXFX_H
#define REVOLUTION_AXFX_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXFX_STEREO_CHANNEL_MAX 3

#define AXFX_CHANNEL_L 0
#define AXFX_CHANNEL_R 1
#define AXFX_CHANNEL_S 2

#define AXFX_SAMPLES_PER_FRAME 96
#define AXFX_SAMPLE_RATE 32000.0f

typedef struct AXFX_BUS {
    s32* left;
    s32* right;
    s32* surround;
} AXFX_BUS;

typedef struct AXFX_REVERBSTD_EXP {
    f32* earlyLine[AXFX_STEREO_CHANNEL_MAX];
    u32 earlyPos;
    u32 earlyLength;
    u32 earlyMaxLength;
    f32 earlyCoef;
    f32* preDelayLine[AXFX_STEREO_CHANNEL_MAX];
    u32 preDelayPos;
    u32 preDelayLength;
    u32 preDelayMaxLength;
    f32* combLine[AXFX_STEREO_CHANNEL_MAX][2];
    u32 combPos[2];
    u32 combLength[2];
    u32 combMaxLength[2];
    f32 combCoef[2];
    f32* allpassLine[AXFX_STEREO_CHANNEL_MAX][2];
    u32 allpassPos[2];
    u32 allpassLength[2];
    u32 allpassMaxLength[2];
    f32 allpassCoef;
    f32 lastLpfOut[AXFX_STEREO_CHANNEL_MAX];
    f32 lpfCoef;
    u32 active;
    u32 earlyMode;
    f32 preDelayTimeMax;
    f32 preDelayTime;
    u32 fusedMode;
    f32 fusedTime;
    f32 coloration;
    f32 damping;
    f32 earlyGain;
    f32 fusedGain;
    AXFX_BUS* busIn;
    AXFX_BUS* busOut;
    f32 outGain;
    f32 sendGain;
} AXFX_REVERBSTD_EXP;

typedef struct AXFX_CHORUS_EXP_LFO {
    s32* table;
    s32 phaseAdd;
    s32 stepSamp;
    s32 depthSamp;
    u32 phase;
    u32 sign;
    u32 lastNum;
    s32 lastValue;
    s32 grad;
    s32 gradFactor;
} AXFX_CHORUS_EXP_LFO;

typedef struct AXFX_CHORUS_EXP_DELAY {
    f32* line[AXFX_STEREO_CHANNEL_MAX];
    u32 inPos;
    u32 outPos;
    u32 lastPos;
    u32 sizeFP;
    u32 size;
} AXFX_CHORUS_EXP_DELAY;

typedef struct AXFX_CHORUS_EXP {
    AXFX_CHORUS_EXP_DELAY delay;
    AXFX_CHORUS_EXP_LFO lfo;
    f32 history[AXFX_STEREO_CHANNEL_MAX][4];
    u32 histIndex;
    u32 active;
    f32 delayTime;
    f32 depth;
    f32 rate;
    f32 feedback;
    AXFX_BUS* busIn;
    AXFX_BUS* busOut;
    f32 outGain;
    f32 sendGain;
} AXFX_CHORUS_EXP;

typedef void* (*AXFXAllocHook)(u32 size);
typedef void (*AXFXFreeHook)(void* block);

void AXFXSetHooks(AXFXAllocHook alloc, AXFXFreeHook free);
void AXFXGetHooks(AXFXAllocHook* alloc, AXFXFreeHook* free);

u32 AXFXReverbStdExpGetMemSize(AXFX_REVERBSTD_EXP* reverb);
BOOL AXFXReverbStdExpInit(AXFX_REVERBSTD_EXP* reverb);
BOOL AXFXReverbStdExpSettings(AXFX_REVERBSTD_EXP* reverb);
BOOL AXFXReverbStdExpSettingsUpdate(AXFX_REVERBSTD_EXP* reverb);
void AXFXReverbStdExpShutdown(AXFX_REVERBSTD_EXP* reverb);
void AXFXReverbStdExpCallback(AXFX_BUS* bus, AXFX_REVERBSTD_EXP* reverb);

u32 AXFXChorusExpGetMemSize(AXFX_CHORUS_EXP* chorus);
BOOL AXFXChorusExpInit(AXFX_CHORUS_EXP* chorus);
BOOL AXFXChorusExpSettingsUpdate(AXFX_CHORUS_EXP* chorus);
void AXFXChorusExpShutdown(AXFX_CHORUS_EXP* chorus);
void AXFXChorusExpCallback(AXFX_BUS* bus, AXFX_CHORUS_EXP* chorus);

#ifdef __cplusplus
}
#endif

#endif
