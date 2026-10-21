#include <game/BModelObj.h>

#include <game/AnmObjChrRootScale.h>
#include <game/CModel.h>
#include <game/Memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/math/math_triangular.h>

void SetSceneObjectEnabled(nw4r::g3d::ScnObj* pScnObj, bool enable);

BModelObj::BModelObj(u32 type) :
    BWorldObj(type | TYPE_MODEL_OBJ),
    mFlags(0),
    mModelScale(1.0f, 1.0f, 1.0f),
    mSize(0.0f, 0.0f, 0.0f),
    mVelocity(0.0f, 0.0f, 0.0f),
    mOutlineType(1) {}


BModelObj::~BModelObj() {}

bool BModelObj::IsOutlineModel(const std::tr1::shared_ptr<CModel>& rModel) {
    nw4r::g3d::ResNode node = static_cast<nw4r::g3d::ScnMdlSimple*>(rModel->GetScnObj())->GetResMdl().GetResNode(0);
    if (node.IsValid()) {
        nw4r::g3d::ResUserData userData = node.GetResUserData();
        if (userData.IsValid()) {
            nw4r::g3d::ResUserDataEntry entry = userData.GetEntry("OUTLINED");
            if (entry.IsValid() && entry.GetType() == nw4r::g3d::ResUserDataEntry::TYPE_INT && entry.GetInt() == 0) {
                return false;
            }
        }
    }

    nw4r::g3d::ScnMdl* pScnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(rModel->GetScnObj());
    if (pScnMdl == nullptr) {
        return false;
    }

    if (pScnMdl->GetResMdl().GetResMatNumEntries() == 0) {
        return false;
    }

    nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, 0);
    return access.IsValid();
}

void BModelObj::SetModel(const std::tr1::shared_ptr<CModel>& rModel, u32 flags) {
    mModel = rModel;
    if (mModel) {
        mFlags |= FLAG_DIRTY_MTX;
        mFlags &= ~FLAG_HIDDEN;

        int type = flags & MODEL_TYPE_MASK;
        bool outline = false;
        if (type == MODEL_TYPE_OUTLINE && IsOutlineModel(rModel)) {
            outline = true;
        }

        if (outline) {
            mFlags |= FLAG_OUTLINE;
        } else {
            mFlags &= ~FLAG_OUTLINE;
        }

        if (type == MODEL_TYPE_SHADOW) {
            mFlags |= FLAG_SHADOW;
        } else {
            mFlags &= ~FLAG_SHADOW;
        }

        if (flags & MODEL_SCALE_SIZE) {
            mFlags |= FLAG_MODEL_SCALE_SIZE;
        } else {
            mFlags &= ~FLAG_MODEL_SCALE_SIZE;
        }

        nw4r::g3d::ScnObj* pScnObj = mModel->GetScnObj();
        if (pScnObj->GetPriorityDrawXlu() == 0) {
            pScnObj->SetPriorityDrawXlu(50);
        }
    }

    mSize = CalcSize((mFlags & FLAG_SIZE_MODE) != 0);
    UpdateBounds();
}
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

void BModelObj::SetModelScaleDirect(const nw4r::math::VEC3& rScale) {
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> rootScale;
    mModelScale = rScale;
    if (mModelScale.x != 1.0f || mModelScale.y != 1.0f || mModelScale.z != 1.0f) {
        if (mModel->mRootScale) {
            mModel->mRootScale->SetScale(mModelScale);
            return;
        }

        rootScale.reset(nw4r::g3d::AnmObjChrRootScale::Construct(GetDefaultAllocator(), nullptr, static_cast<nw4r::g3d::ScnMdlSimple*>(mModel->GetScnObj())->GetResMdl()), DestroyG3dObj);
        rootScale->SetScale(mModelScale);
    }

    mModel->SetRootScale(rootScale);
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

void BModelObj::AddDrawLayer(const std::tr1::shared_ptr<IDrawLayer>& rLayer) {
    if (rLayer) {
        mDrawLayers.insert(rLayer);
    }
}

static inline nw4r::math::MTX34 MakeTransformMtx(
    const nw4r::math::VEC3& rPosition, f32 rotation,
    const nw4r::math::VEC3& rScale) {
    f32 sin;
    f32 cos;
    nw4r::math::SinCosDeg(&sin, &cos, rotation);

    nw4r::math::MTX34 rot;
    rot = nw4r::math::MTX34(cos, 0.0f, sin, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            -sin, 0.0f, cos, 0.0f);
    nw4r::math::MTX34 scale(rScale.x, 0.0f, 0.0f, 0.0f,
                            0.0f, rScale.y, 0.0f, 0.0f,
                            0.0f, 0.0f, rScale.z, 0.0f);
    nw4r::math::MTX34 trans(1.0f, 0.0f, 0.0f, rPosition.x,
                            0.0f, 1.0f, 0.0f, rPosition.y,
                            0.0f, 0.0f, 1.0f, rPosition.z);
    nw4r::math::MTX34 tmp;
    nw4r::math::MTX34 mtx;
    nw4r::math::MTX34Mult(&tmp, &trans, &scale);
    nw4r::math::MTX34Mult(&mtx, &tmp, &rot);
    return mtx;
}

void BModelObj::CalcMtx() {
    bool visible = IsVisible();
    if (mModel != nullptr) {
        nw4r::g3d::ScnObj* pScnObj = mModel->GetScnObj();
        if (visible) {
            if (mFlags & FLAG_HIDDEN) {
                mFlags &= ~FLAG_HIDDEN;
                SetSceneObjectEnabled(pScnObj, true);
            }
        } else {
            if (!(mFlags & FLAG_HIDDEN)) {
                mFlags |= FLAG_HIDDEN;
                SetSceneObjectEnabled(pScnObj, false);
            }
        }
    }

    if (!IsVisible()) {
        return;
    }
    if (!(mFlags & FLAG_DIRTY_MTX)) {
        return;
    }
    if (!mModel) {
        return;
    }

    mFlags &= ~FLAG_DIRTY_MTX;

    if (!(mFlags & FLAG_BASE_MTX)) {
        nw4r::math::MTX34 mtx = MakeTransformMtx(mPosition, mRotation, mScale);
        mModel->GetScnObj()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mtx);
    } else {
        mModel->GetScnObj()->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mBaseMtx);
    }
}

void BModelObj::SetRotation(f32 rotation) {
    mRotation = rotation;
    mFlags |= FLAG_DIRTY_MTX;
}
