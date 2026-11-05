#include <game/input/detail/PadData.h>
#include <game/core/MathUtil.h>
#include <game/core/Memory.h>
#include <game/core/ResetCallback.h>
#include <functional>
#include <nw4r/snd/snd_RemoteSpeakerManager.h>
#include <nw4r/snd/snd_SoundSystem.h>

extern "C" void KPADEnableAimingMode(s32 channel);
void ClampGCPads(PADStatus* pStatus);
void SamplePad(OSAlarm* pAlarm, OSContext* pContext);
void ShutdownPad();
unsigned int DetectWiiShake(int channel);

namespace {
std::tr1::shared_ptr<PadData> spPadData;
int sPointerDisableCounts[4];
const unsigned int scPadChannelMasks[4] = {PAD_CHAN0_BIT, PAD_CHAN1_BIT, PAD_CHAN2_BIT, PAD_CHAN3_BIT};
}

inline PadData::PadData() : mWriteIndex(0), mReadIndex(0), mChannelCount(4), mMinPointerIdleFrames(0), mMinButtonIdleFrames(0), mMinNunchukIdleFrames(0), mMinInputIdleFrames(0), mClampMode(3), mClampRegion(0), mShutdownCallbackId(0) {
    memset(&mAlarm, 0, sizeof(mAlarm));
    memset(mWiiHistory, 0, sizeof(mWiiHistory));
    memset(mWiiWorking, 0, sizeof(mWiiWorking));
    memset(mWiiPublic, 0, sizeof(mWiiPublic));
    memset(mConnectCallbacks, 0, sizeof(mConnectCallbacks));
    memset(mGCRawHistory, 0, sizeof(mGCRawHistory));
    memset(mGCWorking, 0, sizeof(mGCWorking));
    memset(mGCPublic, 0, sizeof(mGCPublic));
    for (int channel = 0; channel < 4; ++channel) {
        for (int sample = 0; sample < 60; ++sample) {
            mDirectionHistory[sample][channel] = 0;
            mProbeHistory[sample][channel] = -1;
            mDeviceHistory[sample][channel] = 253;
            mWiiHistory[sample][channel].wpad_err = -1;
        }
        mWiiPublic[channel].wpad_err = -1;
        mWiiPublic[channel].mProbeResult = -1;
        mWiiPublic[channel].mDeviceType = 253;
        mActivityMask[channel] = ~0u;
        mWiiRumble[channel].mChannel = channel;
    }
    for (int channel = 0; channel < 4; ++channel) {
        mGCRumble[channel].mChannel = channel;
    }
}

inline bool HasWiiSample(const KPADStatus* pStatus) {
    return pStatus != nullptr && (pStatus->wpad_err == 0 || pStatus->wpad_err == -7);
}

inline bool HasNunchukSample(const KPADStatus* pStatus) {
    return pStatus != nullptr && pStatus->wpad_err == 0 && pStatus->dev_type == 1;
}

inline bool HasPointerSample(const KPADStatus* pStatus) {
    return HasWiiSample(pStatus) && pStatus->dpd_valid_fg > 0;
}

inline void WiiPadState::UpdateButtons(unsigned int hold) {
    mTrigger = hold & (mHold ^ hold);
    mRelease = mHold & (mHold ^ hold);
    mHold = hold;
    if (mTrigger != 0) {
        mRepeatButtons = mTrigger;
        mButtonRepeat = 0;
        mRepeatDelay = 20;
    } else if (mRepeatButtons != 0) {
        if (hold & mRepeatButtons) {
            if (--mRepeatDelay == 0) {
                mRepeatDelay = 4;
                mButtonRepeat = mRepeatButtons;
            } else {
                mButtonRepeat = 0;
            }
        } else {
            mRepeatButtons = 0;
            mButtonRepeat = 0;
        }
    }
}

inline void GCPadState::UpdateButtons(unsigned int hold) {
    mTrigger = hold & (mHold ^ hold);
    mRelease = mHold & (mHold ^ hold);
    mHold = hold;
    if (mTrigger != 0) {
        mRepeatButtons = mTrigger;
        mRepeat = 0;
        mRepeatDelay = 20;
    } else if (mRepeatButtons != 0) {
        if (hold & mRepeatButtons) {
            if (--mRepeatDelay == 0) {
                mRepeatDelay = 4;
                mRepeat = mRepeatButtons;
            } else {
                mRepeat = 0;
            }
        } else {
            mRepeatButtons = 0;
            mRepeat = 0;
        }
    }
}

