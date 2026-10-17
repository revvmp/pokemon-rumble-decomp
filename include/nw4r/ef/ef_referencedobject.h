#ifndef NW4R_EF_REFERENCED_OBJECT_H
#define NW4R_EF_REFERENCED_OBJECT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_linkedobject.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class ReferencedObject : public LinkedObject {
public:
    enum LifeStatus {
        NW4R_EF_LS_CLOSED,
        NW4R_EF_LS_ACTIVE,
        NW4R_EF_LS_WAIT,
        NW4R_EF_LS_CLOSING,
    };

protected:
    LifeStatus mLifeStatus;
    u32 mRefCount;

public:
    ut::Link mActivityLink;

public:
    virtual void SendClosing() {}
    virtual void DestroyFunc() {}

    void ChangeLifeStatus(LifeStatus status) {
        mLifeStatus = status;
    }
    LifeStatus GetLifeStatus() const {
        return mLifeStatus;
    }

    u32 GetRefCount() const {
        return mRefCount;
    }

    bool Initialize() {
        mRefCount = 0;
        mLifeStatus = NW4R_EF_LS_ACTIVE;
        return true;
    }
    void Destroy() {
        DestroyFunc();
        mLifeStatus = NW4R_EF_LS_WAIT;

        if (mRefCount == 0) {
            SendClosing();
        }
    }

    u32 Ref() {
        return mRefCount++;
    }
    u32 UnRef() {
        if (--mRefCount == 0 && mLifeStatus == NW4R_EF_LS_WAIT) {
            SendClosing();
        }

        return mRefCount;
    }
};

}
}

#endif
