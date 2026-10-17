#include <nw4r/ut/ut_algorithm.h>

#include <nw4r/snd/snd_AxfxImpl.h>

namespace nw4r {
namespace snd {
namespace detail {

AxfxImpl* AxfxImpl::mCurrentFx = nullptr;
u32 AxfxImpl::mAllocatedSize = 0;

bool AxfxImpl::CreateHeap(void* pBuffer, u32 size) {
    if (pBuffer == nullptr || size == 0) {
        mHeap = nullptr;
        return false;
    }

    mHeap = MEMCreateFrmHeap(pBuffer, size);
    return mHeap != nullptr;
}

void AxfxImpl::DestroyHeap() {
    if (mHeap != nullptr) {
        MEMDestroyFrmHeap(mHeap);
        mHeap = nullptr;
    }
}

void AxfxImpl::HookAlloc(AXFXAllocHook* pAllocHook, AXFXFreeHook* pFreeHook) {
    AXFXGetHooks(pAllocHook, pFreeHook);
    AXFXSetHooks(Alloc, Free);

    mCurrentFx = this;
    mAllocatedSize = 0;
}

u32 AxfxImpl::RestoreAlloc(AXFXAllocHook allocHook, AXFXFreeHook freeHook) {
    AXFXSetHooks(allocHook, freeHook);

    mCurrentFx = nullptr;
    return mAllocatedSize;
}

void* AxfxImpl::Alloc(u32 size) {
    void* pBlock = MEMAllocFromFrmHeap(mCurrentFx->mHeap, size);

    mCurrentFx->mAllocCount++;
    mAllocatedSize += ut::RoundUp<u32>(size, 4);

    return pBlock;
}

void AxfxImpl::Free(void*) {
    if (mCurrentFx->mAllocCount != 0) {
        mCurrentFx->mAllocCount--;
    }

    if (mCurrentFx->mAllocCount == 0) {
        MEMFreeToFrmHeap(mCurrentFx->mHeap, MEM_FRMHEAP_FREE_ALL);
    }
}

}
}
}
