#include "revolution/hbm/sound/syn.h"

#include "revolution/ax.h"
#include "revolution/hbm/sound/synprivate.h"

enum HBMSYNSampleFormat {
    HBMSYN_SAMPLE_FORMAT_ADPCM,
    HBMSYN_SAMPLE_FORMAT_PCM16,
    HBMSYN_SAMPLE_FORMAT_PCM8,
};

enum HBMSYNVoiceType {
    HBMSYN_VOICE_TYPE_ONESHOT,
    HBMSYN_VOICE_TYPE_LOOPED,
};

static u32 __HBMSYNGetNibbleAddress(u32 count) {
    u32 frames = count / AX_ADPCM_SAMPLES_PER_FRAME;
    u32 samplesLeft = count % AX_ADPCM_SAMPLES_PER_FRAME;

    return 2 + frames * AX_ADPCM_NIBBLES_PER_FRAME + samplesLeft;
}

static void __HBMSYNSetupAdpcm(HBMSYNVOICE* voice) {
    AXVPB* axvpb = voice->axvpb;

    if (voice->region->loopStart + voice->region->loopLength != 0) {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;
        u32* adpcm;
        u16* adpcmLoop;

        adpcm = (u32*)voice->adpcm;
        voice->type = HBMSYN_VOICE_TYPE_LOOPED;

        sampleStart = voice->synth->samplesBaseNibble + voice->sample->offset;
        sampleLoop = sampleStart + __HBMSYNGetNibbleAddress(voice->region->loopStart);
        sampleEnd = sampleStart + __HBMSYNGetNibbleAddress(voice->region->loopStart +
                                                           voice->region->loopLength - 1);

        sampleStart += 2;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x00010000;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p = *adpcm;

        adpcmLoop = (u16*)++adpcm;

        axvpb->pb.adpcmLoop.loop_pred_scale = *adpcmLoop++;
        axvpb->pb.adpcmLoop.loop_yn1 = *adpcmLoop++;
        axvpb->pb.adpcmLoop.loop_yn2 = *adpcmLoop;

        axvpb->sync &= ~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR | AX_PBSYNC_END_ADDR |
                         AX_PBSYNC_CURR_ADDR);
        axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM | AX_PBSYNC_ADPCM_LOOP;
    } else {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;
        u32* adpcm;

        adpcm = (u32*)voice->adpcm;
        voice->type = HBMSYN_VOICE_TYPE_ONESHOT;

        sampleStart = voice->synth->samplesBaseNibble + voice->sample->offset;
        sampleLoop = voice->synth->samplesBaseNibble + voice->sample->offset;
        sampleEnd = sampleStart + __HBMSYNGetNibbleAddress(voice->sample->length - 1);

        sampleStart += 2;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x00000000;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p++ = *adpcm++;
        *p = *adpcm;

        axvpb->sync &= ~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR | AX_PBSYNC_END_ADDR |
                         AX_PBSYNC_CURR_ADDR);
        axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
    }
}

static void __HBMSYNSetupPcm16(HBMSYNVOICE* voice) {
    AXVPB* axvpb = voice->axvpb;

    if (voice->region->loopStart + voice->region->loopLength != 0) {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;

        voice->type = HBMSYN_VOICE_TYPE_LOOPED;

        sampleStart = voice->synth->samplesBaseWord + voice->sample->offset;
        sampleLoop = sampleStart + voice->region->loopStart;
        sampleEnd = sampleLoop + voice->region->loopLength - 1;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x0001000A;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x08000000;
        *p = 0x00000000;
    } else {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;

        voice->type = HBMSYN_VOICE_TYPE_ONESHOT;

        sampleStart = voice->synth->samplesBaseWord + voice->sample->offset;
        sampleLoop = voice->synth->samplesBaseWord + voice->sample->offset;
        sampleEnd = sampleStart + voice->sample->length - 1;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x0000000A;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x08000000;
        *p = 0x00000000;
    }

    axvpb->sync &= ~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR | AX_PBSYNC_END_ADDR |
                     AX_PBSYNC_CURR_ADDR);
    axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
}

static void __HBMSYNSetupPcm8(HBMSYNVOICE* voice) {
    AXVPB* axvpb = voice->axvpb;

    if (voice->region->loopStart + voice->region->loopLength != 0) {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;

        voice->type = HBMSYN_VOICE_TYPE_LOOPED;

        sampleStart = voice->synth->samplesBaseByte + voice->sample->offset;
        sampleLoop = sampleStart + voice->region->loopStart;
        sampleEnd = sampleLoop + voice->region->loopLength - 1;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x00010019;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01000000;
        *p = 0x00000000;
    } else {
        u32 sampleStart;
        u32 sampleLoop;
        u32 sampleEnd;
        u32* p;

        voice->type = HBMSYN_VOICE_TYPE_ONESHOT;

        sampleStart = voice->synth->samplesBaseByte + voice->sample->offset;
        sampleLoop = voice->synth->samplesBaseByte + voice->sample->offset;
        sampleEnd = sampleStart + voice->sample->length - 1;
        p = (u32*)&axvpb->pb.addr;

        *p++ = 0x00000019;
        *p++ = sampleLoop;
        *p++ = sampleEnd;
        *p++ = sampleStart;

        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01000000;
        *p = 0x00000000;
    }

    axvpb->sync &= ~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR | AX_PBSYNC_END_ADDR |
                     AX_PBSYNC_CURR_ADDR);
    axvpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;
}

void __HBMSYNSetupSample(HBMSYNVOICE* voice) {
    switch (voice->sample->format) {
    case HBMSYN_SAMPLE_FORMAT_ADPCM:
        __HBMSYNSetupAdpcm(voice);
        break;

    case HBMSYN_SAMPLE_FORMAT_PCM16:
        __HBMSYNSetupPcm16(voice);
        break;

    case HBMSYN_SAMPLE_FORMAT_PCM8:
        __HBMSYNSetupPcm8(voice);
        break;
    }
}
