#include <nw4r/snd/snd_BasicSound.h>

#include <nw4r/snd/snd_BasicPlayer.h>
#include <nw4r/snd/snd_ExternalSoundPlayer.h>
#include <nw4r/snd/snd_SoundActor.h>
#include <nw4r/snd/snd_SoundHandle.h>
#include <nw4r/snd/snd_SoundPlayer.h>

#include <limits.h>
#include <string.h>

namespace nw4r {
namespace snd {
namespace detail {

NW4R_UT_RTTI_DEF_BASE(BasicSound);

BasicSound::BasicSound(int priority, int ambientPriority)
    : mPlayerHeap(nullptr),
      mGeneralHandle(nullptr),
      mTempGeneralHandle(nullptr),
      mSoundPlayer(nullptr),
      mSoundActor(nullptr),
      mExtSoundPlayer(nullptr),
      mId(INVALID_ID) {
    mAmbientInfo.paramUpdateCallback = nullptr;
    mAmbientInfo.argUpdateCallback = nullptr;
    mAmbientInfo.argAllocaterCallback = nullptr;
    mAmbientInfo.arg = nullptr;
    mAmbientInfo.argSize = 0;

    mVoiceOutCount = 1;
    mPriority = priority;
    mAmbientParam.priority = ambientPriority;
}

void BasicSound::InitParam() {
    mPauseState = PAUSE_STATE_NORMAL;
    mUnPauseFlag = false;
    mStartFlag = false;
    mStartedFlag = false;
    mAutoStopFlag = false;
    mFadeOutFlag = false;
    mAutoStopCounter = 0;
    mUpdateCounter = 0;

    mFadeVolume.InitValue(0.0f);
    mPauseFadeVolume.InitValue(1.0f);
    mFadeVolume.SetTarget(1.0f, 1);

    mInitVolume = 1.0f;
    mExtPitch = 1.0f;
    mExtPan = 0.0f;
    mExtSurroundPan = 0.0f;
    mExtMoveVolume.InitValue(1.0f);
    mLpfFreq = 0.0f;
    mBiquadFilterType = 0;
    mBiquadFilterValue = 0.0f;

    mOutputLineFlag = mSoundPlayer != nullptr
                          ? mSoundPlayer->GetDefaultOutputLine()
                          : OUTPUT_LINE_MAIN;

    mMainOutVolume = 1.0f;
    mMainSend = 0.0f;

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        mFxSend[i] = 0.0f;
    }

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        mRemoteOutVolume[i] = 1.0f;
    }