inline float NormalizePadAxis(float value, float range) {
    return Min(Max(-1.0f, value / range), 1.0f);
}

inline OSTime SecondsToTicks(float seconds) { return static_cast<OSTime>(seconds * static_cast<float>(OS_TIME_SPEED)); }

inline void BRumble::Schedule(float seconds) {
    OSTime ticks = SecondsToTicks(seconds);
    mStart = OSGetTime();
    OSSetAlarm(&mAlarm, ticks, AlarmCallback);
}

inline void BRumble::UpdatePattern() {
    switch (mState) {
    case 3:
        mState = 0;
    case 0:
        if (mpPattern != nullptr) {
            mState = 1;
            Start();
            Schedule(0.001f * static_cast<float>(static_cast<unsigned int>(mpPattern->mOnMilliseconds)));
        }
        break;
    case 1:
        if (mpPattern->mStopHard) {
            StopHard();
        } else {
            Stop();
        }
        if (mpPattern->mOffMilliseconds == 0) {
            mState = 3;
            mpPattern = nullptr;
            Schedule(mCooldown);
        } else {
            mState = 2;
            Schedule(0.001f * static_cast<float>(static_cast<unsigned int>(mpPattern->mOffMilliseconds)));
        }
        break;
    case 2:
        ++mpPattern;
        mState = 1;
        Start();
        Schedule(0.001f * static_cast<float>(static_cast<unsigned int>(mpPattern->mOnMilliseconds)));
        break;
    }
}

inline void BRumble::StopPattern() {
    InterruptLock lock;
    mpPattern = nullptr;
    switch (mState) {
    case 1:
        StopHard();
        OSCancelAlarm(&mAlarm);
        if (0.0f < mCooldown) {
            mState = 3;
            OSTime ticks = SecondsToTicks(mCooldown);
            mStart = OSGetTime();
            OSSetAlarm(&mAlarm, ticks, AlarmCallback);
        } else {
            mState = 0;
        }
        break;
    case 2:
        OSCancelAlarm(&mAlarm);
        OSTime elapsed = OSGetTime() - mStart;
        OSTime cooldown = static_cast<OSTime>(mCooldown * static_cast<float>(OS_TIME_SPEED));
        if (elapsed < cooldown) {
            mState = 3;
            mStart = OSGetTime();
            OSSetAlarm(&mAlarm, cooldown - elapsed, AlarmCallback);
        } else {
            mState = 0;
        }
        break;
    }
}

inline void BRumble::StartPattern(const RumbleStep* pPattern) {
    InterruptLock lock;
    StopPattern();
    if (CanRumble()) {
        mpPattern = pPattern;
        if (mState == 0) {
            UpdatePattern();
        }
    }
}

inline void BRumble::Shutdown() {
    InterruptLock lock;
    switch (mState) {
    case 1:
        StopHard();
    case 2:
    case 3:
        OSCancelAlarm(&mAlarm);
        break;
    }
    mState = 0;
    mpPattern = nullptr;
}

inline void BRumble::Block() {
    ++mBlockCount;
    if (!CanRumble()) {
        StopPattern();
    }
}

inline void BRumble::Unblock() {
    --mBlockCount;
    if (!CanRumble()) {
        StopPattern();
    }
}

inline void BRumble::SetEnabled(bool enabled) {
    if (mEnabled != enabled) {
        mEnabled = enabled;
        if (!CanRumble()) {
            StopPattern();
        }
    }
}

inline unsigned int GetShakeDirection(const KPADStatus& rStatus) {
    if (rStatus.wpad_err != 0 && rStatus.wpad_err != -7) {
        return 0;
    } else {
        unsigned int direction;
        direction = 0;
        if (1.5f < rStatus.acc.y) {
            direction |= 1;
        } else if (rStatus.acc.y < -2.5f) {
            direction |= 2;
        }
        if (2.0f < rStatus.acc.x) {
            direction |= 4;
        } else if (rStatus.acc.x < -2.0f) {
            direction |= 8;
        }
        return direction;
    }
}

void* AllocatePadMemory(u32 size) {
    return GameMemory::AllocateMEM2(size);
}

u8 FreePadMemory(void* pMemory) {
    GameMemory::Free(pMemory);
    return TRUE;
}

void RemoteSpeakerSetupCallback(s32, s32) {}

