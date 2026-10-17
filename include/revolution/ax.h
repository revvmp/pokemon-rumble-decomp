#ifndef AX_H
#define AX_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AXPBMIX {
    u16 vL;
    u16 vDeltaL;
    u16 vR;
    u16 vDeltaR;
    u16 vAuxAL;
    u16 vDeltaAuxAL;
    u16 vAuxAR;
    u16 vDeltaAuxAR;
    u16 vAuxBL;
    u16 vDeltaAuxBL;
    u16 vAuxBR;
    u16 vDeltaAuxBR;
    u16 vAuxCL;
    u16 vDeltaAuxCL;
    u16 vAuxCR;
    u16 vDeltaAuxCR;
    u16 vS;
    u16 vDeltaS;
    u16 vAuxAS;
    u16 vDeltaAuxAS;
    u16 vAuxBS;
    u16 vDeltaAuxBS;
    u16 vAuxCS;
    u16 vDeltaAuxCS;
} AXPBMIX;

typedef struct _AXPBITD {
    u16 flag;
    u16 bufferHi;
    u16 bufferLo;
    u16 shiftL;
    u16 shiftR;
    u16 targetShiftL;
    u16 targetShiftR;
} AXPBITD;

typedef struct _AXPBDPOP {
    s16 aL;
    s16 aAuxAL;
    s16 aAuxBL;
    s16 aAuxCL;
    s16 aR;
    s16 aAuxAR;
    s16 aAuxBR;
    s16 aAuxCR;
    s16 aS;
    s16 aAuxAS;
    s16 aAuxBS;
    s16 aAuxCS;
} AXPBDPOP;

typedef struct _AXPBVE {
    u16 currentVolume;
    s16 currentDelta;
} AXPBVE;

typedef struct _AXPBADDR {
    u16 loopFlag;
    u16 format;
    u16 loopAddressHi;
    u16 loopAddressLo;
    u16 endAddressHi;
    u16 endAddressLo;
    u16 currentAddressHi;
    u16 currentAddressLo;
} AXPBADDR;

typedef struct _AXPBADPCM {
    u16 a[8][2];
    u16 gain;
    u16 pred_scale;
    u16 yn1;
    u16 yn2;
} AXPBADPCM;

typedef struct _AXPBSRC {
    u16 ratioHi;
    u16 ratioLo;
    u16 currentAddressFrac;
    u16 last_samples[4];
} AXPBSRC;

typedef struct _AXPBADPCMLOOP {
    u16 loop_pred_scale;
    u16 loop_yn1;
    u16 loop_yn2;
} AXPBADPCMLOOP;

typedef struct _AXPBLPF {
    u16 on;
    u16 yn1;
    u16 a0;
    u16 b0;
} AXPBLPF;

typedef struct _AXPBBIQUAD {
    u16 on;
    u16 xn1;
    u16 xn2;
    u16 yn1;
    u16 yn2;
    u16 b0;
    u16 b1;
    u16 b2;
    u16 a1;
    u16 a2;
} AXPBBIQUAD;

typedef struct _AXPBRMTMIX {
    u16 vMain0;
    u16 vDeltaMain0;
    u16 vAux0;
    u16 vDeltaAux0;
    u16 vMain1;
    u16 vDeltaMain1;
    u16 vAux1;
    u16 vDeltaAux1;
    u16 vMain2;
    u16 vDeltaMain2;
    u16 vAux2;
    u16 vDeltaAux2;
    u16 vMain3;
    u16 vDeltaMain3;
    u16 vAux3;
    u16 vDeltaAux3;
} AXPBRMTMIX;

typedef struct _AXPBRMTDPOP {
    s16 aMain0;
    s16 aMain1;
    s16 aMain2;
    s16 aMain3;
    s16 aAux0;
    s16 aAux1;
    s16 aAux2;
    s16 aAux3;
} AXPBRMTDPOP;

typedef struct _AXPBRMTSRC {
    u16 currentAddressFrac;
    u16 last_samples[4];
} AXPBRMTSRC;

typedef union __AXPBRMTIIR {
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
} AXPBRMTIIR;

