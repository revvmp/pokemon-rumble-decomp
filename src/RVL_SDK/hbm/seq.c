#include "revolution/hbm/sound/seq.h"

#include "revolution/ax.h"
#include "revolution/hbm/sound/syn.h"
#include "revolution/os.h"

#define ASSERT(exp) ((void)0)
#define ASSERTMSG(exp, msg) ((void)0)

#define MIDI_HEADER_MAGIC 'MThd'
#define MIDI_TRACK_MAGIC 'MTrk'

enum MidiStatus {
    MIDI_CONTROL_CHANGE = 0xB0,
    MIDI_SYSEX_BEGIN = 0xF0,
    MIDI_SYSEX_END = 0xF7,
    MIDI_META_EVENT = 0xFF,
};

enum MidiMetaEvent {
    MIDI_META_END_OF_TRACK = 0x2F,
    MIDI_META_SET_TEMPO = 0x51,
};

enum MidiController {
    MIDI_CC_ALL_NOTES_OFF = 0x7B,
};

enum HBMSEQTrackState {
    HBMSEQ_TRACK_STATE_STOP,
    HBMSEQ_TRACK_STATE_RUN,
    HBMSEQ_TRACK_STATE_RUNLOOPED,
};

static u8 __HBMSEQMidiEventLength[128] = {
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static HBMSEQSEQUENCE* __HBMSEQSequenceList;
static BOOL __init;

static void __HBMSEQPushSequenceList(HBMSEQSEQUENCE* sequence) {
    BOOL enabled = OSDisableInterrupts();

    if (__HBMSEQSequenceList != nullptr) {
        sequence->next = __HBMSEQSequenceList;
    } else {
        sequence->next = nullptr;
    }

    __HBMSEQSequenceList = sequence;

    OSRestoreInterrupts(enabled);
}

static void __HBMSEQRemoveSequenceFromList(HBMSEQSEQUENCE* sequence) {
    BOOL enabled = OSDisableInterrupts();
    HBMSEQSEQUENCE* it = __HBMSEQSequenceList;

    __HBMSEQSequenceList = nullptr;

    while (it != nullptr) {
        HBMSEQSEQUENCE* next = it->next;

        if (it != sequence) {
            __HBMSEQPushSequenceList(it);
        }

        it = next;
    }

    OSRestoreInterrupts(enabled);
}

static u32 __HBMSEQGetIntTrack(HBMSEQTRACK* track) {
    u32 value;

    ASSERT(track);

    value = *track->current & 0x7F;

    while (*track->current & 0x80) {
        track->current++;
        value = (value << 7) + (*track->current & 0x7F);
    }

    track->current++;
    return value;
}

static void __HBMSEQHandleSysExEvent(HBMSEQTRACK* track) {
    u32 length;

    ASSERT(track);

    length = __HBMSEQGetIntTrack(track);
    track->current += length;
}

static void __HBMSEQSetTicksPerFrame(HBMSEQTRACK* track, f32 bps) {
    HBMSEQSEQUENCE* sequence;

    ASSERT(track);

    sequence = track->sequence;

    track->beatsPerSec = bps;
    track->ticksPerFrame =
        AX_SAMPLES_PER_FRAME / ((f32)AX_SAMPLE_RATE / bps / sequence->timeFormat) * 65536.0f;
}

static void __HBMSEQTempoMetaEvent(HBMSEQTRACK* track) {
    u32 data;
    f32 beatsPerSec;

    data = *track->current;
    track->current++;
    data = (data << 8) + *track->current;
    track->current++;
    data = (data << 8) + *track->current;
    track->current++;

    beatsPerSec = 1000000.0f / data;

    __HBMSEQSetTicksPerFrame(track, beatsPerSec);
}

static void __HBMSEQTrackEnd(HBMSEQTRACK* track) {
    HBMSEQSEQUENCE* sequence;

    ASSERT(track);

    sequence = track->sequence;

    sequence->tracksRunning--;
    track->state = HBMSEQ_TRACK_STATE_STOP;

    if (sequence->tracksRunning == 0) {
        sequence->end = TRUE;
    }
}

static void __HBMSEQHandleMetaEvent(HBMSEQTRACK* track) {
    u8 type;
    u32 length;

    ASSERT(track);

    type = *track->current;
    track->current++;

    switch (type) {
    case MIDI_META_END_OF_TRACK:
        __HBMSEQTrackEnd(track);
        break;

    case MIDI_META_SET_TEMPO:
        length = __HBMSEQGetIntTrack(track);
        __HBMSEQTempoMetaEvent(track);
        break;

    default:
        length = __HBMSEQGetIntTrack(track);
        track->current += length;
        break;
    }
}

static void __HBMSEQHandleSynthEvent(HBMSYNSYNTH* synth, HBMSEQTRACK* track) {
    u8 ch[3];
    u32 bytes;

    bytes = __HBMSEQMidiEventLength[track->status - 0x80];
    ch[0] = track->status;

    switch (bytes) {
    case 0:
        break;

    case 1:
        ch[1] = *track->current;
        track->current++;
        break;

    case 2:
        ch[1] = *track->current;
        track->current++;
        ch[2] = *track->current;
        track->current++;
        break;
    }

    HBMSYNMidiInput(synth, ch);
}

static void __HBMSEQRunEvent(HBMSYNSYNTH* synth, HBMSEQTRACK* track) {
    u8 event;

    ASSERT(synth);
    ASSERT(track);

    event = *track->current;

    if (event >= 0x80) {
        track->status = event;
        track->current++;
    }

    switch (track->status) {
    case MIDI_SYSEX_BEGIN:
    case MIDI_SYSEX_END:
        __HBMSEQHandleSysExEvent(track);
        break;

    case MIDI_META_EVENT:
        __HBMSEQHandleMetaEvent(track);
        break;

    default:
        __HBMSEQHandleSynthEvent(synth, track);
        break;
    }

    if (track->current >= track->end) {
        __HBMSEQTrackEnd(track);
    }
}

static void __HBMSEQInitTracks(HBMSEQSEQUENCE* sequence, u8* read, int tracks) {
    int i = 0;
    u8* p = read;

    for (; tracks != 0; tracks--, i++) {
        u32 chunk;
        u32 bytes;

    nextChunk:
        chunk = *(u32*)p;
        p += sizeof(u32);
        bytes = *(u32*)p;
        p += sizeof(u32);

        if (chunk == MIDI_TRACK_MAGIC) {
            HBMSEQTRACK* track = sequence->track + i;

            track->sequence = sequence;
            track->start = p;
            track->end = p + bytes;
            track->current = p;
            track->defaultTicksPerFrame =
                AX_SAMPLES_PER_FRAME / (16000.0f / sequence->timeFormat) * 65536.0f;
            track->state = HBMSEQ_TRACK_STATE_STOP;

            p += bytes;
        } else {
            p += bytes;
            goto nextChunk;
        }
    }
}

static void __HBMSEQReadHeader(HBMSEQSEQUENCE* sequence, u8* midiStream) {
    u8* read = midiStream;
    u32 bytes;
    u32 fileType;

    ASSERTMSG(*(u32*)read == MIDI_HEADER_MAGIC, "!!!midiStream is not a valid MIDI file
!!!");
    read += sizeof(u32);

    bytes = *(u32*)read;
    read += sizeof(u32);
    fileType = *(u16*)read;
    read += sizeof(u16);
    sequence->nTracks = *(u16*)read;
    read += sizeof(u16);
    sequence->timeFormat = *(s16*)read;
    read += sizeof(s16);

    ASSERTMSG(sequence->timeFormat >= 0, "!!!HBMSEQ does not support SMPTE time!!!
");

    bytes -= sizeof(u16) + sizeof(u16) + sizeof(s16);
    read += bytes;

    switch (fileType) {
    case 0:
        sequence->nTracks = 1;
        __HBMSEQInitTracks(sequence, read, 1);
        break;

    case 1:
        ASSERTMSG(sequence->nTracks < HBMSEQ_MAX_TRACKS,
                  "exceeded HBMSEQ_MAX_TRACKS, please increase HBMSEQ_MAX_TRACKS
");
        __HBMSEQInitTracks(sequence, read, sequence->nTracks);
        break;

    default:
        ASSERTMSG(FALSE, "!!!Invalid MIDI file type
!!!");
        break;
    }

    sequence->tracksRunning = sequence->nTracks;
}

void HBMSEQInit(void) {
    if (!__init) {
        __HBMSEQSequenceList = nullptr;
        __init = TRUE;
    }
}

void HBMSEQQuit(void) {
    __HBMSEQSequenceList = nullptr;
    __init = FALSE;
}

void HBMSEQRunAudioFrame(void) {
    HBMSEQSEQUENCE* sequence = __HBMSEQSequenceList;

    if (!__init) {
        return;
    }

    for (; sequence != nullptr; sequence = sequence->next) {
        if (sequence->state == HBMSEQ_STATE_RUN || sequence->state == HBMSEQ_STATE_RUNLOOPED) {
            for (u32 i = 0; i < sequence->nTracks; i++) {
                HBMSEQTRACK* track = &sequence->track[i];

                if (track->state == HBMSEQ_TRACK_STATE_RUN ||
                    track->state == HBMSEQ_TRACK_STATE_RUNLOOPED) {
                    u32 ticks = track->ticksPerFrame;

                    if (track->delay > ticks) {
                        track->delay -= ticks;
                        continue;
                    }

                    while (ticks >= track->delay) {
                        ticks -= track->delay;

                        __HBMSEQRunEvent(&sequence->synth, track);

                        if (track->state == HBMSEQ_TRACK_STATE_STOP) {
                            break;
                        }

                        track->delay = __HBMSEQGetIntTrack(track) << 16;
                    }

                    track->delay -= ticks;
                }
            }
        }

        if (sequence->end) {
            if (sequence->state == HBMSEQ_STATE_RUNLOOPED) {
                HBMSEQSetState(sequence, HBMSEQ_STATE_STOP);
                HBMSEQSetState(sequence, HBMSEQ_STATE_RUNLOOPED);
            } else {
                HBMSEQSetState(sequence, HBMSEQ_STATE_STOP);
            }
        }
    }
}

void HBMSEQAddSequence(HBMSEQSEQUENCE* sequence, u8* midiStream, u8* wavetable, u8* samples,
                       u8* zerobuffer) {
    ASSERT(sequence);
    ASSERT(midiStream);
    ASSERT(wavetable);
    ASSERT(samples);

    HBMSYNInitSynth(&sequence->synth, wavetable, samples, zerobuffer);
    sequence->state = HBMSEQ_STATE_STOP;
    __HBMSEQReadHeader(sequence, midiStream);
    __HBMSEQPushSequenceList(sequence);
}

void HBMSEQRemoveSequence(HBMSEQSEQUENCE* sequence) {
    ASSERT(sequence);

    __HBMSEQRemoveSequenceFromList(sequence);
    HBMSYNQuitSynth(&sequence->synth);
}

void HBMSEQSetState(HBMSEQSEQUENCE* sequence, HBMSEQSTATE state) {
    int i;

    ASSERT(sequence);

    switch (state) {
    case HBMSEQ_STATE_RUN:
    case HBMSEQ_STATE_RUNLOOPED:
        if (sequence->state == HBMSEQ_STATE_STOP) {
            BOOL enabled = OSDisableInterrupts();

            for (i = 0; i < sequence->nTracks; i++) {
                HBMSEQTRACK* track = &sequence->track[i];

                track->current = track->start;
                track->ticksPerFrame = track->defaultTicksPerFrame;
                track->delay = __HBMSEQGetIntTrack(track) << 16;
                track->state = HBMSEQ_TRACK_STATE_RUN;
            }

            sequence->tracksRunning = sequence->nTracks;

            OSRestoreInterrupts(enabled);
        }

        sequence->end = FALSE;
        break;

    case HBMSEQ_STATE_STOP:
    case HBMSEQ_STATE_PAUSE:
        for (i = 0; i < HBMSYN_NUM_MIDI_CHANNELS; i++) {
            BOOL enabled = OSDisableInterrupts();
            u8 ch[3];

            ch[0] = MIDI_CONTROL_CHANGE | i;
            ch[1] = MIDI_CC_ALL_NOTES_OFF;
            ch[2] = 0;

            HBMSYNMidiInput(&sequence->synth, ch);

            OSRestoreInterrupts(enabled);
        }
        break;
    }

    sequence->state = state;
}

HBMSEQSTATE HBMSEQGetState(HBMSEQSEQUENCE* sequence) {
    ASSERT(sequence);

    return sequence->state;
}

void HBMSEQSetVolume(HBMSEQSEQUENCE* sequence, s32 dB) {
    ASSERT(sequence);

    HBMSYNSetMasterVolume(&sequence->synth, dB);
}
