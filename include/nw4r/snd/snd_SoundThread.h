#ifndef NW4R_SND_SOUND_THREAD_H
#define NW4R_SND_SOUND_THREAD_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_AxManager.h>

#include <nw4r/ut/ut_LinkList.h>
#include <nw4r/ut/ut_NonCopyable.h>

#include <revolution/os/OSMessage.h>
#include <revolution/os/OSMutex.h>
#include <revolution/os/OSThread.h>

namespace nw4r {
namespace snd {
namespace detail {

class SoundThread {
public:
    class SoundFrameCallback {
    public:
        NW4R_UT_LINKLIST_NODE_DECL();

        virtual ~SoundFrameCallback() {}
        virtual void OnBeginSoundFrame() {}
        virtual void OnEndSoundFrame() {}
    };

    NW4R_UT_LINKLIST_TYPEDEF_DECL(SoundFrameCallback);

    class PlayerCallback {
    public:
        NW4R_UT_LINKLIST_NODE_DECL();

        virtual ~PlayerCallback() {}
        virtual void OnUpdateFrameSoundThread() {}
        virtual void OnUpdateVoiceSoundThread() {}
        virtual void OnShutdownSoundThread() {}
    };

    NW4R_UT_LINKLIST_TYPEDEF_DECL(PlayerCallback);

    class AutoLock : private ut::NonCopyable {
    public:
        AutoLock() {
            SoundThread::GetInstance().Lock();
        }

        ~AutoLock() {
            SoundThread::GetInstance().Unlock();
        }
    };

public:
    static SoundThread& GetInstance();

    bool Create(s32 priority, void* pStack, u32 stackSize);
    void Shutdown();

    void RegisterPlayerCallback(PlayerCallback* pCallback);
    void UnregisterPlayerCallback(PlayerCallback* pCallback);

    void Lock() {
        OSLockMutex(&mMutex);
    }
    void Unlock() {
        OSUnlockMutex(&mMutex);
    }

private:
    enum ThreadMessage {
        MSG_NONE,
        MSG_AX_CALLBACK,
        MSG_SHUTDOWN,
    };

    static const int MSG_QUEUE_CAPACITY = 4;

private:
    SoundThread();

    static void AxCallbackFunc();
    void AxCallbackProc();

    static void* SoundThreadFunc(void* pArg);
    void SoundThreadProc();
    void FrameProcess();

private:
    OSThread mThread;
    OSThreadQueue mThreadQueue;
    OSMessageQueue mMsgQueue;
    OSMessage mMsgBuffer[MSG_QUEUE_CAPACITY];
    void* mStackEnd;
    OSMutex mMutex;

    AxManager::CallbackListNode mAxCallbackNode;
    SoundFrameCallbackList mSoundFrameCallbackList;
    PlayerCallbackList mPlayerCallbackList;

    u32 mProcessTick;
    bool mCreateFlag;
    bool mPauseFlag;
};

}
}
}

#endif
