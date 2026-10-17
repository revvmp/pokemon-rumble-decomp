#include <nw4r/ef/ef_handle.h>

namespace nw4r {
namespace ef {

HandleBase::HandleBase() : mObjectID(0), mObject(nullptr) {}

HandleBase& HandleBase::operator=(const HandleBase& rOther) {
    mObjectID = rOther.mObjectID;
    mObject = rOther.mObject;

    return *this;
}

HandleBase& HandleBase::operator=(LinkedObject* pObject) {
    if (pObject == nullptr) {
        mObjectID = 0;
        mObject = nullptr;
    } else {
        mObjectID = pObject->mObjectID;
        mObject = pObject;
    }

    return *this;
}

bool HandleBase::IsValid() const {
    if (mObject == nullptr) {
        return false;
    }

    return mObjectID == mObject->mObjectID;
}

LinkedObject* HandleBase::GetPtr() const {
    return IsValid() ? mObject : nullptr;
}

}
}