typedef struct _AXPB {
    u16 nextHi;
    u16 nextLo;
    u16 currHi;
    u16 currLo;
    u16 srcSelect;
    u16 coefSelect;
    u32 mixerCtrl;
    u16 state;
    u16 type;
    AXPBMIX mix;
    AXPBITD itd;
    AXPBDPOP dpop;
    AXPBVE ve;
    AXPBADDR addr;
    AXPBADPCM adpcm;
    AXPBSRC src;
    AXPBADPCMLOOP adpcmLoop;
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
    u16 remote;
    u16 rmtMixerCtrl;
    AXPBRMTMIX rmtMix;
    AXPBRMTDPOP rmtDpop;
    AXPBRMTSRC rmtSrc;
    AXPBRMTIIR rmtIIR;
    u8 padding[0x140 - 0x128];
} AXPB;

typedef void (*AXVoiceCallback)(void* vpb);

typedef struct _AXVPB {
    void* next;
    void* prev;
    void* next1;
    u32 priority;
    AXVoiceCallback callback;
    u32 userContext;
    u32 index;
    u32 sync;
    u32 depop;
    void* itdBuffer;
    AXPB pb;
} AXVPB;

typedef void (*AXAuxCallback)(void* chans, void* context);

void __AXSetPBDefault(AXVPB* vpb);

#define AX_SAMPLES_PER_FRAME 96
#define AX_SAMPLE_DEPTH_BYTES sizeof(u32)
#define AX_SAMPLES_PER_FRAME_RMT 18
#define AX_FRAME_SIZE (AX_SAMPLES_PER_FRAME * AX_SAMPLE_DEPTH_BYTES)

typedef enum {
    AX_STEREO_L,
    AX_STEREO_R,
    AX_STEREO_S,

    AX_STEREO_MAX
} AXStereoChannel;

typedef enum {
    AX_DPL2_L,
    AX_DPL2_R,
    AX_DPL2_LS,
    AX_DPL2_RS,

    AX_DPL2_MAX
} AXDPL2Channel;

void __AXAuxInit(void);
void __AXGetAuxAInput(void** out);
void __AXGetAuxAOutput(void** out);
void __AXGetAuxAInputDpl2(void** out);
void __AXGetAuxAOutputDpl2R(void** out);
void __AXGetAuxAOutputDpl2Ls(void** out);
void __AXGetAuxAOutputDpl2Rs(void** out);
void __AXGetAuxBInput(void** out);
void __AXGetAuxBOutput(void** out);
void __AXGetAuxBInputDpl2(void** out);
void __AXGetAuxBOutputDpl2R(void** out);
void __AXGetAuxBOutputDpl2Ls(void** out);
void __AXGetAuxBOutputDpl2Rs(void** out);
void __AXGetAuxCInput(void** out);
void __AXGetAuxCOutput(void** out);
void __AXProcessAux(void);
void AXRegisterAuxACallback(AXAuxCallback callback, void* context);
void AXRegisterAuxBCallback(AXAuxCallback callback, void* context);
void AXRegisterAuxCCallback(AXAuxCallback callback, void* context);
void AXGetAuxACallback(AXAuxCallback* callback, void** context);
void AXGetAuxBCallback(AXAuxCallback* callback, void** context);
void AXGetAuxCCallback(AXAuxCallback* callback, void** context);

#define AX_MAX_VOLUME 32768
#define AX_CL_SIZE 128

typedef enum {
    AX_OUTPUT_STEREO,
    AX_OUTPUT_SURROUND,
    AX_OUTPUT_DPL2
} AXOutputMode;

extern const u16 __AXCompressorTable[];

AXPB* __AXGetPBs(void);
u32 __AXGetCommandListCycles(void);
void* __AXGetCommandListAddress(void);
void __AXNextFrame(void* surround, void* lr, void* rmt);
void __AXClInit(void);
void AXSetMode(u32 mode);
u16 AXGetAuxAReturnVolume(void);
u16 AXGetAuxBReturnVolume(void);
u16 AXGetAuxCReturnVolume(void);
void AXSetMasterVolume(u16 volume);
void AXSetAuxAReturnVolume(u16 volume);
void AXSetAuxBReturnVolume(u16 volume);
void AXSetAuxCReturnVolume(u16 volume);

