#include <nw4r/snd/snd_Channel.h>

#include <nw4r/snd/snd_ChannelManager.h>
#include <nw4r/snd/snd_Util.h>
#include <nw4r/snd/snd_VoiceManager.h>
#include <nw4r/snd/snd_WaveFile.h>

#include <nw4r/ut/ut_Lock.h>

inline void* operator new(size_t, void* p) {
    return p;
}

namespace nw4r {
namespace snd {
namespace detail {

ChannelManager& ChannelManager::GetInstance() {
    static ChannelManager instance;
    return instance;
}

ChannelManager::ChannelManager() : mInitialized(false), mChannelCount(0) {}

u32 ChannelManager::GetRequiredMemSize(int channelCount) {
    return sizeof(Channel) * (channelCount + 1);
}

void ChannelManager::Setup(void* pWork, u32 workSize) {
    ut::AutoInterruptLock lock;

    if (mInitialized) {
        return;
    }

    mChannelCount = mPool.Create(pWork, workSize);
    mMem = pWork;
    mMemSize = workSize;
    mInitialized = true;
}

void ChannelManager::Shutdown() {
    ut::AutoInterruptLock lock;

    if (!mInitialized) {
        return;
    }

    for (ChannelList::Iterator it = mChannelList.GetBeginIter();
         it != mChannelList.GetEndIter();) {
        ChannelList::Iterator curr = it++;
        curr->Stop();
    }

    mPool.Destroy(mMem, mMemSize);
    mInitialized = false;
}

Channel* ChannelManager::Alloc() {
    Channel* pChannel = mPool.Alloc();
    mChannelList.PushBack(pChannel);
    return pChannel;
}

void ChannelManager::Free(Channel* pChannel) {
    mChannelList.Erase(pChannel);
    mPool.Free(pChannel);
}

void ChannelManager::UpdateAllChannel() {
    for (ChannelList::Iterator it = mChannelList.GetBeginIter();
         it != mChannelList.GetEndIter();) {
        ChannelList::Iterator curr = it++;
        curr->Update(true);
    }
}

Channel::Channel()
    : mPauseFlag(false), mActiveFlag(false), mAllocFlag(false), mVoice(nullptr) {}

Channel::~Channel() {}

void Channel::InitParam(ChannelCallback pCallback, u32 callbackArg) {
    mNextLink = nullptr;
    mCallback = pCallback;
    mCallbackData = callbackArg;
    mWaveDataLocationCallback = nullptr;
    mWaveInfo = nullptr;
    mPauseFlag = false;
    mAutoSweep = true;
    mReleasePriorityFixFlag = false;
    mReleaseIgnoreFlag = false;
    mLength = 0;
    mKey = KEY_INIT;
    mOriginalKey = ORIGINAL_KEY_INIT;
    mInitVolume = 1.0f;
    mInitPan = 0.0f;
    mInitSurroundPan = 0.0f;
    mTune = 1.0f;
    mUserVolume = 1.0f;
    mUserPitch = 0.0f;
    mUserPitchRatio = 1.0f;
    mUserPan = 0.0f;
    mUserSurroundPan = 0.0f;
    mUserLpfFreq = 0.0f;
    mBiquadType = 0;
    mBiquadValue = 0.0f;
    mRemoteFilter = 0;
    mOutputLineFlag = OUTPUT_LINE_MAIN;
    mMainOutVolume = 1.0f;
    mMainSend = 0.0f;

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        mFxSend[i] = 0.0f;
    }

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mRemoteOutVolume[i] = 1.0f;
    }

    mSilenceVolume.InitValue(SILENCE_VOLUME_MAX);

    mSweepPitch = 0.0f;
    mSweepLength = 0;
    mSweepCounter = 0;

    mEnvelope.Init();
    mLfo.GetParam().Init();

