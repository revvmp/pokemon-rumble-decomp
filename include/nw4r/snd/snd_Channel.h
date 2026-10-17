#ifndef NW4R_SND_CHANNEL_H
#define NW4R_SND_CHANNEL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_EnvGenerator.h>
#include <nw4r/snd/snd_Lfo.h>
#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_Types.h>
#include <nw4r/snd/snd_Voice.h>

#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace snd {
namespace detail {

struct WaveInfo;
class WaveDataLocationCallback;

class Channel {
public:
    enum ChannelCallbackStatus {
        CALLBACK_STATUS_STOPPED,
        CALLBACK_STATUS_DROP,
        CALLBACK_STATUS_FINISH,
        CALLBACK_STATUS_CANCEL
    };

    typedef void (*ChannelCallback)(Channel* pDropChannel,
                                    ChannelCallbackStatus status,
                                    u32 callbackArg);

    enum LfoTarget { LFO_TARGET_PITCH, LFO_TARGET_VOLUME, LFO_TARGET_PAN };

public:
    Channel();
    ~Channel();

    void InitParam(ChannelCallback pCallback, u32 callbackArg);
    void Update(bool periodic);
    void Start(const WaveInfo& rInfo, int length, u32 offset);
    void Release();
    void NoteOff();
    void Stop();

    void SetAttack(int attack) {
        mEnvelope.SetAttack(attack);
    }
    void SetHold(int hold) {
        mEnvelope.SetHold(hold);
    }
    void SetDecay(int decay) {
        mEnvelope.SetDecay(decay);
    }
    void SetSustain(int sustain) {
        mEnvelope.SetSustain(sustain);
    }
    void SetRelease(int release) {
        mEnvelope.SetRelease(release);
    }
    bool IsRelease() const {
        return mEnvelope.GetStatus() == EnvGenerator::STATUS_RELEASE;
    }

    void SetLfoParam(const LfoParam& rParam) {
        mLfo.SetParam(rParam);
    }
    void SetLfoTarget(LfoTarget target) {
        mLfoTarget = target;
    }

    void Pause(bool pause) {
        mPauseFlag = pause;
        mVoice->Pause(pause);
    }
    void SetReleasePriorityFix(bool fix) {
        mReleasePriorityFixFlag = fix;
    }
    void SetReleaseIgnore(bool ignore) {
        mReleaseIgnoreFlag = ignore;
    }

    bool IsPause() const {
        return mPauseFlag != false;
    }
    bool IsActive() const {
        return mActiveFlag;
    }
    bool IsAutoUpdateSweep() const {
        return mAutoSweep;
    }
    Voice* GetVoice() const {
        return mVoice;
    }

    void SetUserVolume(f32 volume) {
        mUserVolume = volume;
    }
    void SetUserPitch(f32 pitch) {
        mUserPitch = pitch;
    }
    void SetUserPitchRatio(f32 ratio) {
        mUserPitchRatio = ratio;
    }
    void SetUserPan(f32 pan) {
        mUserPan = pan;
    }
    void SetUserSurroundPan(f32 pan) {
        mUserSurroundPan = pan;
    }
    void SetUserLpfFreq(f32 freq) {
        mUserLpfFreq = freq;
    }

    void SetRemoteFilter(int filter) {
        mRemoteFilter = filter;
    }
    void SetOutputLine(int flag) {
        mOutputLineFlag = flag;
    }

    void SetMainOutVolume(f32 volume) {
        mMainOutVolume = volume;
    }
    void SetMainSend(f32 send) {
        mMainSend = send;
    }
    void SetFxSend(AuxBus bus, f32 send) {
        mFxSend[bus] = send;
    }

    void SetRemoteOutVolume(int remote, f32 volume) {
        mRemoteOutVolume[remote] = volume;
    }
    void SetVoiceOutParam(int index, const VoiceOutParam& rParam) {
        mVoice->SetVoiceOutParam(index, rParam);
    }

