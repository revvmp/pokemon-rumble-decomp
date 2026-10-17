#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_FxReverbStd.h>

namespace nw4r {
namespace snd {

FxReverbStd::FxReverbStd() : mIsActive(false) {
    detail::FxReverbStdParam param;
    SetParam(param);
}

u32 FxReverbStd::GetRequiredMemSize() {
    u32 memSize = AXFXReverbStdExpGetMemSize(&mAxfxParam);
    return ut::RoundUp<u32>(memSize + detail::AxfxImpl::HEAP_SIZE_MIN, 32);
}

bool FxReverbStd::AssignWorkBuffer(void* pBuffer, u32 size) {
    return mImpl.CreateHeap(pBuffer, size);
}

void FxReverbStd::ReleaseWorkBuffer() {
    mImpl.DestroyHeap();
}

bool FxReverbStd::StartUp() {
    u32 memSize = GetRequiredMemSize();
    if (memSize > mImpl.GetHeapTotalSize()) {
        return false;
    }

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    BOOL success = AXFXReverbStdExpInit(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);

    AXFXReverbStdExpGetMemSize(&mAxfxParam);
    mIsActive = true;

    return success;
}

void FxReverbStd::Shutdown() {
    if (!mIsActive) {
        return;
    }

    mIsActive = false;

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    AXFXReverbStdExpShutdown(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);
}

bool FxReverbStd::SetParam(const detail::FxReverbStdParam& rParam) {
    mParam = rParam;

    f32 preDelayTimeMax = ut::Max(rParam.preDelayTimeMax, 0.0f);
    bool needReset = preDelayTimeMax != mAxfxParam.preDelayTimeMax;

    mAxfxParam.earlyMode = rParam.earlyMode;
    mAxfxParam.preDelayTimeMax = preDelayTimeMax;
    mAxfxParam.preDelayTime =
        ut::Clamp(rParam.preDelayTime, 0.0f, rParam.preDelayTimeMax);
    mAxfxParam.fusedMode = rParam.fusedMode;
    mAxfxParam.fusedTime = ut::Max(rParam.fusedTime, 0.0f);
    mAxfxParam.coloration = ut::Clamp(rParam.coloration, 0.0f, 1.0f);
    mAxfxParam.damping = ut::Clamp(rParam.damping, 0.0f, 1.0f);
    mAxfxParam.earlyGain = ut::Clamp(rParam.earlyGain, 0.0f, 1.0f);
    mAxfxParam.fusedGain = ut::Clamp(rParam.fusedGain, 0.0f, 1.0f);
    mAxfxParam.outGain = ut::Clamp(rParam.outGain, 0.0f, 1.0f);
    mAxfxParam.busIn = nullptr;
    mAxfxParam.busOut = nullptr;
    mAxfxParam.sendGain = 0.0f;

    if (!mIsActive) {
        return true;
    }

    u32 memSize = GetRequiredMemSize();
    if (memSize > mImpl.GetHeapTotalSize()) {
        return false;
    }

    BOOL success;
    if (needReset) {
        AXFXAllocHook allocHook;
        AXFXFreeHook freeHook;
        mImpl.HookAlloc(&allocHook, &freeHook);
        success = AXFXReverbStdExpSettings(&mAxfxParam);
        mImpl.RestoreAlloc(allocHook, freeHook);
    } else {
        success = AXFXReverbStdExpSettingsUpdate(&mAxfxParam);
    }

    return success;
}

void FxReverbStd::UpdateBuffer(int, void** ppBuffer, u32, SampleFormat, f32,
                         OutputMode mode) {
    if (!mIsActive) {
        return;
    }

    if (mode == OUTPUT_MODE_DPL2) {
        return;
    }

    AXFX_BUS bus;
    bus.left = static_cast<s32*>(ppBuffer[AX_STEREO_L]);
    bus.right = static_cast<s32*>(ppBuffer[AX_STEREO_R]);
    bus.surround = static_cast<s32*>(ppBuffer[AX_STEREO_S]);

    AXFXReverbStdExpCallback(&bus, &mAxfxParam);
}

}
}
