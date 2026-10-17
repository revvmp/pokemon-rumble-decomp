#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_FxChorus.h>

namespace nw4r {
namespace snd {

FxChorus::FxChorus() : mIsActive(false) {
    detail::FxChorusParam param;
    SetParam(param);
}

u32 FxChorus::GetRequiredMemSize() {
    u32 memSize = AXFXChorusExpGetMemSize(&mAxfxParam);
    return ut::RoundUp<u32>(memSize + detail::AxfxImpl::HEAP_SIZE_MIN, 32);
}

bool FxChorus::AssignWorkBuffer(void* pBuffer, u32 size) {
    return mImpl.CreateHeap(pBuffer, size);
}

void FxChorus::ReleaseWorkBuffer() {
    mImpl.DestroyHeap();
}

bool FxChorus::StartUp() {
    u32 memSize = GetRequiredMemSize();
    if (memSize > mImpl.GetHeapTotalSize()) {
        return false;
    }

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    BOOL success = AXFXChorusExpInit(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);

    AXFXChorusExpGetMemSize(&mAxfxParam);
    mIsActive = true;

    return success;
}

void FxChorus::Shutdown() {
    if (!mIsActive) {
        return;
    }

    mIsActive = false;

    AXFXAllocHook allocHook;
    AXFXFreeHook freeHook;
    mImpl.HookAlloc(&allocHook, &freeHook);
    AXFXChorusExpShutdown(&mAxfxParam);
    mImpl.RestoreAlloc(allocHook, freeHook);
}

bool FxChorus::SetParam(const detail::FxChorusParam& rParam) {
    mParam = rParam;

    mAxfxParam.delayTime = ut::Clamp(rParam.delayTime, 0.1f, 50.0f);
    mAxfxParam.depth = ut::Clamp(rParam.depth, 0.0f, 1.0f);
    mAxfxParam.rate = ut::Clamp(rParam.rate, 0.1f, 2.0f);
    mAxfxParam.feedback = ut::Clamp(rParam.feedback, 0.0f, 0.99f);
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

    BOOL success = AXFXChorusExpSettingsUpdate(&mAxfxParam);
    return success;
}

void FxChorus::UpdateBuffer(int, void** ppBuffer, u32, SampleFormat, f32,
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

    AXFXChorusExpCallback(&bus, &mAxfxParam);
}

}
}