    void UpdateSweep(int count);
    void SetBiquadFilter(int type, f32 value);
    void SetSweepParam(f32 pitch, int time, bool autoUpdate);
    f32 GetSweepValue() const;

    void SetInitVolume(f32 volume) {
        mInitVolume = volume;
    }
    void SetInitPan(f32 pan) {
        mInitPan = pan;
    }
    void SetInitSurroundPan(f32 pan) {
        mInitSurroundPan = pan;
    }
    void SetTune(f32 tune) {
        mTune = tune;
    }
    void SetSilence(bool silence, int fadeTime) {
        mSilenceVolume.SetTarget(silence ? 0 : SILENCE_VOLUME_MAX, fadeTime);
    }

    void SetKey(int key) {
        mKey = key;
    }
    void SetOriginalKey(int key) {
        mOriginalKey = key;
    }

    s32 GetLength() const {
        return mLength;
    }
    void SetLength(s32 length) {
        mLength = length;
    }

    void SetPanMode(PanMode mode) {
        mPanMode = mode;
    }
    void SetPanCurve(PanCurve curve) {
        mPanCurve = curve;
    }

    int GetAlternateAssignId() const {
        return mAlternateAssign;
    }
    void SetAlternateAssignId(int id) {
        mAlternateAssign = id;
    }

    void SetWaveDataLocationCallback(WaveDataLocationCallback* pCallback,
                                     const WaveInfo* pWaveInfo) {
        mWaveDataLocationCallback = pCallback;
        mWaveInfo = pWaveInfo;
    }

    Channel* GetNextTrackChannel() const {
        return mNextLink;
    }
    void SetNextTrackChannel(Channel* pChannel) {
        mNextLink = pChannel;
    }

    static Channel* AllocChannel(int channels, int voices, int priority,
                                 ChannelCallback pCallback, u32 callbackArg);
    static void FreeChannel(Channel* pChannel);

private:
    static const u8 SILENCE_VOLUME_MAX = 255;

    static const int KEY_INIT = 60;
    static const int ORIGINAL_KEY_INIT = 60;

    static const int PRIORITY_RELEASE = 1;

private:
    static void VoiceCallbackFunc(Voice* pDropVoice,
                                  Voice::VoiceCallbackStatus status,
                                  void* pCallbackArg);

private:
    EnvGenerator mEnvelope;
    Lfo mLfo;
    u8 mLfoTarget;

    bool mPauseFlag;
    bool mActiveFlag;
    bool mAllocFlag;
    bool mAutoSweep;
    bool mReleasePriorityFixFlag;
    u8 mReleaseIgnoreFlag;
    u8 mBiquadType;
    u8 mRemoteFilter;

    f32 mUserVolume;
    f32 mUserPitchRatio;
    f32 mUserPan;
    f32 mUserSurroundPan;
    f32 mUserLpfFreq;
    f32 mBiquadValue;
    int mOutputLineFlag;

    f32 mMainOutVolume;
    f32 mMainSend;
    f32 mFxSend[AUX_BUS_NUM];

    f32 mRemoteOutVolume[WPAD_MAX_CONTROLLERS];

    f32 mUserPitch;
    f32 mSweepPitch;
    int mSweepCounter;
    int mSweepLength;

    f32 mInitVolume;
    f32 mInitPan;
    f32 mInitSurroundPan;
    f32 mTune;
    MoveValue<u8, u16> mSilenceVolume;

    int mKey;
    int mOriginalKey;
    int mLength;

    PanMode mPanMode;
    PanCurve mPanCurve;
    int mAlternateAssign;

    ChannelCallback mCallback;
    u32 mCallbackData;

    WaveDataLocationCallback* mWaveDataLocationCallback;
    const WaveInfo* mWaveInfo;

    Voice* mVoice;
    Channel* mNextLink;

public:
    NW4R_UT_LINKLIST_NODE_DECL();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Channel);

}
}
}

#endif
