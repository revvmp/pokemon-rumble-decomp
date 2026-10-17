#include <nw4r/snd/snd_RemoteSpeakerManager.h>

#include <revolution/ax.h>

namespace nw4r {
namespace snd {
namespace detail {

RemoteSpeakerManager& RemoteSpeakerManager::GetInstance() {
    static RemoteSpeakerManager instance;
    return instance;
}

RemoteSpeakerManager::RemoteSpeakerManager() : mInitialized(false) {
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mSpeaker[i].SetChannelIndex(i);
    }
}

RemoteSpeaker& RemoteSpeakerManager::GetRemoteSpeaker(int idx) {
    return mSpeaker[idx];
}

void RemoteSpeakerManager::Setup() {
    if (mInitialized) {
        return;
    }

    OSCreateAlarm(&mRemoteSpeakerAlarm);
    OSSetPeriodicAlarm(&mRemoteSpeakerAlarm, OSGetTime(),
                       OS_NSEC_TO_TICKS(SPEAKER_ALARM_PERIOD_NSEC),
                       RemoteSpeakerAlarmProc);

    mInitialized = true;
}

void RemoteSpeakerManager::Shutdown() {
    if (!mInitialized) {
        return;
    }

    OSCancelAlarm(&mRemoteSpeakerAlarm);
    mInitialized = false;
}

void RemoteSpeakerManager::RemoteSpeakerAlarmProc(OSAlarm* pAlarm,
                                                  OSContext* pCtx) {
    RemoteSpeakerManager& rManager = GetInstance();
    s16 samples[RemoteSpeaker::SAMPLES_PER_AUDIO_PACKET];

    if (AXRmtGetSamplesLeft() < RemoteSpeaker::SAMPLES_PER_AUDIO_PACKET) {
        return;
    }

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        if (rManager.mSpeaker[i].IsAvailable()) {
            AXRmtGetSamples(i, samples, RemoteSpeaker::SAMPLES_PER_AUDIO_PACKET);
            rManager.mSpeaker[i].UpdateStreamData(samples);
        }

        rManager.mSpeaker[i].Update();
    }

    AXRmtAdvancePtr(RemoteSpeaker::SAMPLES_PER_AUDIO_PACKET);
}

}
}
}