    mAmbientParam.volume = 1.0f;
    mAmbientParam.pitch = 1.0f;
    mAmbientParam.pan = 0.0f;
    mAmbientParam.surroundPan = 0.0f;
    mAmbientParam.fxSend = 0.0f;
    mAmbientParam.lpf = 0.0f;
    mAmbientParam.biquadFilterValue = 0.0f;
    mAmbientParam.biquadFilterType = 0;
    mAmbientParam.priority = 0;
}

void BasicSound::StartPrepared() {
    if (!mStartedFlag) {
        mStartFlag = true;
    }
}

void BasicSound::Stop(int frames) {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (frames == 0 || !rPlayer.IsActive() || !rPlayer.IsStarted() ||
        rPlayer.IsPause()) {
        Shutdown();
        return;
    }

    int fadeFrames = frames * mFadeVolume.GetValue();
    mFadeVolume.SetTarget(0.0f, fadeFrames);
    SetPlayerPriority(0);

    mAutoStopFlag = false;
    mPauseState = PAUSE_STATE_NORMAL;
    mUnPauseFlag = false;
    mFadeOutFlag = true;
}

void BasicSound::Pause(bool flag, int frames) {
    int fadeFrames;

    if (flag) {
        switch (mPauseState) {
        case PAUSE_STATE_NORMAL:
        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_UNPAUSING:
            fadeFrames = frames * mPauseFadeVolume.GetValue();
            if (fadeFrames <= 0) {
                fadeFrames = 1;
            }

            mPauseFadeVolume.SetTarget(0.0f, fadeFrames);
            mPauseState = PAUSE_STATE_PAUSING;
            mUnPauseFlag = false;
            return;

        case PAUSE_STATE_PAUSED:
            return;

        default:
            return;
        }
    } else {
        switch (mPauseState) {
        case PAUSE_STATE_NORMAL:
            return;

        case PAUSE_STATE_PAUSING:
        case PAUSE_STATE_PAUSED:
        case PAUSE_STATE_UNPAUSING:
            fadeFrames = frames * (1.0f - mPauseFadeVolume.GetValue());
            if (fadeFrames <= 0) {
                fadeFrames = 1;
            }

            mPauseFadeVolume.SetTarget(1.0f, fadeFrames);
            mPauseState = PAUSE_STATE_UNPAUSING;
            mUnPauseFlag = true;
            return;

        default:
            return;
        }
    }
}

void BasicSound::SetAutoStopCounter(int count) {
    mAutoStopCounter = count;
    mAutoStopFlag = count > 0;
}

void BasicSound::FadeIn(int frames) {
    if (!mFadeOutFlag) {
        int fadeFrames = frames * (1.0f - mFadeVolume.GetValue());
        mFadeVolume.SetTarget(1.0f, fadeFrames);
    }
}

bool BasicSound::IsPause() const {
    return mPauseState == PAUSE_STATE_PAUSING ||
           mPauseState == PAUSE_STATE_PAUSED;
}

void BasicSound::Update() {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (mAutoStopFlag && rPlayer.IsActive()) {
        if (mAutoStopCounter == 0) {
            if (mPauseState == PAUSE_STATE_NORMAL ||
                mPauseState == PAUSE_STATE_UNPAUSING) {
                Stop(0);
                return;
            }
        } else {
            mAutoStopCounter--;
        }
    }

    bool startPlayer = false;

    if (!mStartedFlag) {
        if (!mStartFlag) {
            return;
        }

        if (!IsPrepared()) {
            return;
        }

        startPlayer = true;
    }

    if (rPlayer.IsStarted() && mUpdateCounter < ULONG_MAX) {
        mUpdateCounter++;
    }

    if (!rPlayer.IsActive()) {
        Shutdown();
        return;
    }

    switch (mPauseState) {
    case PAUSE_STATE_PAUSING:
        mPauseFadeVolume.Update();
        break;

    case PAUSE_STATE_UNPAUSING:
        mPauseFadeVolume.Update();
        UpdateMoveValue();
        break;

    case PAUSE_STATE_NORMAL:
        UpdateMoveValue();
        break;
    }

    if (mAmbientInfo.argUpdateCallback != nullptr) {
        mAmbientInfo.argUpdateCallback->detail_UpdateAmbientArg(
            mAmbientInfo.arg, this);
    }

    if (mAmbientInfo.paramUpdateCallback != nullptr) {
        SoundAmbientParam ambientParam;

        if (mUpdateCounter != 0) {
            ambientParam.volume = mAmbientParam.volume;
            ambientParam.pitch = mAmbientParam.pitch;
            ambientParam.pan = mAmbientParam.pan;
            ambientParam.surroundPan = mAmbientParam.surroundPan;
            ambientParam.fxSend = mAmbientParam.fxSend;
            ambientParam.lpf = mAmbientParam.lpf;
            ambientParam.biquadFilterValue = mAmbientParam.biquadFilterValue;
            ambientParam.biquadFilterType = mAmbientParam.biquadFilterType;
            ambientParam.priority = mAmbientParam.priority;
            ambientParam.userData = mAmbientParam.userData;
        } else {
            ambientParam.userData = 0;
        }

        for (int i = 0; i < mVoiceOutCount; i++) {
            ambientParam.voiceOutParam[i] = rPlayer.GetVoiceOutParam(i);
        }

        mAmbientInfo.paramUpdateCallback->detail_UpdateAmbientParam(
            mAmbientInfo.arg, mId, mVoiceOutCount, &ambientParam);

        mAmbientParam.volume = ambientParam.volume;
        mAmbientParam.pitch = ambientParam.pitch;
        mAmbientParam.pan = ambientParam.pan;
        mAmbientParam.surroundPan = ambientParam.surroundPan;
        mAmbientParam.fxSend = ambientParam.fxSend;
        mAmbientParam.lpf = ambientParam.lpf;
        mAmbientParam.biquadFilterValue = ambientParam.biquadFilterValue;
        mAmbientParam.biquadFilterType = ambientParam.biquadFilterType;
        mAmbientParam.priority = ambientParam.priority;
        mAmbientParam.userData = ambientParam.userData;

        for (int i = 0; i < mVoiceOutCount; i++) {
            rPlayer.SetVoiceOutParam(i, ambientParam.voiceOutParam[i]);
        }
    }

    if (mSoundActor != nullptr) {
        mActorParam = mSoundActor->detail_GetActorParam();
    }

    UpdateParam();

    if (mFadeOutFlag && mFadeVolume.IsFinished()) {
        mFadeOutFlag = false;
        Shutdown();
        return;
    }

    if (startPlayer) {
        if (rPlayer.Start()) {
            mStartedFlag = true;
            mStartFlag = false;
        } else {
            Shutdown();
            return;
        }
    }

    if (mPauseState == PAUSE_STATE_PAUSING) {
        if (mPauseFadeVolume.IsFinished()) {
            rPlayer.Pause(true);
            mPauseState = PAUSE_STATE_PAUSED;
        }
    } else if (mPauseState == PAUSE_STATE_UNPAUSING) {
        if (mPauseFadeVolume.IsFinished()) {
            mPauseState = PAUSE_STATE_NORMAL;
        }
    }

    if (mUnPauseFlag) {
        rPlayer.Pause(false);
        mUnPauseFlag = false;
    }
}

void BasicSound::UpdateMoveValue() {
    mFadeVolume.Update();
    mExtMoveVolume.Update();
}

void BasicSound::UpdateParam() {
    f32 volume = 1.0f;
    volume *= mInitVolume;
    volume *= GetSoundPlayer()->GetVolume();
    volume *= mExtMoveVolume.GetValue();
    volume *= mFadeVolume.GetValue();
    volume *= mPauseFadeVolume.GetValue();
    volume *= mAmbientParam.volume;
    volume *= mActorParam.volume;

    f32 pan = 0.0f;
    pan += mExtPan;
    pan += mAmbientParam.pan;
    pan += mActorParam.pan;

    f32 surroundPan = 0.0f;
    surroundPan += mExtSurroundPan;
    surroundPan += mAmbientParam.surroundPan;

    f32 pitch = 1.0f;
    pitch *= mExtPitch;
    pitch *= mAmbientParam.pitch;
    pitch *= mActorParam.pitch;

    f32 lpfFreq = mLpfFreq;
    lpfFreq += mAmbientParam.lpf;
    lpfFreq += GetSoundPlayer()->GetLpfFreq();

    int biquadFilterType = mBiquadFilterType;
    f32 biquadFilterValue = mBiquadFilterValue;

    if (biquadFilterType == 0) {
        biquadFilterType = GetSoundPlayer()->GetBiquadFilterType();
        biquadFilterValue = GetSoundPlayer()->GetBiquadFilterValue();

        if (biquadFilterType == 0) {
            biquadFilterType = mAmbientParam.biquadFilterType;
            biquadFilterValue = mAmbientParam.biquadFilterValue;
        }
    }

    int outputLineFlag = mOutputLineFlag;

    f32 mainOutVolume = 1.0f;
    mainOutVolume *= mMainOutVolume;
    mainOutVolume *= GetSoundPlayer()->GetMainOutVolume();

    f32 remoteOutVolume[WPAD_MAX_CONTROLLERS];
    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        remoteOutVolume[i] = 1.0f;
        remoteOutVolume[i] *= GetSoundPlayer()->GetRemoteOutVolume(i);
        remoteOutVolume[i] *= mRemoteOutVolume[i];
    }