    mLfoTarget = LFO_TARGET_PITCH;
    mPanMode = PAN_MODE_DUAL;
    mPanCurve = PAN_CURVE_SQRT;
    mAlternateAssign = 0;
}

void Channel::Update(bool periodic) {
    if (!mActiveFlag) {
        return;
    }

    if (mPauseFlag) {
        periodic = false;
    }

    f32 lfoValue = mLfo.GetValue();

    mSilenceVolume.Update();

    f32 volume = 1.0f;
    volume *= mInitVolume;
    volume *= mUserVolume;
    volume *= mSilenceVolume.GetValue() / static_cast<f32>(SILENCE_VOLUME_MAX);

    f32 veInitVolume = 1.0f;
    veInitVolume *= Util::CalcVolumeRatio(mEnvelope.GetValue());

    if (mLfoTarget == LFO_TARGET_VOLUME) {
        veInitVolume *= Util::CalcVolumeRatio(lfoValue * 6.0f);
    }

    if (mEnvelope.GetStatus() == EnvGenerator::STATUS_RELEASE) {
        if (mCallback != nullptr) {
            if (veInitVolume == 0.0f) {
                Stop();
                return;
            }
        } else {
            if (volume * veInitVolume == 0.0f) {
                Stop();
                return;
            }
        }
    }

    f32 cent = 0.0f;
    cent += mKey - mOriginalKey;
    cent += GetSweepValue();
    cent += mUserPitch;

    if (mLfoTarget == LFO_TARGET_PITCH) {
        cent += lfoValue;
    }

    f32 pitchRatio = 1.0f;
    pitchRatio *= mTune;
    pitchRatio *= mUserPitchRatio;

    f32 pitch = Util::CalcPitchRatio(cent * 256.0f);
    pitch *= pitchRatio;

    f32 pan = 0.0f;
    pan += mInitPan;
    pan += mUserPan;

    if (mLfoTarget == LFO_TARGET_PAN) {
        pan += lfoValue;
    }

    f32 surroundPan = 0.0f;
    surroundPan += mInitSurroundPan;
    surroundPan += mUserSurroundPan;

    f32 lpfFreq = 1.0f;
    lpfFreq += mUserLpfFreq;

    int remoteFilter = 0;
    remoteFilter += mRemoteFilter;

    f32 mainOutVolume = 1.0f;
    mainOutVolume *= mMainOutVolume;

    f32 mainSend = 0.0f;
    mainSend += mMainSend;

    f32 fxSend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        fxSend[i] = 0.0f;
        fxSend[i] += mFxSend[i];
    }

    f32 remoteOutVolume[WPAD_MAX_CONTROLLERS];
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        remoteOutVolume[i] = 1.0f;
        remoteOutVolume[i] *= mRemoteOutVolume[i];
    }

    if (periodic) {
        if (mAutoSweep) {
            UpdateSweep(3);
        }

        mLfo.Update(3);
        mEnvelope.Update(3);
    }

    f32 nextLfoValue = mLfo.GetValue();

    f32 veTargetVolume = 1.0f;
    veTargetVolume *= Util::CalcVolumeRatio(mEnvelope.GetValue());

    if (mLfoTarget == LFO_TARGET_VOLUME) {
        veTargetVolume *= Util::CalcVolumeRatio(nextLfoValue * 6.0f);
    }

    if (mVoice != nullptr) {
        mVoice->SetPanMode(mPanMode);
        mVoice->SetPanCurve(mPanCurve);
        mVoice->SetVolume(volume);
        mVoice->SetVeVolume(veTargetVolume, veInitVolume);
        mVoice->SetPitch(pitch);
        mVoice->SetPan(pan);
        mVoice->SetSurroundPan(surroundPan);
        mVoice->SetLpfFreq(lpfFreq);
        mVoice->SetBiquadFilter(mBiquadType, mBiquadValue);
        mVoice->SetRemoteFilter(remoteFilter);
        mVoice->SetOutputLine(mOutputLineFlag);
        mVoice->SetMainOutVolume(mainOutVolume);
        mVoice->SetMainSend(mainSend);

        for (int i = 0; i < AUX_BUS_NUM; i++) {
            mVoice->SetFxSend(static_cast<AuxBus>(i), fxSend[i]);
        }

        for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
            mVoice->SetRemoteOutVolume(i, remoteOutVolume[i]);
        }
    }
}

void Channel::Start(const WaveInfo& rInfo, int length, u32 offset) {
    mLength = length;

    mLfo.Reset();
    mEnvelope.Reset();
    mSweepCounter = 0;

    mVoice->Setup(rInfo, offset);
    mVoice->Start();
    mActiveFlag = true;
}

