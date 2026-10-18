#include <game/BModelObj.h>

#include <game/CModel.h>
#include <nw4r/math/math_triangular.h>

BModelObj::BModelObj(u32 type) :
    BWorldObj(type | TYPE_MODEL_OBJ),
    mFlags(0),
    mModelScale(1.0f, 1.0f, 1.0f),
    mSize(0.0f, 0.0f, 0.0f),
    mVelocity(0.0f, 0.0f, 0.0f),
    mOutlineType(1) {}

BModelObj::~BModelObj() {}

void BModelObj::SetTransformDirect(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity) {
    mPosition = rPosition;
    mVelocity = rVelocity;
    mFlags |= FLAG_DIRTY_MTX;
    UpdateBounds();
}

void BModelObj::SetScaleDirect(const nw4r::math::VEC3& rScale) {
    mScale = rScale;
    mFlags |= FLAG_DIRTY_MTX;
    mSize = CalcSize((mFlags & FLAG_SIZE_MODE) != 0);
    UpdateBounds();
}

void BModelObj::SetBaseMtx(const nw4r::math::MTX34& rMtx) {
    mBaseMtx = rMtx;
    mFlags |= FLAG_BASE_MTX;
    SetTransform(nw4r::math::VEC3(rMtx._03, rMtx._13, rMtx._23), nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
}

void BModelObj::ClearBaseMtx() {
    mFlags &= ~FLAG_BASE_MTX;
    SetTransform(mPosition, nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
}

int BModelObj::GetMotionLength(const char* pName) {
    return mModel->GetMotionLength(pName);
}

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::Play(const char* pName, f32 frame, f32 rate) {
    return mModel->Play(pName, frame, rate);
}

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::PlayLoop(const char* pName, f32 frame, f32 rate) {
    return mModel->PlayLoop(pName, frame, rate);
}

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::PlayOnce(const char* pName, f32 frame, f32 rate) {
    return mModel->PlayOnce(pName, frame, rate);
}

void BModelObj::Pause(bool pause) {
    mModel->Pause(pause);
}

void BModelObj::Resume() {
    mModel->Pause(false);
}

void BModelObj::SetBoundingEnable(bool enable) {
    if (enable) {
        mFlags &= ~FLAG_NO_BOUNDING;
        CalcSize(0);
    } else {
        mFlags |= FLAG_NO_BOUNDING;
        if (mModel) {
            mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, nullptr);
        }
    }
}

void BModelObj::SetOutlineType(int type) {
    if (type != mOutlineType) {
        mOutlineType = type;
        UpdateOutline();
    }
}

nw4r::math::VEC3 BModelObj::GetSize() {
    return mSize;
}

DrawLayerSet* BModelObj::GetDrawLayers() {
    return &mDrawLayers;
}

void BModelObj::OnLeaveWorld() {
    Unlink();
    if (mModel) {
        mModel->GetScnObj()->DetachFromParent();
    }
}

void BModelObj::Calc() {
    if (mModel) {
        mModel->UpdateFrame(1.0f);
    }
    BSoundSource::SetPosition(mPosition);
}

void BModelObj::SetRotation(f32 rotation) {
    mRotation = rotation;
    mFlags |= FLAG_DIRTY_MTX;
}
