#ifndef NW4R_SND_BASIC_SOUND_H
#define NW4R_SND_BASIC_SOUND_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_Types.h>

#include <nw4r/ut/ut_LinkList.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>
#include <nw4r/ut/ut_algorithm.h>

#include <revolution/wpad.h>

namespace nw4r {
namespace snd {

class SoundActor;
class SoundHandle;
class SoundPlayer;

struct SoundAmbientParam {
    SoundAmbientParam()
        : volume(1.0f),
          pitch(1.0f),
          pan(0.0f),
          surroundPan(0.0f),
          fxSend(0.0f),
          lpf(0.0f),
          biquadFilterValue(0.0f),
          biquadFilterType(0),
          priority(0),
          userData(0) {}

    f32 volume;
    f32 pitch;
    f32 pan;
    f32 surroundPan;
    f32 fxSend;
    f32 lpf;
    f32 biquadFilterValue;
    int biquadFilterType;
    int priority;
    u32 userData;
    VoiceOutParam voiceOutParam[WPAD_MAX_CONTROLLERS];
};

namespace detail {

class BasicPlayer;
class ExternalSoundPlayer;
class PlayerHeap;

struct SoundActorParam {
    SoundActorParam() : volume(1.0f), pitch(1.0f), pan(0.0f) {}

    f32 volume;
    f32 pitch;
    f32 pan;
};

class BasicSound {
    friend class nw4r::snd::SoundHandle;

public:
    NW4R_UT_RTTI_DECL(BasicSound);

    enum PauseState {
        PAUSE_STATE_NORMAL,
        PAUSE_STATE_PAUSING,
        PAUSE_STATE_PAUSED,
        PAUSE_STATE_UNPAUSING
    };

    class AmbientParamUpdateCallback {
    public:
        virtual ~AmbientParamUpdateCallback() {}

        virtual void detail_UpdateAmbientParam(const void* pArg, u32 id,
                                               int voiceOutCount,
                                               SoundAmbientParam* pParam) = 0;
        virtual int detail_GetAmbientPriority(const void* pArg, u32 id) = 0;
        virtual int detail_GetRequiredVoiceOutCount(const void* pArg,
                                                    u32 id) = 0;
    };

    class AmbientArgUpdateCallback {
    public:
        virtual ~AmbientArgUpdateCallback() {}

        virtual void detail_UpdateAmbientArg(void* pArg,
                                             const BasicSound* pSound) = 0;
    };

    class AmbientArgAllocaterCallback {
    public:
        virtual ~AmbientArgAllocaterCallback() {}

        virtual void* detail_AllocAmbientArg(u32 size) = 0;
        virtual void detail_FreeAmbientArg(void* pArg,
                                           const BasicSound* pSound) = 0;
    };

    struct AmbientInfo {
        AmbientParamUpdateCallback* paramUpdateCallback;
        AmbientArgUpdateCallback* argUpdateCallback;
        AmbientArgAllocaterCallback* argAllocaterCallback;
        void* arg;
        u32 argSize;
    };

    static const u32 INVALID_ID = 0xFFFFFFFF;
    static const int PRIORITY_MIN = 0;
    static const int PRIORITY_MAX = 127;

public:
    BasicSound(int priority, int ambientPriority);
    virtual ~BasicSound() {}

    virtual void Shutdown();
    virtual bool IsPrepared() const = 0;
    virtual bool IsAttachedTempSpecialHandle() = 0;
    virtual void DetachTempSpecialHandle() = 0;
    virtual void InitParam();
    virtual BasicPlayer& GetBasicPlayer() = 0;
    virtual const BasicPlayer& GetBasicPlayer() const = 0;
    virtual void OnUpdatePlayerPriority() {}
    virtual void UpdateMoveValue();
    virtual void UpdateParam();

    void StartPrepared();
    void Update();
    void Stop(int frames);
    void Pause(bool flag, int frames);
    bool IsPause() const;
    void SetAutoStopCounter(int count);
    void FadeIn(int frames);

    bool GetStartedFlag() const {
        return mStartedFlag;
    }