void RemoteSpeakerShutdownCallback(s32, s32) {}

void PadConnectionCallback(s32 channel, s32 result) {
    if (result == 0) {
        if (channel >= spPadData->mChannelCount) {
            WPADDisconnect(channel);
        } else {
            if (nw4r::snd::SoundSystem::IsInitializedSoundSystem()) {
                nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Setup(RemoteSpeakerSetupCallback);
            }
            if (sPointerDisableCounts[channel] > 0) {
                KPADDisableDPD(channel);
            }
            if (spPadData->mConnectCallbacks[channel] != nullptr) {
                spPadData->mConnectCallbacks[channel](channel, result);
            }
        }
    } else if (result == -1) {
        if (spPadData->mConnectCallbacks[channel] != nullptr) {
            spPadData->mConnectCallbacks[channel](channel, result);
        }
        if (nw4r::snd::SoundSystem::IsInitializedSoundSystem()) {
            nw4r::snd::detail::RemoteSpeakerManager::GetInstance().GetRemoteSpeaker(channel).Shutdown(RemoteSpeakerShutdownCallback);
        }
    }
}

void ClampGCPads(PADStatus* pStatus) {
    switch (spPadData->mClampMode) {
    default:
        PADClamp(pStatus);
        break;
    case 1:
        PADClamp2(pStatus, 0);
        PADClampTrigger(pStatus, spPadData->mClampRegion);
        break;
    case 2:
        PADClamp2(pStatus, 1);
        PADClampTrigger(pStatus, spPadData->mClampRegion);
        break;
    case 3:
        PADClampCircle(pStatus);
        break;
    case 4:
        PADClampCircle2(pStatus, 2);
        PADClampTrigger(pStatus, spPadData->mClampRegion);
        break;
    case 5:
        PADClampCircle2(pStatus, 3);
        PADClampTrigger(pStatus, spPadData->mClampRegion);
        break;
    }
}

void SamplePad(OSAlarm*, OSContext*) {
    InterruptLock lock;
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mProbeHistory[spPadData->mWriteIndex][channel] = WPADProbe(channel, &spPadData->mDeviceHistory[spPadData->mWriteIndex][channel]);
        KPADStatus* pStatus = &spPadData->mWiiHistory[spPadData->mWriteIndex][channel];
        if (KPADRead(channel, pStatus, 1) <= 0) {
            *pStatus = spPadData->mWiiHistory[(spPadData->mWriteIndex + 59) % 60][channel];
        }
    }
    PADStatus* pStatus = spPadData->mGCRawHistory[spPadData->mWriteIndex];
    PADRead(pStatus);
    ClampGCPads(pStatus);
    unsigned int resetMask = 0;
    for (int channel = 0; channel < 4; ++channel) {
        if (pStatus[channel].err == PAD_ERR_NO_CONTROLLER) {
            resetMask |= scPadChannelMasks[channel];
        }
    }
    if (resetMask != 0) {
        PADReset(resetMask);
    }
    spPadData->mWriteIndex = (spPadData->mWriteIndex + 1) % 60;
    if (spPadData->mReadIndex == spPadData->mWriteIndex) {
        spPadData->mReadIndex = (spPadData->mReadIndex + 1) % 60;
    }
}

unsigned int DetectWiiShake(int channel) {
    int sampleIndex = spPadData->mReadIndex;
    unsigned int direction = GetShakeDirection(spPadData->mWiiHistory[sampleIndex][channel]);
    spPadData->mDirectionHistory[sampleIndex][channel] = direction;
    int shake = 0;
    int previousIndex = spPadData->mReadIndex;
    for (int i = 0; i < 6; ++i) {
        if (previousIndex == spPadData->mWriteIndex) {
            break;
        }
        previousIndex = (previousIndex + 59) % 60;
        int previous = spPadData->mDirectionHistory[previousIndex][channel];
        if ((previous & 1) && (direction & 2)) {
            shake |= 1;
        }
        if ((previous & 2) && (direction & 1)) {
            shake |= 2;
        }
        if ((previous & 4) && (direction & 8)) {
            shake |= 8;
        }
        if ((previous & 8) && (direction & 4)) {
            shake |= 4;
        }
    }
    return shake;
}

