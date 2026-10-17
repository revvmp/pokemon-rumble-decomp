#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_FxDelay.h>

namespace nw4r {
namespace snd {

FxDelay::FxDelay() : mIsActive(false) {
    detail::FxDelayParam param;
    SetParam(param);
}

u32 FxDelay::GetRequiredMemSize() {
    u32 memSize = AXFXDelayExpGetMemSize(&mAxfxParam);
    return ut::RoundUp<u32>(memSize + detail::AxfxImpl::HEAP_SIZE_MIN, 32);
}

bool FxDelay::AssignWorkBuffer(void* pBuffer, u32 size) {
    return mImpl.CreateHeap(pBuffer, size);
}

void FxDelay::ReleaseWorkBuffer() {
    mImpl.DestroyHeap();
}

bool FxDelay::StartUp() {
    u32 memSize = GetRequiredMemSize();
    if (memSize > mImpl.GetHeapTotalSize()) {
        return false;
    }

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    BOOL success = AXFXDelayExpInit(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);

    AXFXDelayExpGetMemSize(&mAxfxParam);
    mIsActive = true;

    return success;
}

void FxDelay::Shutdown() {
    if (!mIsActive) {
        return;
    }

    mIsActive = false;

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    AXFXDelayExpShutdown(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);
}

bool FxDelay::SetParam(const detail::FxDelayParam& rParam) {
    mParam = rParam;

    f32 maxDelay = ut::Max(rParam.maxDelay, 0.04f);
    bool needReset = maxDelay != mAxfxParam.maxDelay;

    mAxfxParam.maxDelay = maxDelay;
    mAxfxParam.delay = ut::Clamp(rParam.delay, 0.04f, rParam.maxDelay);
    mAxfxParam.feedback = ut::Clamp(rParam.feedback, 0.0f, 0.99f);
    mAxfxParam.lpf = ut::Clamp(rParam.lpf, 0.0f, 1.0f);
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
        success = AXFXDelayExpSettings(&mAxfxParam);
        mImpl.RestoreAlloc(allocHook, freeHook);
    } else {
        success = AXFXDelayExpSettingsUpdate(&mAxfxParam);
    }

    return success;
}

void FxDelay::UpdateBuffer(int, void** ppBuffer, u32, SampleFormat, f32,
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

    AXFXDelayExpCallback(&bus, &mAxfxParam);
}

}
}