    f32 mainSend = 0.0f;
    mainSend += mMainSend;
    mainSend += GetSoundPlayer()->GetMainSend();

    f32 fxSend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++) {
        fxSend[i] = 0.0f;
        fxSend[i] += mFxSend[i];
        fxSend[i] += GetSoundPlayer()->GetFxSend(i);
    }

    fxSend[AUX_A] += mAmbientParam.fxSend;

    BasicPlayer& rPlayer = GetBasicPlayer();
    rPlayer.SetVolume(volume);
    rPlayer.SetPan(pan);
    rPlayer.SetSurroundPan(surroundPan);
    rPlayer.SetPitch(pitch);
    rPlayer.SetLpfFreq(lpfFreq);
    rPlayer.SetBiquadFilter(biquadFilterType, biquadFilterValue);
    rPlayer.SetOutputLine(outputLineFlag);
    rPlayer.SetMainOutVolume(mainOutVolume);

    for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
        rPlayer.SetRemoteOutVolume(i, remoteOutVolume[i]);
    }

    rPlayer.SetMainSend(mainSend);

    for (int i = 0; i < AUX_BUS_NUM; i++) {
        rPlayer.SetFxSend(static_cast<AuxBus>(i), fxSend[i]);
    }
}

void BasicSound::Shutdown() {
    BasicPlayer& rPlayer = GetBasicPlayer();

    if (rPlayer.IsActive()) {
        if (mFadeOutFlag) {
            rPlayer.SetVolume(0.0f);
        }

        rPlayer.Stop();
    }

    SetId(INVALID_ID);

    if (IsAttachedGeneralHandle()) {
        DetachGeneralHandle();
    }

    if (IsAttachedTempGeneralHandle()) {
        DetachTempGeneralHandle();
    }

    if (IsAttachedTempSpecialHandle()) {
        DetachTempSpecialHandle();
    }

    if (mPlayerHeap != nullptr) {
        mSoundPlayer->detail_FreePlayerHeap(this);
    }

    if (mSoundPlayer != nullptr) {
        mSoundPlayer->detail_RemoveSound(this);
    }

    if (mExtSoundPlayer != nullptr) {
        mExtSoundPlayer->RemoveSound(this);
    }

    if (mAmbientInfo.argAllocaterCallback != nullptr) {
        mAmbientInfo.argAllocaterCallback->detail_FreeAmbientArg(
            mAmbientInfo.arg, this);
        mAmbientInfo.arg = nullptr;
    }

    mStartedFlag = false;
    mFadeOutFlag = false;
}