void WiiPadState::Update(const KPADStatus* pStatus, unsigned int activityMask) {
    static_cast<KPADStatus&>(*this) = *pStatus;
    unsigned int hold = 0;
    ++mIdleFrames;
    if (HasWiiSample(pStatus)) {
        hold = this->hold;
        if (activityMask & (trig | this->hold)) {
            mIdleFrames = 0;
        }
    }
    ++mNunchukIdleFrames;
    if (HasNunchukSample(pStatus)) {
        if (pStatus->ex_status.fs.stick.x < -0.5f) {
            hold |= 0x40000;
        } else if (0.5f < pStatus->ex_status.fs.stick.x) {
            hold |= 0x80000;
        }
        if (pStatus->ex_status.fs.stick.y < -0.5f) {
            hold |= 0x20000;
        } else if (0.5f < pStatus->ex_status.fs.stick.y) {
            hold |= 0x10000;
        }
        if (0.0f != pStatus->ex_status.fs.stick.x || 0.0f != pStatus->ex_status.fs.stick.y) {
            mNunchukIdleFrames = 0;
        }
    }
    UpdateButtons(hold);
    mRepeat = mButtonRepeat & 0xFFFF;
    if (HasPointerSample(pStatus)) {
        float dx = pStatus->pos.x - mStablePosition.x;
        float dy = pStatus->pos.y - mStablePosition.y;
        if (dx * dx + dy * dy < 0.00007971939339768142f) {
            ++mPointerIdleFrames;
        } else {
            mPointerIdleFrames = 0;
            mStablePosition = pStatus->pos;
        }
    } else {
        ++mPointerIdleFrames;
    }
}

void GCPadState::Update(const PADStatus* pStatus) {
    if (pStatus->err != -3) {
        float triggerRange = spPadData->mClampRegion == 1 ? 180.0f : 150.0f;
        float stickRange;
        float substickRange;
        switch (spPadData->mClampMode) {
        default:
            stickRange = 72.0f;
            substickRange = 59.0f;
            triggerRange = 150.0f;
            break;

        case 1:
            stickRange = 72.0f;
            substickRange = 59.0f;
            break;
        case 2:
            stickRange = 87.0f;
            substickRange = 74.0f;
            break;
        case 3:
            stickRange = 56.0f;
            substickRange = 44.0f;
            triggerRange = 150.0f;
            break;
        case 4:
            stickRange = 62.0f;
            substickRange = 50.0f;
            break;
        case 5:
            stickRange = 80.0f;
            substickRange = 68.0f;
            break;
        }
        mStick.x = NormalizePadAxis(pStatus->stickX, stickRange);
        mStick.y = NormalizePadAxis(pStatus->stickY, stickRange);
        mSubstick.x = NormalizePadAxis(pStatus->substickX, substickRange);
        mSubstick.y = NormalizePadAxis(pStatus->substickY, substickRange);
        mLeftTrigger = NormalizePadAxis(static_cast<unsigned int>(pStatus->triggerLeft), triggerRange);
        mRightTrigger = NormalizePadAxis(static_cast<unsigned int>(pStatus->triggerRight), triggerRange);
        unsigned int hold = pStatus->button;
        if (mStick.x < -0.5f) hold |= 0x40000;
        if (mStick.x > 0.5f) hold |= 0x80000;
        if (mStick.y < -0.5f) hold |= 0x20000;
        if (mStick.y > 0.5f) hold |= 0x10000;
        if (mSubstick.x < -0.5f) hold |= 0x400000;
        if (mSubstick.x > 0.5f) hold |= 0x800000;
        if (mSubstick.y < -0.5f) hold |= 0x200000;
        if (mSubstick.y > 0.5f) hold |= 0x100000;
        UpdateButtons(hold);
        mStickDeltaX = pStatus->stickX - stickX;
        mStickDeltaY = pStatus->stickY - stickY;
        mSubstickDeltaX = pStatus->substickX - substickX;
        mSubstickDeltaY = pStatus->substickY - substickY;
        static_cast<PADStatus&>(*this) = *pStatus;
    } else {
        mRelease = 0;
        mTrigger = 0;
        mRepeatButtons = 0;
        mRepeat = 0;
        mStickDeltaY = 0;
        mStickDeltaX = 0;
        mSubstickDeltaY = 0;
        mSubstickDeltaX = 0;
        mStick.x = 0.0f;
        mStick.y = 0.0f;
        mSubstick.x = 0.0f;
        mSubstick.y = 0.0f;
        mLeftTrigger = 0.0f;
        mRightTrigger = 0.0f;
    }
}

