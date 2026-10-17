#include "revolution/hbm/sound/syn.h"

#include "revolution/ax.h"
#include "revolution/hbm/sound/mix.h"
#include "revolution/hbm/sound/synprivate.h"

void __HBMSYNClearVoiceReferences(void* p) {
    AXVPB* axvpb;
    HBMSYNSYNTH* synth;
    HBMSYNVOICE* voice;
    int index;

    axvpb = (AXVPB*)p;
    synth = (HBMSYNSYNTH*)axvpb->userContext;

    index = HBMGetIndex(axvpb->index);
    HBMFreeIndex(index);

    voice = __HBMSYNVoice + index;
    HBMMIXReleaseChannel(axvpb);

    if (synth->voice[voice->midiChannel][voice->keyNum] == voice) {
        synth->voice[voice->midiChannel][voice->keyNum] = nullptr;
    }

    voice->synth = nullptr;
    synth->notes--;
}

void __HBMSYNSetVoiceToRelease(HBMSYNVOICE* voice) {
    voice->veState = HBMSYN_VOLUME_ENVELOPE_RELEASE;
}

void __HBMSYNServiceVoice(int i) {
    HBMSYNVOICE* voice = __HBMSYNVoice + i;
    HBMSYNSYNTH* synth = voice->synth;

    if (synth == nullptr) {
        return;
    }

    if (voice->type == 0 && voice->axvpb->pb.state == AX_VOICE_STOP) {
        if (synth->voice[voice->midiChannel][voice->keyNum] == voice) {
            synth->voice[voice->midiChannel][voice->keyNum] = nullptr;
        }

        voice->veState = HBMSYN_VOLUME_ENVELOPE_END;
    }

    __HBMSYNRunVolumeEnvelope(voice);

    if (voice->veState == HBMSYN_VOLUME_ENVELOPE_END) {
        voice->synth = nullptr;

        HBMMIXReleaseChannel(voice->axvpb);
        HBMFreeIndexByKey(voice->axvpb->index);
        AXFreeVoice(voice->axvpb);

        synth->notes--;
    } else {
        __HBMSYNUpdateMix(voice);
        __HBMSYNUpdateSrc(voice);
    }
}
