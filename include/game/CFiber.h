#ifndef GAME_CFIBER_H
#define GAME_CFIBER_H

#include <functional>
#include <memory>
#include <revolution/os/OSMutex.h>
#include <revolution/os/OSThread.h>

class CFiber {
public:
    class Terminated {};

    enum Flag {
        FLAG_STARTED = 1 << 0,
        FLAG_FINISHED = 1 << 1,
        FLAG_TERMINATING = 1 << 2,
        FLAG_RUNNING = 1 << 3
    };

    enum WaitMode {
        WAIT_RESUME,
        WAIT_YIELD
    };

    CFiber() { Initialize(); }
    CFiber(const std::tr1::function<void()>& rBody, u32 stackSize) throw() {
        Initialize();
        Start(rBody, stackSize);
    }

    void Start(const std::tr1::function<void()>& rBody, u32 stackSize) throw();
    void Resume() throw();
    void Terminate(int mode) throw(Terminated);
    void Abort() throw() {
        mFlags |= FLAG_STARTED | FLAG_TERMINATING;
        if (sCurrent != this) {
            while (!IsFinished()) {
                Resume();
            }
        }
    }

    bool IsStarted() const { return mFlags & FLAG_STARTED; }
    bool IsFinished() const { return mFlags & FLAG_FINISHED; }
    void SetStarted() { mFlags |= FLAG_STARTED; }

    static CFiber* GetCurrent() { return sCurrent; }

private:
    friend void YieldFiber();

    struct StartParam {
        CFiber* mpFiber;
        const std::tr1::function<void()>* mpBody;
    };

    void Initialize() throw();
    static void* ThreadMain(void* pParam) throw();

    static CFiber* sCurrent;

    u32 mFlags;
    std::tr1::shared_ptr<u8> mStack;
    std::tr1::function<void()> mOnSuspend;
    std::tr1::function<void()> mOnResume;
    OSMutex mMutex;
    OSCond mResumeCond;
    OSCond mSuspendCond;
    OSThread mThread;

public:
    virtual ~CFiber() throw();
};

void YieldFiber();
void YieldFiberStarted();
void TerminateFiber(int mode) throw(CFiber::Terminated);

#endif
