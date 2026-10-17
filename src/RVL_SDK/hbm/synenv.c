#include "revolution/hbm/sound/syn.h"

#include <limits.h>
#include <math.h>

#include "revolution/hbm/sound/synprivate.h"

s32 __HBMSYNGetEnvelopeTime(s32 scale, s32 mod, u8 key) {
    if (scale == LONG_MIN) {
        return 0;
    }

    if (mod == LONG_MIN) {
        return powf(2.0f, scale / (65536.0f * 1200.0f)) * 1000.0f;
    }

    return powf(2.0f, (mod * __HBMSYNn128[key] + scale) / (65535.0f * 1200.0f)) * 1000.0f;
}

void __HBMSYNSetupVolumeEnvelope(HBMSYNVOICE* voice) {
    if (voice->art->eg1Attack == LONG_MIN) {
        voice->veState = HBMSYN_VOLUME_ENVELOPE_DECAY;
        voice->veAttn = 0;

        if (voice->art->eg1Decay == LONG_MIN) {
            voice->veState = HBMSYN_VOLUME_ENVELOPE_SUSTAIN;
            voice->veAttn = voice->art->eg1Sustain;
        }
    } else {
        s32 frames = __HBMSYNGetEnvelopeTime(voice->art->eg1Attack, voice->art->eg1Vel2Attack,
                                             voice->keyVel) /
                     3;

        if (frames != 0) {
            voice->veAttack = 0;
            voice->veAttackDelta = (100 << 16) / frames;
            voice->veAttn = -(960 << 16);
            voice->veState = HBMSYN_VOLUME_ENVELOPE_ATTACK;
        } else {
            voice->veAttack = 0;
            voice->veAttackDelta = 100 << 16;
            voice->veAttn = -(960 << 16);
            voice->veState = HBMSYN_VOLUME_ENVELOPE_ATTACK;
        }
    }

    if (voice->veState < HBMSYN_VOLUME_ENVELOPE_SUSTAIN) {
        s32 frames = __HBMSYNGetEnvelopeTime(voice->art->eg1Decay, voice->art->eg1Key2Decay,
                                             voice->keyNum) /
                     3;

        if (frames != 0) {
            voice->veDecay = -(960 << 16) / frames;
        } else {
            voice->veDecay = -(960 << 16);
        }
    }

    voice->veSustain = voice->art->eg1Sustain;
    voice->veRelease = voice->art->eg1Release;
}

void __HBMSYNRunVolumeEnvelope(HBMSYNVOICE* voice) {
    switch (voice->veState) {
    case HBMSYN_VOLUME_ENVELOPE_ATTACK:
        voice->veAttack += voice->veAttackDelta;

        if (voice->veAttack >= (99 << 16)) {
            voice->veAttn = 0;
        } else {
            voice->veAttn = __HBMSYNAttackAttnTable[voice->veAttack >> 16];
        }

        if (voice->veAttn == 0) {
            voice->veState = HBMSYN_VOLUME_ENVELOPE_DECAY;
        }
        break;

    case HBMSYN_VOLUME_ENVELOPE_DECAY:
        voice->veAttn += voice->veDecay;

        if (voice->veAttn <= voice->veSustain) {
            voice->veAttn = voice->veSustain;
            voice->veState = HBMSYN_VOLUME_ENVELOPE_SUSTAIN;
        }

        if (voice->veAttn <= -(720 << 16)) {
            voice->veState = HBMSYN_VOLUME_ENVELOPE_END;
            voice->synth->voice[voice->midiChannel][voice->keyNum] = nullptr;
        }
        break;

    case HBMSYN_VOLUME_ENVELOPE_SUSTAIN:
        break;

    case HBMSYN_VOLUME_ENVELOPE_RELEASE:
        if (voice->veAttn <= -(720 << 16)) {
            voice->veState = HBMSYN_VOLUME_ENVELOPE_END;
        } else {
            voice->veAttn += voice->veRelease;
        }
        break;
    }
}
