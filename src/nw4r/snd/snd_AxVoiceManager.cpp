#include <nw4r/snd/snd_AxVoiceManager.h>

#include <nw4r/ut/ut_Lock.h>

#include <revolution/ax.h>

inline void* operator new(size_t, void* p) {
    return p;
}

namespace nw4r {
namespace snd {
namespace detail {

AxVoiceManager& AxVoiceManager::GetInstance() {
    static AxVoiceManager instance;
    return instance;
}

AxVoiceManager::AxVoiceManager() : mInitialized(false) {}

u32 AxVoiceManager::GetRequiredMemSize(int voiceCount) {
    return (voiceCount + VOICE_MARGIN) * sizeof(AxVoice);
}

void AxVoiceManager::Setup(void* pBuffer, u32 size) {
    if (mInitialized) {
        return;
    }

    mVoiceCount = size / sizeof(AxVoice);
    u8* pPtr = static_cast<u8*>(pBuffer);

    for (u32 i = 0; i < mVoiceCount; i++) {
        mFreeVoiceList.PushBack(new (pPtr) AxVoice);
        pPtr += sizeof(AxVoice);
    }

    mInitialized = true;
}

void AxVoiceManager::Shutdown() {
    if (!mInitialized) {
        return;
    }

    while (!mActiveVoiceList.IsEmpty()) {
        AxVoice& rVoice = mActiveVoiceList.GetFront();
        mActiveVoiceList.PopFront();

        if (rVoice.mVpb.IsAvailable()) {
            rVoice.Stop();

            if (rVoice.mCallback != nullptr) {
                rVoice.mCallback(&rVoice, AxVoice::CALLBACK_STATUS_CANCEL,
                                 rVoice.mCallbackData);
            }

            FreeAxVoice(&rVoice);
        }
    }

    while (!mFreeReservedVoiceList.IsEmpty()) {
        AxVoice& rVoice = mFreeReservedVoiceList.GetFront();
        mActiveVoiceList.PopFront();

        if (rVoice.mVpb.IsAvailable()) {
            rVoice.Stop();

            if (rVoice.mCallback != nullptr) {
                rVoice.mCallback(&rVoice, AxVoice::CALLBACK_STATUS_CANCEL,
                                 rVoice.mCallbackData);
            }

            FreeAxVoice(&rVoice);
        }
    }

    while (!mFreeVoiceList.IsEmpty()) {
        AxVoice& rVoice = mFreeVoiceList.GetFront();
        mFreeVoiceList.PopFront();
        rVoice.~AxVoice();
    }

    mInitialized = false;
}

AxVoice* AxVoiceManager::Alloc() {
    ut::AutoInterruptLock lock;

    FreeAllReservedAxVoice();

    if (mFreeVoiceList.IsEmpty()) {
        return nullptr;
    }

    AxVoice* pPtr = &mFreeVoiceList.GetFront();
    mFreeVoiceList.PopFront();

    AxVoice* pVoice = new (pPtr) AxVoice();
    mActiveVoiceList.PushBack(pPtr);

    return pVoice;
}

void AxVoiceManager::Free(AxVoice* pVoice) {
    pVoice->~AxVoice();

    ut::AutoInterruptLock lock;

    if (pVoice->mReserveForFreeFlag) {
        mFreeReservedVoiceList.Erase(pVoice);
    } else {
        mActiveVoiceList.Erase(pVoice);
    }

    mFreeVoiceList.PushBack(pVoice);
}

void AxVoiceManager::ReserveForFree(AxVoice* pVoice) {
    ut::AutoInterruptLock lock;

    mActiveVoiceList.Erase(pVoice);
    mFreeReservedVoiceList.PushBack(pVoice);
}

AxVoice* AxVoiceManager::AcquireAxVoice(u32 priority,
                                        AxVoice::AxVoiceCallback pCallback,
                                        void* pArg) {
    ut::AutoInterruptLock lock;

    AxVoice* pVoice = Alloc();
    if (pVoice == nullptr) {
        return nullptr;
    }

    AXVPB* pVpb = AXAcquireVoice(priority, AxVoice::VoiceCallback,
                                 reinterpret_cast<u32>(pVoice));
    if (pVpb == nullptr) {
        Free(pVoice);
        return nullptr;
    }

    pVoice->mVpb.Set(pVpb);
    pVoice->mCallback = pCallback;
    pVoice->mCallbackData = pArg;

    return pVoice;
}

void AxVoiceManager::FreeAxVoice(AxVoice* pVoice) {
    ut::AutoInterruptLock lock;

    if (pVoice->mVpb.IsAvailable()) {
        AXFreeVoice(pVoice->mVpb);
    }

    Free(pVoice);
}

void AxVoiceManager::ReserveForFreeAxVoice(AxVoice* pVoice) {
    ut::AutoInterruptLock lock;

    pVoice->mReserveForFreeFlag = true;
    ReserveForFree(pVoice);
}

void AxVoiceManager::FreeAllReservedAxVoice() {
    while (!mFreeReservedVoiceList.IsEmpty()) {
        AxVoice& rVoice = mFreeReservedVoiceList.GetFront();

        if (rVoice.mCallback != nullptr) {
            rVoice.mCallback(&rVoice, AxVoice::CALLBACK_STATUS_DROP_DSP,
                             rVoice.mCallbackData);
        }

        GetInstance().FreeAxVoice(&rVoice);
    }
}

}
}
}