#pragma pack(push, 1)
typedef struct _AXSTUDIO {
    s32 L;
    s16 dL;

    s32 R;
    s16 dR;

    s32 S;
    s16 dS;

    s32 AuxAL;
    s16 dAuxAL;

    s32 AuxAR;
    s16 dAuxAR;

    s32 AuxAS;
    s16 dAuxAS;

    s32 AuxBL;
    s16 dAuxBL;

    s32 AuxBR;
    s16 dAuxBR;

    s32 AuxBS;
    s16 dAuxBS;

    s32 AuxCL;
    s16 dAuxCL;

    s32 AuxCR;
    s16 dAuxCR;

    s32 AuxCS;
    s16 dAuxCS;

    s32 Main0;
    s16 dMain0;

    s32 Aux0;
    s16 dAux0;

    s32 Main1;
    s16 dMain1;

    s32 Aux1;
    s16 dAux1;

    s32 Main2;
    s16 dMain2;

    s32 Aux2;
    s16 dAux2;

    s32 Main3;
    s16 dMain3;

    s32 Aux3;
    s16 dAux3;
} AXSTUDIO;
#pragma pack(pop)

AXSTUDIO* __AXGetStudio(void);
void __AXDepopFadeMain(s32* all, s32* value, s16* depop) __attribute__((noinline));
void __AXDepopFadeRmt(s32* all, s32* value, s16* depop) __attribute__((noinline));
void __AXPrintStudio(void);
void __AXSPBInit(void);
void __AXDepopVoice(AXPB* pb);

#define AX_RMT_MAX 4

typedef void (*AXOutCallback)(void);
typedef void (*AXExceedCallback)(u32 cycles);
typedef struct _AXPROFILE {
    s64 timeBegin;
    s64 timeProcessAuxBegin;
    s64 timeProcessAuxEnd;
    s64 timeUserFrameBegin;
    s64 timeUserFrameEnd;
    s64 timeEnd;
    s32 numVoices;
} AXPROFILE;

extern u16 axDspSlave[];
extern u16 axDspSlaveLength;
extern u16 axDspInitVector;
extern u16 axDspResumeVector;
void __AXSyncPBs(u32 baseCycles);
s32 __AXGetNumVoices(void);
AXPROFILE* __AXGetCurrentProfile(void);
u32 __AXOutNewFrame(void);
void __AXOutAiCallback(void);
void __AXOutInitDSP(void);
void __AXOutInit(u32 mode);
AXOutCallback AXRegisterCallback(AXOutCallback callback);
s32 AXRmtGetSamplesLeft(void);
s32 AXRmtGetSamples(s32 chan, s16* out, s32 num);
s32 AXRmtAdvancePtr(s32 num);

#define AX_PRIORITY_FREE 0
#define AX_PRIORITY_MIN 1
#define AX_PRIORITY_MAX 31
#define AX_VOICE_STOP 0
#define AX_VOICE_RUN 1
#define AX_PB_LPF_ON 1
#define AX_PB_BIQUAD_ON 2
#define AX_SAMPLE_RATE 32000
#define AX_VOICE_MAX 96

AXVPB* __AXGetStackHead(u32 prio);
void __AXServiceCallbackStack(void);
void __AXInitVoiceStacks(void);
void __AXAllocInit(void);
void __AXPushFreeStack(AXVPB* vpb);
AXVPB* __AXPopFreeStack(void);
void __AXPushCallbackStack(AXVPB* vpb);
AXVPB* __AXPopCallbackStack(void);
void __AXRemoveFromStack(AXVPB* vpb);
void __AXPushStackHead(AXVPB* vpb, u32 prio);
AXVPB* __AXPopStackFromBottom(u32 prio);
void AXFreeVoice(AXVPB* vpb);
AXVPB* AXAcquireVoice(u32 prio, AXVoiceCallback callback, u32 userContext);
void AXSetVoicePriority(AXVPB* vpb, u32 prio);

void __AXServiceVPB(AXVPB* vpb);

void __AXVPBInit(void);
void AXSetVoiceState(AXVPB* voice, u16 state);
void AXSetVoiceAddr(AXVPB* voice, AXPBADDR* addr);
void AXGetLpfCoefs(u16 frequency, u16* a, u16* b);
void AXSetMaxDspCycles(u32 cycles);
s32 AXGetMaxVoices(void);

#ifdef __cplusplus
}
#endif

#endif
