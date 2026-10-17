#ifndef NW4R_SND_AX_MANAGER_H
#define NW4R_SND_AX_MANAGER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BiquadFilterPreset.h>
#include <nw4r/snd/snd_FxBase.h>
#include <nw4r/snd/snd_MoveValue.h>
#include <nw4r/snd/snd_Types.h>

#include <nw4r/ut/ut_LinkList.h>

#include <revolution/ai.h>
#include <revolution/ax.h>

namespace nw4r {
namespace snd {
namespace detail {

class AxManager {
public:
    struct CallbackListNode {
        NW4R_UT_LINKLIST_NODE_DECL();
        AXOutCallback callback;
    };

    NW4R_UT_LINKLIST_TYPEDEF_DECL(CallbackListNode);

public:
    static AxManager& GetInstance();

    void Init();
    void Shutdown();
    void Update();
    void PrepareReset();

    bool CheckInit() {
        return mInitialized;
    }

    bool IsResetReady() const {
        return mResetReadyCounter == 0;
    }

    f32 GetOutputVolume() const;
    void* GetZeroBufferAddress();

    void RegisterCallback(CallbackListNode* pNode, AXOutCallback pCallback);
    void UnregisterCallback(CallbackListNode* pNode);

    void SetOutputMode(OutputMode mode);
    OutputMode GetOutputMode();

    f32 GetMasterVolume() const {
        return mMasterVolume.GetValue();
    }
    void SetMasterVolume(f32 volume, int frame);

    bool AppendEffect(AuxBus bus, FxBase* pFx);
    void ClearEffect(AuxBus bus, int frame);
    void ShutdownEffect(AuxBus bus);

    FxBaseList& GetEffectList(AuxBus bus) {
        return mFxList[bus];
    }

    const BiquadFilterCallback* GetBiquadFilterCallback(int type) {
        return sBiquadFilterCallbackTable[static_cast<u8>(type)];
    }
    void SetBiquadFilterCallback(int type,
                                 const BiquadFilterCallback* pCallback);

private:
    static const u8 AUX_CALLBACK_WAIT_FRAME = 6;
    static const u16 AUX_RETURN_VOLUME_MAX = AX_MAX_VOLUME;

    static const int FX_SAMPLE_RATE = AX_SAMPLE_RATE;
    static const SampleFormat FX_SAMPLE_FORMAT = SAMPLE_FORMAT_PCM_S32;
    static const int FX_BUFFER_SIZE = AX_FRAME_SIZE;

    static const int ZERO_BUFFER_SIZE = 256;
    static const int BIQUAD_FILTER_CALLBACK_MAX = 128;

private:
    AxManager();

    static void AxCallbackFunc();
    static void AuxCallbackFunc(void* pChans, void* pContext);
    static void AiDmaCallbackFunc();

private:
    OutputMode mOutputMode;
    void* mZeroBufferAddress;
    CallbackListNodeList mCallbackList;
    AXOutCallback mNextAxRegisterCallback;
    bool mInitialized;
    bool mUpdateVoicePrioFlag;
    MoveValue<f32, int> mMasterVolume;
    MoveValue<f32, int> mMainOutVolume;
    MoveValue<f32, int> mVolumeForReset;
    AIDCallback mOldAidCallback;
    volatile s32 mResetReadyCounter;
    MoveValue<f32, int> mAuxFadeVolume[AUX_BUS_NUM];
    MoveValue<f32, int> mAuxUserVolume[AUX_BUS_NUM];
    FxBaseList mFxList[AUX_BUS_NUM];
    AXAuxCallback mAuxCallback[AUX_BUS_NUM];
    void* mAuxCallbackContext[AUX_BUS_NUM];
    u8 mAuxCallbackWaitCounter[AUX_BUS_NUM];
    u32 mEffectProcessTick[AUX_BUS_NUM];

    static u8 sZeroBuffer[ZERO_BUFFER_SIZE];
    static const BiquadFilterCallback*
        sBiquadFilterCallbackTable[BIQUAD_FILTER_CALLBACK_MAX];
    static BiquadFilterLpf sBiquadFilterLpf;
    static BiquadFilterHpf sBiquadFilterHpf;
    static BiquadFilterBpf512 sBiquadFilterBpf512;
    static BiquadFilterBpf1024 sBiquadFilterBpf1024;
    static BiquadFilterBpf2048 sBiquadFilterBpf2048;
};

}
}
}

#endif
