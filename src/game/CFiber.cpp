#include <game/CFiber.h>

#include <game/Memory.h>
#include <nw4r/g3d/platform/g3d_cpu.h>

CFiber* CFiber::sCurrent;

void CFiber::Initialize() throw() {
    mFlags = 0;
    OSInitMutex(&mMutex);
    OSInitCond(&mResumeCond);
    OSInitCond(&mSuspendCond);
}

CFiber::~CFiber() throw() {
    Abort();
}

void CFiber::Start(const std::tr1::function<void()>& rBody, u32 stackSize) throw() {
    if (IsStarted()) {
        return;
    }

    u8* pStack = static_cast<u8*>(GameMemory::Allocate(stackSize));
    mStack.reset(pStack, GameMemory::Free);

    StartParam param = {this, &rBody};
    OSCreateThread(&mThread, ThreadMain, &param, mStack.get() + stackSize, stackSize, 16, 0);

    while (!IsStarted()) {
        Resume();
    }
}

void CFiber::Resume() throw() {
    if (IsFinished()) {
        return;
    }

    CFiber* pPrevious = sCurrent;
    sCurrent = this;
    mFlags |= FLAG_RUNNING;
    OSSignalCond(&mResumeCond);
    OSLockMutex(&mMutex);
    OSResumeThread(&mThread);
    OSWaitCond(&mSuspendCond, &mMutex);

    bool suspended = false;
    if (!OSIsThreadTerminated(&mThread)) {
        OSSuspendThread(&mThread);
        suspended = true;
    }
    OSUnlockMutex(&mMutex);

    mFlags &= ~FLAG_RUNNING;
    sCurrent = pPrevious;

    if (IsFinished() && mStack.get() != nullptr) {
        if (suspended) {
            OSResumeThread(&mThread);
        }
        OSJoinThread(&mThread, nullptr);
        mStack.reset();
    }
}

inline void CFiber::Terminate(int mode) throw(Terminated) {
    mFlags |= FLAG_STARTED | FLAG_TERMINATING;
    if (sCurrent == this) {
        throw Terminated();
    }

    if (mode == WAIT_YIELD) {
        while (!IsFinished()) {
            YieldFiber();
        }
    } else {
        while (!IsFinished()) {
            Resume();
        }
    }
}

void TerminateFiber(int mode) throw(CFiber::Terminated) {
    CFiber::GetCurrent()->Terminate(mode);
}

void YieldFiber() {
    CFiber* pFiber = CFiber::sCurrent;
    if (pFiber->mOnSuspend) {
        pFiber->mOnSuspend();
    }
    OSSignalCond(&CFiber::sCurrent->mSuspendCond);
    OSWaitCond(&CFiber::sCurrent->mResumeCond, &CFiber::sCurrent->mMutex);
    if (CFiber::sCurrent->mFlags & CFiber::FLAG_TERMINATING) {
        throw CFiber::Terminated();
    }
    pFiber = CFiber::sCurrent;
    if (pFiber->mOnResume) {
        pFiber->mOnResume();
    }
}

void YieldFiberStarted() {
    CFiber::GetCurrent()->SetStarted();
    YieldFiber();
}

void* CFiber::ThreadMain(void* pParam) throw() {
    nw4r::g3d::fastcast::detail::Init();

    StartParam* pStart = static_cast<StartParam*>(pParam);
    OSLockMutex(&pStart->mpFiber->mMutex);
    std::tr1::function<void()> body(*pStart->mpBody);
    try {
        body();
    } catch (Terminated&) {
    }

    sCurrent->mFlags |= FLAG_STARTED | FLAG_FINISHED | FLAG_TERMINATING;
    OSSignalCond(&sCurrent->mSuspendCond);
    OSUnlockMutex(&sCurrent->mMutex);
    return nullptr;
}