void Channel::Release() {
    if (mEnvelope.GetStatus() != EnvGenerator::STATUS_RELEASE) {
        if (mVoice != nullptr && !mReleasePriorityFixFlag) {
            mVoice->SetPriority(PRIORITY_RELEASE);
        }

        mEnvelope.SetStatus(EnvGenerator::STATUS_RELEASE);
    }

    mPauseFlag = false;
}

void Channel::NoteOff() {
    if (!mReleaseIgnoreFlag) {
        Release();
    }
}

void Channel::Stop() {
    if (mVoice == nullptr) {
        return;
    }

    mVoice->Stop();
    mVoice->Free();
    mVoice = nullptr;

    mPauseFlag = false;
    mActiveFlag = false;

    if (mCallback != nullptr) {
        mCallback(this, CALLBACK_STATUS_STOPPED, mCallbackData);
    }

    if (mWaveDataLocationCallback != nullptr) {
        mWaveDataLocationCallback->ReleaseWaveInfo(mWaveInfo);
    }

    if (mAllocFlag) {
        mAllocFlag = false;
        ChannelManager::GetInstance().Free(this);
    }
}

void Channel::UpdateSweep(int count) {
    mSweepCounter += count;

    if (mSweepCounter > mSweepLength) {
        mSweepCounter = mSweepLength;
    }
}

void Channel::SetSweepParam(f32 pitch, int time, bool autoUpdate) {
    mSweepPitch = pitch;
    mSweepLength = time;
    mAutoSweep = autoUpdate;
    mSweepCounter = 0;
}

f32 Channel::GetSweepValue() const {
    if (mSweepPitch == 0.0f) {
        return 0.0f;
    }

    if (mSweepCounter >= mSweepLength) {
        return 0.0f;
    }

    f32 sweep = mSweepPitch * (mSweepLength - mSweepCounter);
    sweep /= mSweepLength;

    return sweep;
}

void Channel::SetBiquadFilter(int type, f32 value) {
    mBiquadType = type;
    mBiquadValue = value;
}

void Channel::VoiceCallbackFunc(Voice* pDropVoice,
                                Voice::VoiceCallbackStatus status,
                                void* pCallbackArg) {
    ChannelCallbackStatus channelStatus;

    switch (status) {
    case Voice::CALLBACK_STATUS_FINISH_WAVE:
        channelStatus = CALLBACK_STATUS_FINISH;
        pDropVoice->Free();
        break;

    case Voice::CALLBACK_STATUS_CANCEL:
        channelStatus = CALLBACK_STATUS_CANCEL;
        pDropVoice->Free();
        break;

    case Voice::CALLBACK_STATUS_DROP_VOICE:
        channelStatus = CALLBACK_STATUS_DROP;
        break;

    case Voice::CALLBACK_STATUS_DROP_DSP:
        channelStatus = CALLBACK_STATUS_DROP;
        break;
    }

    Channel* pChannel = static_cast<Channel*>(pCallbackArg);

    if (pChannel->mCallback != nullptr) {
        pChannel->mCallback(pChannel, channelStatus, pChannel->mCallbackData);
    }

    if (pChannel->mWaveDataLocationCallback != nullptr) {
        pChannel->mWaveDataLocationCallback->ReleaseWaveInfo(
            pChannel->mWaveInfo);
    }

    pChannel->mVoice = nullptr;
    pChannel->mPauseFlag = false;
    pChannel->mActiveFlag = false;
    pChannel->mAllocFlag = false;

    ChannelManager::GetInstance().Free(pChannel);
}

Channel* Channel::AllocChannel(int channels, int voices, int priority,
                               ChannelCallback pCallback, u32 callbackArg) {
    Channel* pChannel = ChannelManager::GetInstance().Alloc();
    if (pChannel == nullptr) {
        return nullptr;
    }

    pChannel->mAllocFlag = true;

    Voice* pVoice = VoiceManager::GetInstance().AllocVoice(
        channels, voices, priority, VoiceCallbackFunc, pChannel);

    if (pVoice == nullptr) {
        ChannelManager::GetInstance().Free(pChannel);
        return nullptr;
    }

    pChannel->mVoice = pVoice;
    pChannel->InitParam(pCallback, callbackArg);

    return pChannel;
}

void Channel::FreeChannel(Channel* pChannel) {
    if (pChannel == nullptr) {
        return;
    }

    pChannel->mCallback = nullptr;
    pChannel->mCallbackData = 0;
}

}
}
}
