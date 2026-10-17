#ifndef HBM_SOUND_SEQ_H
#define HBM_SOUND_SEQ_H

#include "revolution/types.h"

#include "revolution/hbm/sound/syn.h"

#define HBMSEQ_MAX_TRACKS 64

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 HBMSEQSTATE;
enum HBMSEQSTATE_et {
    HBMSEQ_STATE_STOP,
    HBMSEQ_STATE_RUN,
    HBMSEQ_STATE_RUNLOOPED,
    HBMSEQ_STATE_PAUSE,
};

typedef struct _HBMSEQSEQUENCE HBMSEQSEQUENCE;

typedef struct _HBMSEQTRACK {
    HBMSEQSEQUENCE* sequence;
    u8* start;
    u8* end;
    u8* current;
    u8 status;
    f32 beatsPerSec;
    u32 defaultTicksPerFrame;
    u32 ticksPerFrame;
    u32 delay;
    u32 state;
} HBMSEQTRACK;

struct _HBMSEQSEQUENCE {
    HBMSEQSEQUENCE* next;
    HBMSEQSTATE state;
    u16 nTracks;
    s16 timeFormat;
    u32 tracksRunning;
    u32 end;
    HBMSYNSYNTH synth;
    HBMSEQTRACK track[HBMSEQ_MAX_TRACKS];
};

void HBMSEQInit(void);
void HBMSEQQuit(void);

void HBMSEQRunAudioFrame(void);

void HBMSEQAddSequence(HBMSEQSEQUENCE* sequence, u8* midiStream, u8* wavetable, u8* samples,
                       u8* zerobuffer);
void HBMSEQRemoveSequence(HBMSEQSEQUENCE* sequence);

void HBMSEQSetState(HBMSEQSEQUENCE* sequence, HBMSEQSTATE state);
HBMSEQSTATE HBMSEQGetState(HBMSEQSEQUENCE* sequence);

void HBMSEQSetVolume(HBMSEQSEQUENCE* sequence, s32 dB);
s32 HBMSEQGetVolume(HBMSEQSEQUENCE* sequence);

#ifdef __cplusplus
}
#endif

#endif
