#include <game/world/BWorldObj.h>
#include <game/script/NPpuVM.h>

BWorldObj::BWorldObj(u32 type)
    : mState(4), mType(type | TYPE_WORLD_OBJ), mWorld(nullptr), mId(0),
      mRotation(0.0f), mPosition(0.0f, 0.0f, 0.0f), mScale(1.0f, 1.0f, 1.0f) {}

BWorldObj::~BWorldObj() {}

void BWorldObj::SetParent(std::tr1::shared_ptr<BWorldObj> parent) {
    mParent = parent;
}

BWorldObj* BWorldObj::GetParent() {
    return Lock(mParent).get();
}

std::tr1::shared_ptr<BWorldObj> Lock(std::tr1::weak_ptr<BWorldObj> obj) {
    std::tr1::shared_ptr<BWorldObj> owner = obj.lock();
    return owner && owner->IsInWorld() ? owner : std::tr1::shared_ptr<BWorldObj>();
}

NPpu::CValue BWorldObj::GetRegister(u32 index) {
    return mRegister.GetValue(index);
}

void BWorldObj::SetRegister(u32 index, const NPpu::CValue& rValue) {
    mRegister.Set(index, rValue);
}

void BWorldObj::SwapRegister(CPpuRegister* pRegister) {
    mRegister.Swap(pRegister);
}

void BWorldObj::Pause() {
    u32 flags = mState;
    if (flags & 0x18) return;
    if (flags & 1) mState = flags | 0x30;
    else mState = flags | 0x10;
    SetVisible(false);
}

void BWorldObj::Resume() {
    if (mState & 0x10) {
        SetVisible((mState & 0x20) != 0);
        mState &= ~0x30;
    }
}
