#include "revolution/hbm/sound/syn.h"

#include "macros.h"
#include "revolution/ax.h"
#include "revolution/ax/AX.h"
#include "revolution/hbm/sound/mix.h"
#include "revolution/hbm/sound/synprivate.h"
#include "revolution/os.h"
#include "revolution/os/OSAddress.h"

static HBMSYNSYNTH* __HBMSYNSynthList;
HBMSYNVOICE* __HBMSYNVoice;
static HBMSYNVOICE __s_HBMSYNVoice[16];
static BOOL __init;

static void __HBMSYNAddSynthToList(HBMSYNSYNTH* synth) {
    BOOL enabled = OSDisableInterrupts();

    if (__HBMSYNSynthList != nullptr) {
        synth->next = __HBMSYNSynthList;
    } else {
        synth->next = nullptr;
    }

    __HBMSYNSynthList = synth;

    OSRestoreInterrupts(enabled);
}

static void __HBMSYNRemoveSynthFromList(HBMSYNSYNTH* synth) {
    HBMSYNSYNTH* head;
    HBMSYNSYNTH* tail;
    HBMSYNSYNTH* it;

    BOOL enabled = OSDisableInterrupts();

    head = nullptr;
    tail = nullptr;

    for (it = __HBMSYNSynthList; it != nullptr; it = it->next) {
        if (it != synth) {
            if (head != nullptr) {
                tail->next = it;
                tail = it;
            } else {
                tail = it;
                head = tail;
            }
        }
    }

    if (tail != nullptr) {
        tail->next = nullptr;
    }

    __HBMSYNSynthList = head;

    OSRestoreInterrupts(enabled);
}

void HBMSYNInit(void) {
    int i;

    if (AXIsInit() && !__init) {
        __HBMSYNVoice = __s_HBMSYNVoice;

        for (i = 0; i < (int)ARRAY_LENGTH(__s_HBMSYNVoice); i++) {
            __HBMSYNVoice[i].synth = nullptr;
        }

        __HBMSYNSynthList = nullptr;
        __init = TRUE;
    }
}

void HBMSYNQuit(void) {
    __HBMSYNVoice = nullptr;
    __init = FALSE;
}

void HBMSYNRunAudioFrame(void) {
    int i;
    HBMSYNSYNTH* synth;

    if (!__init) {
        return;
    }

    for (i = 0; i < (int)ARRAY_LENGTH(__s_HBMSYNVoice); i++) {
        __HBMSYNServiceVoice(i);
    }

    for (synth = __HBMSYNSynthList; synth != nullptr; synth = synth->next) {
        __HBMSYNRunInputBufferEvents(synth);
    }
}

void HBMSYNInitSynth(HBMSYNSYNTH* synth, u8* wavetable, u8* samples, u8* zerobuffer) {
    u32* p;
    u32 mramBase;
    u32 midiChannel;
    u32 noteNumber;

    p = (u32*)wavetable;
    mramBase = OSCachedToPhysical(samples);

    synth->percussiveInst = (WTINST*)(wavetable + *p);
    p++;
    synth->melodicInst = (WTINST*)(wavetable + *p);
    p++;
    synth->region = (WTREGION*)(wavetable + *p);
    p++;
    synth->art = (WTART*)(wavetable + *p);
    p++;
    synth->sample = (WTSAMPLE*)(wavetable + *p);
    p++;
    synth->adpcm = (WTADPCM*)(wavetable + *p);
    p++;

    synth->samplesBaseWord = mramBase >> 1;
    synth->samplesBaseByte = mramBase;
    synth->samplesBaseNibble = mramBase << 1;
    synth->masterVolume = 0;

    __HBMSYNResetAllControllers(synth);

    synth->inputPosition = *synth->input;
    synth->inputCounter = 0;
    synth->notes = 0;

    for (midiChannel = 0; midiChannel < HBMSYN_NUM_MIDI_CHANNELS; midiChannel++) {
        for (noteNumber = 0; noteNumber < HBMSYN_NUM_MIDI_NOTES; noteNumber++) {
            synth->voice[midiChannel][noteNumber] = nullptr;
        }
    }

    __HBMSYNAddSynthToList(synth);
}

void HBMSYNQuitSynth(HBMSYNSYNTH* synth) {
    int i;
    BOOL enabled = OSDisableInterrupts();

    if (synth->notes != 0) {
        for (i = 0; i < (int)ARRAY_LENGTH(__s_HBMSYNVoice); i++) {
            HBMSYNVOICE* voice = __HBMSYNVoice + i;

            if (voice->synth == synth) {
                HBMMIXReleaseChannel(voice->axvpb);
                HBMFreeIndexByKey(voice->axvpb->index);
                AXFreeVoice(voice->axvpb);
                voice->synth = nullptr;
            }
        }
    }

    __HBMSYNRemoveSynthFromList(synth);

    OSRestoreInterrupts(enabled);
}

void HBMSYNMidiInput(HBMSYNSYNTH* synth, u8* input) {
    u8* src = input;

    *synth->inputPosition = *src;
    synth->inputPosition++;
    src++;
    *synth->inputPosition = *src;
    synth->inputPosition++;
    src++;
    *synth->inputPosition = *src;
    synth->inputPosition++;
    src++;

    synth->inputCounter++;
}

void HBMSYNSetMasterVolume(HBMSYNSYNTH* synth, s32 dB) {
    synth->masterVolume = dB << 16;
}