void BasicSound::AttachPlayerHeap(PlayerHeap* pHeap) {
    mPlayerHeap = pHeap;
}

void BasicSound::DetachPlayerHeap(PlayerHeap* pHeap) {
    mPlayerHeap = nullptr;
}

void BasicSound::AttachSoundPlayer(SoundPlayer* pPlayer) {
    mSoundPlayer = pPlayer;
}

void BasicSound::DetachSoundPlayer(SoundPlayer* pPlayer) {
    mSoundPlayer = nullptr;
}

void BasicSound::AttachSoundActor(SoundActor* pActor) {
    mSoundActor = pActor;
}

void BasicSound::DetachSoundActor(SoundActor* pActor) {
    mSoundActor = nullptr;
}

void BasicSound::AttachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer) {
    mExtSoundPlayer = pExtPlayer;
}

void BasicSound::DetachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer) {
    mExtSoundPlayer = nullptr;
}

int BasicSound::GetRemainingFadeFrames() const {
    return mPauseFadeVolume.GetRemainingTime();
}

int BasicSound::GetVoiceOutCount() const {
    return mVoiceOutCount;
}

void BasicSound::SetPlayerPriority(int priority) {
    mPriority = priority;

    if (mSoundPlayer != nullptr) {
        mSoundPlayer->detail_SortPriorityList(this);
    }

    OnUpdatePlayerPriority();
}

void BasicSound::SetInitialVolume(f32 volume) {
    if (volume < 0.0f) {
        volume = 0.0f;
    }

    mInitVolume = volume;
}

void BasicSound::SetVolume(f32 volume, int frames) {
    if (volume < 0.0f) {
        volume = 0.0f;
    }

    mExtMoveVolume.SetTarget(volume, frames);
}

void BasicSound::SetPitch(f32 pitch) {
    mExtPitch = pitch;
}

void BasicSound::SetPan(f32 pan) {
    mExtPan = pan;
}

void BasicSound::SetSurroundPan(f32 pan) {
    mExtSurroundPan = pan;
}

void BasicSound::SetLpfFreq(f32 freq) {
    mLpfFreq = freq;
}

void BasicSound::SetOutputLineFlag(int flag) {
    mOutputLineFlag = flag;
}

void BasicSound::SetFxSend(AuxBus bus, f32 send) {
    mFxSend[bus] = send;
}

void BasicSound::SetRemoteFilter(int filter) {
    GetBasicPlayer().SetRemoteFilter(filter);
}

void BasicSound::SetPanMode(PanMode mode) {
    GetBasicPlayer().SetPanMode(mode);
}

void BasicSound::SetPanCurve(PanCurve curve) {
    GetBasicPlayer().SetPanCurve(curve);
}

void BasicSound::SetAmbientInfo(const AmbientInfo& rInfo) {
    void* pArg = rInfo.argAllocaterCallback->detail_AllocAmbientArg(rInfo.argSize);
    if (pArg == nullptr) {
        return;
    }

    memcpy(pArg, rInfo.arg, rInfo.argSize);

    mAmbientInfo = rInfo;
    mAmbientInfo.arg = pArg;

    if (rInfo.paramUpdateCallback != nullptr) {
        int voiceOutCount =
            mAmbientInfo.paramUpdateCallback->detail_GetRequiredVoiceOutCount(
                mAmbientInfo.arg, mId);

        if (voiceOutCount > WPAD_MAX_CONTROLLERS) {
            voiceOutCount = WPAD_MAX_CONTROLLERS;
        }

        mVoiceOutCount = voiceOutCount;
    }
}

int BasicSound::GetAmbientPriority(const AmbientInfo& rInfo, u32 id) {
    if (rInfo.paramUpdateCallback == nullptr) {
        return PRIORITY_MIN;
    }

    int priority =
        rInfo.paramUpdateCallback->detail_GetAmbientPriority(rInfo.arg, id);
    return priority;
}

bool BasicSound::IsAttachedGeneralHandle() {
    return mGeneralHandle != nullptr;
}

bool BasicSound::IsAttachedTempGeneralHandle() {
    return mTempGeneralHandle != nullptr;
}

void BasicSound::DetachGeneralHandle() {
    mGeneralHandle->DetachSound();
}

void BasicSound::DetachTempGeneralHandle() {
    mTempGeneralHandle->DetachSound();
}

void BasicSound::SetId(u32 id) {
    mId = id;
    GetBasicPlayer().SetId(id);
}

}
}
}