    PlayerHeap* GetPlayerHeap() {
        return mPlayerHeap;
    }
    void AttachPlayerHeap(PlayerHeap* pHeap);
    void DetachPlayerHeap(PlayerHeap* pHeap);

    SoundPlayer* GetSoundPlayer() {
        return mSoundPlayer;
    }
    void AttachSoundPlayer(SoundPlayer* pPlayer);
    void DetachSoundPlayer(SoundPlayer* pPlayer);

    void AttachSoundActor(SoundActor* pActor);
    void DetachSoundActor(SoundActor* pActor);

    void AttachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer);
    void DetachExternalSoundPlayer(ExternalSoundPlayer* pExtPlayer);

    bool IsAttachedGeneralHandle();
    void DetachGeneralHandle();

    bool IsAttachedTempGeneralHandle();
    void DetachTempGeneralHandle();

    int GetRemainingFadeFrames() const;
    int GetVoiceOutCount() const;

    int GetPriority() const {
        return mPriority;
    }
    void SetPlayerPriority(int priority);

    int CalcCurrentPlayerPriority() const {
        return ut::Clamp(mPriority + mAmbientParam.priority, PRIORITY_MIN,
                         PRIORITY_MAX);
    }

    void SetInitialVolume(f32 volume);
    void SetVolume(f32 volume, int frames);
    void SetPitch(f32 pitch);
    void SetPan(f32 pan);
    void SetSurroundPan(f32 pan);
    void SetLpfFreq(f32 freq);
    void SetOutputLineFlag(int flag);
    void SetFxSend(AuxBus bus, f32 send);
    void SetRemoteFilter(int filter);
    void SetPanMode(PanMode mode);
    void SetPanCurve(PanCurve curve);

    void SetAmbientInfo(const AmbientInfo& rInfo);
    void ClearAmbientArgUpdateCallback() {
        mAmbientInfo.argUpdateCallback = nullptr;
    }
    static int GetAmbientPriority(const AmbientInfo& rInfo, u32 id);

    u32 GetId() const {
        return mId;
    }
    void SetId(u32 id);

private:
    PlayerHeap* mPlayerHeap;
    SoundHandle* mGeneralHandle;
    SoundHandle* mTempGeneralHandle;
    SoundPlayer* mSoundPlayer;
    SoundActor* mSoundActor;
    ExternalSoundPlayer* mExtSoundPlayer;

    AmbientInfo mAmbientInfo;
    SoundParam mAmbientParam;
    SoundActorParam mActorParam;

    MoveValue<f32, int> mFadeVolume;
    MoveValue<f32, int> mPauseFadeVolume;

    bool mStartFlag;
    bool mStartedFlag;
    bool mAutoStopFlag;
    bool mFadeOutFlag;
    PauseState mPauseState;
    bool mUnPauseFlag;
    int mAutoStopCounter;
    u32 mUpdateCounter;

    u8 mPriority;
    u8 mVoiceOutCount;
    u8 mBiquadFilterType;
    u32 mId;

    MoveValue<f32, int> mExtMoveVolume;
    f32 mInitVolume;
    f32 mExtPan;
    f32 mExtSurroundPan;
    f32 mExtPitch;
    f32 mLpfFreq;
    f32 mBiquadFilterValue;
    int mOutputLineFlag;
    f32 mMainOutVolume;
    f32 mMainSend;
    f32 mFxSend[AUX_BUS_NUM];
    f32 mRemoteOutVolume[WPAD_MAX_CONTROLLERS];

public:
    NW4R_UT_LINKLIST_NODE_DECL_EX(Prio);
    NW4R_UT_LINKLIST_NODE_DECL_EX(PlayerPlay);
    NW4R_UT_LINKLIST_NODE_DECL_EX(PlayerPrio);
    NW4R_UT_LINKLIST_NODE_DECL_EX(ExtPlay);
};

NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, Prio);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, PlayerPlay);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, PlayerPrio);
NW4R_UT_LINKLIST_TYPEDEF_DECL_EX(BasicSound, ExtPlay);

}
}
}

#endif
