#ifndef NW4R_UT_LOCK_H
#define NW4R_UT_LOCK_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_NonCopyable.h>

#include <revolution/os.h>
#include <revolution/os/OSMutex.h>

namespace nw4r {
namespace ut {
namespace detail {

inline void Lock(OSMutex& rMutex) {
    OSLockMutex(&rMutex);
}
inline void Unlock(OSMutex& rMutex) {
    OSUnlockMutex(&rMutex);
}

template <typename T> class AutoLock : private NonCopyable {
public:
    explicit AutoLock(T& rLockObj) : mLockObj(rLockObj) {
        Lock(mLockObj);
    }
    ~AutoLock() {
        Unlock(mLockObj);
    }

private:
    T& mLockObj;
};

}

class AutoInterruptLock : private NonCopyable {
public:
    AutoInterruptLock() : mOldState(OSDisableInterrupts()) {}
    ~AutoInterruptLock() {
        OSRestoreInterrupts(mOldState);
    }

private:
    BOOL mOldState;
};

}
}

#endif
