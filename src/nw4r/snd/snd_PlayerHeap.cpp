#include <nw4r/snd/snd_PlayerHeap.h>
#include <nw4r/snd/snd_DisposeCallbackManager.h>
#include <nw4r/snd/snd_SoundThread.h>
#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {
namespace detail {

PlayerHeap::PlayerHeap()
    : mSound(nullptr), mPlayer(nullptr), mStartAddress(nullptr), mEndAddress(nullptr), mAllocAddress(nullptr) {}

PlayerHeap::~PlayerHeap() {
    Destroy();
}

bool PlayerHeap::Create(void* pStartAddress, u32 size) {
    void* pEndAddress = ut::AddOffsetToPtr(pStartAddress, size);
    pStartAddress = ut::RoundUp(pStartAddress, 32);
    if (pStartAddress > pEndAddress) {
        return false;
    }
    mStartAddress = pStartAddress;
    mEndAddress = pEndAddress;
    mAllocAddress = mStartAddress;
    return true;
}

void PlayerHeap::Destroy() {
    Clear();
    mAllocAddress = nullptr;
}

void* PlayerHeap::Alloc(u32 size) {
    void* pEndAddress = ut::AddOffsetToPtr(mAllocAddress, size);
    if (pEndAddress > mEndAddress) {
        return nullptr;
    }
    void* pAllocAddress = mAllocAddress;
    mAllocAddress = ut::RoundUp(pEndAddress, 32);
    return pAllocAddress;
}

void PlayerHeap::Clear() {
    SoundThread::AutoLock lockForDispose;
    DisposeCallbackManager::GetInstance().Dispose(mStartAddress, ut::GetOffsetFromPtr(mStartAddress, mAllocAddress), nullptr);
    DisposeCallbackManager::GetInstance().DisposeWave(mStartAddress, ut::GetOffsetFromPtr(mStartAddress, mAllocAddress), nullptr);
    mAllocAddress = mStartAddress;
}

u32 PlayerHeap::GetFreeSize() const {
    return ut::GetOffsetFromPtr(mAllocAddress, mEndAddress);
}

void PlayerHeap::AttachSound(BasicSound* pSound) {
    mSound = pSound;
}

void PlayerHeap::DetachSound(BasicSound* pSound) {
    mSound = nullptr;
}

}
}
}