void ShutdownPad() {
    OSCancelAlarm(&spPadData->mAlarm);
    for (int i = 0; i < 4; ++i) {
        spPadData->mWiiRumble[i].Shutdown();
    }
    for (int i = 0; i < 4; ++i) {
        spPadData->mGCRumble[i].Shutdown();
    }
}

OSTime GetPadSampleInterval() {
    unsigned int format = VIGetTvFormat();
    switch (format) {
    default:
        return static_cast<OSTime>((1.001 / 60.0) * static_cast<double>(OS_TIME_SPEED));
    case 1:
    case 4:
        return static_cast<OSTime>((1.0 / 60.0) * static_cast<double>(OS_TIME_SPEED));
    }
}

void InitializePad(OSTime sampleInterval, int channels) {
    PADInit();
    WPADRegisterAllocator(AllocatePadMemory, FreePadMemory);
    KPADInit();
    spPadData.reset(new PadData);
    spPadData->mChannelCount = channels;
    for (int channel = 0; channel < 4; ++channel) {
        KPADSetPosParam(channel, 0.02f, 0.6f);
        KPADSetHoriParam(channel, 0.02f, 0.6f);
        KPADSetDistParam(channel, 0.02f, 0.6f);
        KPADSetAccParam(channel, 0.0f, 1.0f);
        KPADEnableAimingMode(channel);
        KPADSetConnectCallback(channel, PadConnectionCallback);
    }
    OSCreateAlarm(&spPadData->mAlarm);
    OSTime start = OSGetTime() + sampleInterval;
    OSSetPeriodicAlarm(&spPadData->mAlarm, start, sampleInterval, SamplePad);
    spPadData->mShutdownCallbackId = RegisterResetCallback(ShutdownPad, 0);
}

void SetPadReadDelay(int frames) {
    InterruptLock lock;
    int available = (spPadData->mWriteIndex - spPadData->mReadIndex + 60) % 60;
    if (available < frames) {
        frames = available;
    }
    spPadData->mReadIndex = (spPadData->mWriteIndex - frames + 60) % 60;
}

