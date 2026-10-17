#include <nw4r/snd/snd_DisposeCallbackManager.h>

#include <nw4r/snd/snd_SoundThread.h>

#include <nw4r/ut/ut_Lock.h>

namespace nw4r {
namespace snd {
namespace detail {

DisposeCallbackManager& DisposeCallbackManager::GetInstance() {
    ut::AutoInterruptLock lock;
    static DisposeCallbackManager instance;
    return instance;
}

DisposeCallbackManager::DisposeCallbackManager() {}

void DisposeCallbackManager::RegisterDisposeCallback(
    DisposeCallback* pCallback) {
    mCallbackList.PushBack(pCallback);
}

void DisposeCallbackManager::UnregisterDisposeCallback(
    DisposeCallback* pCallback) {
    mCallbackList.Erase(pCallback);
}

void DisposeCallbackManager::Dispose(void* pData, u32 size, void*) {
    const void* pStart = pData;
    const void* pEnd = static_cast<u8*>(pData) + size;

    SoundThread::AutoLock lock;

    for (DisposeCallbackList::Iterator it =
             GetInstance().mCallbackList.GetBeginIter();
         it != GetInstance().mCallbackList.GetEndIter();) {
        DisposeCallbackList::Iterator curr = it++;
        curr->InvalidateData(pStart, pEnd);
    }
}

void DisposeCallbackManager::DisposeWave(void* pData, u32 size, void*) {
    const void* pStart = pData;
    const void* pEnd = static_cast<u8*>(pData) + size;

    SoundThread::AutoLock lock;

    for (DisposeCallbackList::Iterator it =
             GetInstance().mCallbackList.GetBeginIter();
         it != GetInstance().mCallbackList.GetEndIter();) {
        DisposeCallbackList::Iterator curr = it++;
        curr->InvalidateWaveData(pStart, pEnd);
    }
}

}
}
}
