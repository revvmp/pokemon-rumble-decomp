#ifndef NW4R_SND_REMOTE_SPEAKER_H
#define NW4R_SND_REMOTE_SPEAKER_H

#include <revolution/os.h>
#include <revolution/wenc.h>
#include <revolution/wpad.h>

namespace nw4r {
namespace snd {
class RemoteSpeaker {
public:
    enum SpeakerState {
        STATE_INVALID,
        STATE_EXEC_SPEAKER_ON,
        STATE_SPEAKER_ON,
        STATE_EXEC_SPEAKER_PLAY,
        STATE_SPEAKER_PLAY,
        STATE_EXEC_SPEAKER_OFF,
        STATE_SPEAKER_OFF
    };

    enum SpeakerCommand {
        COMMAND_NONE,
        COMMAND_SPEAKER_ON,
        COMMAND_SPEAKER_PLAY,
        COMMAND_SPEAKER_OFF
    };

    static const int SAMPLES_PER_AUDIO_PACKET = 40;

public:
    RemoteSpeaker();

    void InitParam();
    bool Setup(WPADCallback pCallback);
    void Shutdown(WPADCallback pCallback);

    bool EnableOutput(bool enable);
    bool IsEnabledOutput() const;

    void Update();
    void UpdateStreamData(const s16 *pRmtSamples);

    bool IsAvailable() const {
        return mState == STATE_SPEAKER_PLAY;
    }

    void SetChannelIndex(int index) {
        mChannelIndex = index;
    }

private:
    static const int SAMPLES_PER_ENCODED_PACKET = (SAMPLES_PER_AUDIO_PACKET + 1) / 2;

    static const int CONTINUOUS_PLAY_INTERVAL_MINUTES = 8;

private:
    void ClearParam();
    void ExecCommand(SpeakerCommand command);

    bool IsAllSampleZero(const s16 *pRmtSamples);
    void NotifyCallback(s32 chan, s32 result);

    static void SpeakerOnCallback(s32 chan, s32 result);
    static void SpeakerPlayCallback(s32 chan, s32 result);
    static void SpeakerOffCallback(s32 chan, s32 result);

    static void ContinueAlarmHandler(OSAlarm *pAlarm, OSContext *pCtx);
    static void IntervalAlarmHandler(OSAlarm *pAlarm, OSContext *pCtx);

private:
    bool mInitFlag;
    bool mPlayFlag;
    bool mEnableFlag;
    bool mFirstEncodeFlag;
    bool mValidCallbackFlag;
    bool mCommandBusyFlag;

    bool mContinueFlag;
    volatile bool mIntervalFlag;
    SpeakerState mState;
    SpeakerCommand mUserCommand;
    SpeakerCommand mInternalCommand;
    WENCInfo mEncodeInfo;
    int mChannelIndex;
    WPADCallback mWpadCallback;
    OSAlarm mContinueAlarm;
    OSAlarm mIntervalAlarm;
    s64 mContinueBeginTime;
};

}
}

#endif