void UpdatePad() {
    InterruptLock lock;
    if ((spPadData->mWriteIndex - spPadData->mReadIndex + 60) % 60 == 0) {
        for (int channel = 0; channel < 4; ++channel) {
            spPadData->mWiiPublic[channel].trig = 0;
            spPadData->mWiiPublic[channel].release = 0;
            spPadData->mWiiPublic[channel].acc_speed = 0.0f;
            spPadData->mWiiPublic[channel].vec.x = 0.0f;
            spPadData->mWiiPublic[channel].vec.y = 0.0f;
            spPadData->mWiiPublic[channel].speed = 0.0f;
            spPadData->mWiiPublic[channel].hori_speed = 0.0f;
            spPadData->mWiiPublic[channel].dist_vec = 0.0f;
            spPadData->mWiiPublic[channel].dist_speed = 0.0f;
            spPadData->mWiiPublic[channel].mRepeat = 0;
            spPadData->mWiiPublic[channel].mTrigger = 0;
            spPadData->mWiiPublic[channel].mRelease = 0;
            spPadData->mWiiPublic[channel].mButtonRepeat = 0;
            spPadData->mWiiPublic[channel].mRepeatButtons = 0;
            spPadData->mWiiPublic[channel].mRepeatDelay = 0;
            spPadData->mWiiPublic[channel].mProcessingState = 0;
            spPadData->mWiiPublic[channel].mStablePosition.x = 0.0f;
            spPadData->mWiiPublic[channel].mStablePosition.y = 0.0f;
            spPadData->mWiiPublic[channel].mPointerIdleFrames = 0;
            spPadData->mWiiPublic[channel].mIdleFrames = 0;
            spPadData->mWiiPublic[channel].mNunchukIdleFrames = 0;
            spPadData->mWiiPublic[channel].mShake = 0;
            spPadData->mWiiPublic[channel].mShakeTrigger = 0;
            spPadData->mWiiPublic[channel].mProbeResult = -1;
            spPadData->mWiiPublic[channel].mDeviceType = 253;
        }
        memset(spPadData->mGCPublic, 0, sizeof(spPadData->mGCPublic));
        memset(spPadData->mGCWorking, 0, sizeof(spPadData->mGCWorking));
    } else {
        for (int channel = 0; channel < 4; ++channel) {
            WiiPadState& rPad = spPadData->mWiiWorking[channel];
            rPad.Update(&spPadData->mWiiHistory[spPadData->mReadIndex][channel], spPadData->mActivityMask[channel]);
            unsigned int previousShake = rPad.mShake;
            rPad.mShake = DetectWiiShake(channel);
            rPad.mShakeTrigger = rPad.mShake & (previousShake ^ rPad.mShake);
            rPad.mProbeResult = spPadData->mProbeHistory[spPadData->mReadIndex][channel];
            rPad.mDeviceType = spPadData->mDeviceHistory[spPadData->mReadIndex][channel];
            spPadData->mWiiPublic[channel] = rPad;
        }
        for (int channel = 0; channel < 4; ++channel) {
            spPadData->mGCWorking[channel].Update(&spPadData->mGCRawHistory[spPadData->mReadIndex][channel]);
            spPadData->mGCPublic[channel] = spPadData->mGCWorking[channel];
        }
        spPadData->mReadIndex = (spPadData->mReadIndex + 1) % 60;
    }
    for (int channel = 0; channel < 4; ++channel) {
        const WiiPadState& rPad = spPadData->mWiiWorking[channel];
        if (channel == 0) {
            spPadData->mMinPointerIdleFrames = rPad.mPointerIdleFrames;
            spPadData->mMinButtonIdleFrames = rPad.mIdleFrames;
            spPadData->mMinNunchukIdleFrames = rPad.mNunchukIdleFrames;
            spPadData->mMinInputIdleFrames = spPadData->mMinPointerIdleFrames;
        } else {
            spPadData->mMinPointerIdleFrames = std::min(spPadData->mMinPointerIdleFrames, rPad.mPointerIdleFrames);
            spPadData->mMinButtonIdleFrames = std::min(spPadData->mMinButtonIdleFrames, rPad.mIdleFrames);
            spPadData->mMinNunchukIdleFrames = std::min(spPadData->mMinNunchukIdleFrames, rPad.mNunchukIdleFrames);
            spPadData->mMinInputIdleFrames = std::min(spPadData->mMinInputIdleFrames, spPadData->mMinPointerIdleFrames);
        }
        spPadData->mMinInputIdleFrames = std::min(spPadData->mMinInputIdleFrames, spPadData->mMinButtonIdleFrames);
        spPadData->mMinInputIdleFrames = std::min(spPadData->mMinInputIdleFrames, spPadData->mMinNunchukIdleFrames);
    }
}

WiiPadState* GetWiiPad(int channel) {
    return &spPadData->mWiiPublic[channel];
}

int GetWiiPadDevice(int channel, unsigned int* pDeviceType) {
    WiiPadState& rPad = spPadData->mWiiPublic[channel];
    if (pDeviceType != nullptr) {
        *pDeviceType = rPad.mDeviceType;
    }
    return rPad.mProbeResult;
}

GCPadState* GetGCPad(int channel) {
    return &spPadData->mGCPublic[channel];
}

void StartWiiRumble(int channel, const RumbleStep* pPattern) {
    spPadData->mWiiRumble[channel].StartPattern(pPattern);
}

void BRumble::AlarmCallback(OSAlarm* pAlarm, OSContext*) {
    InterruptLock lock;
    BRumble* pRumble = static_cast<BRumble*>(OSGetAlarmUserData(pAlarm));
    pRumble->UpdatePattern();
}

bool BRumble::CanRumble() const { return mEnabled && mBlockCount == 0; }

void StopWiiRumble(int channel) {
    spPadData->mWiiRumble[channel].StopPattern();
}

void StartGCRumble(int channel, const RumbleStep* pPattern) {
    spPadData->mGCRumble[channel].StartPattern(pPattern);
}

void StopGCRumble(int channel) {
    spPadData->mGCRumble[channel].StopPattern();
}

void SetGCRumbleEnabled(bool enabled) {
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mGCRumble[channel].SetEnabled(enabled);
    }
}

CDisablePadRumble::CDisablePadRumble() {
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mWiiRumble[channel].Block();
    }
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mGCRumble[channel].Block();
    }
}

CDisablePadRumble::~CDisablePadRumble() {
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mWiiRumble[channel].Unblock();
    }
    for (int channel = 0; channel < 4; ++channel) {
        spPadData->mGCRumble[channel].Unblock();
    }
}

void SetPadConnectionCallback(int channel, WPADCallback callback) {
    spPadData->mConnectCallbacks[channel] = callback;
}
