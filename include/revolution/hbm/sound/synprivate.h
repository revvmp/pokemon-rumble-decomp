#ifndef HBM_SOUND_SYNPRIVATE_H
#define HBM_SOUND_SYNPRIVATE_H

#include "revolution/ax.h"
#include "revolution/types.h"

#include "revolution/hbm/sound/syn.h"

#ifdef __cplusplus
extern "C" {
#endif

enum HBMSYNVolumeEnvelopeState {
    HBMSYN_VOLUME_ENVELOPE_ATTACK,
    HBMSYN_VOLUME_ENVELOPE_DECAY,
    HBMSYN_VOLUME_ENVELOPE_SUSTAIN,
    HBMSYN_VOLUME_ENVELOPE_RELEASE,
    HBMSYN_VOLUME_ENVELOPE_END,
};

struct HBMSYNVOICE {
    void* next;
    AXVPB* axvpb;
    HBMSYNSYNTH* synth;
    u8 midiChannel;
    u8 keyNum;
    u8 keyVel;
    u8 pan;
    WTREGION* region;
    WTART* art;
    WTSAMPLE* sample;
    WTADPCM* adpcm;
    u32 type;
    f32 srcRatio;
    s32 cents;
    s32 attn;
    u32 veState;
    s32 veAttn;
    s32 veAttack;
    s32 veAttackDelta;
    s32 veDecay;
    s32 veSustain;
    s32 veRelease;
};

extern HBMSYNVOICE* __HBMSYNVoice;

extern f32 __HBMSYNn128[];

extern s32 __HBMSYNVolumeAttenuation[];
extern s32 __HBMSYNAttackAttnTable[];

void __HBMSYNSetController(HBMSYNSYNTH* synth, u8 midiChannel, u8 function, u8 value);
void __HBMSYNResetAllControllers(HBMSYNSYNTH* synth);
void __HBMSYNRunInputBufferEvents(HBMSYNSYNTH* synth);
BOOL __HBMSYNGetWavetableData(HBMSYNVOICE* voice);

s32 __HBMSYNGetEnvelopeTime(s32 scale, s32 mod, u8 key);
void __HBMSYNSetupVolumeEnvelope(HBMSYNVOICE* voice);
void __HBMSYNRunVolumeEnvelope(HBMSYNVOICE* voice);

void __HBMSYNSetupVolume(HBMSYNVOICE* voice);
void __HBMSYNSetupPan(HBMSYNVOICE* voice);
s32 __HBMSYNGetVoiceInput(HBMSYNVOICE* voice);
s32 __HBMSYNGetVoiceFader(HBMSYNVOICE* voice);
void __HBMSYNUpdateMix(HBMSYNVOICE* voice);

f32 __HBMSYNGetRelativePitch(HBMSYNVOICE* voice);
void __HBMSYNSetupPitch(HBMSYNVOICE* voice);
void __HBMSYNSetupSrc(HBMSYNVOICE* voice);
void __HBMSYNUpdateSrc(HBMSYNVOICE* voice);

void __HBMSYNSetupSample(HBMSYNVOICE* voice);

void __HBMSYNClearVoiceReferences(void* p);
void __HBMSYNSetVoiceToRelease(HBMSYNVOICE* voice);
void __HBMSYNServiceVoice(int i);

#ifdef __cplusplus
}
#endif

#endif
