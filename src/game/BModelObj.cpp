#include <game/BModelObj.h>

#include <game/AnmObjChrRootScale.h>
#include <game/CFileData.h>
#include <game/CModel.h>
#include <game/BWorld.h>
#include <game/CShadowCamera.h>
#include <game/Memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/res/g3d_resfile.h>
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
void BModelObj::SetPosition(const nw4r::math::VEC3& rPosition) {
    Unlink();
    SetTransformDirect(rPosition, nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    LinkKDTree();
}

void BModelObj::SetTransform(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity) {
    Unlink();
    SetTransformDirect(rPosition, rVelocity);
    LinkKDTree();
}

void BModelObj::SetTransformDirect(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity) {
    mPosition = rPosition;
    mVelocity = rVelocity;
    mFlags |= FLAG_DIRTY_MTX;
    UpdateBounds();
}

void BModelObj::SetScale(const nw4r::math::VEC3& rScale) {
    Unlink();
    SetScaleDirect(rScale);
    LinkKDTree();
}

void BModelObj::SetScaleDirect(const nw4r::math::VEC3& rScale) {
    mScale = rScale;
    mFlags |= FLAG_DIRTY_MTX;
    mSize = CalcSize((mFlags & FLAG_SIZE_MODE) != 0);
    UpdateBounds();
}

void BModelObj::SetModelScale(const nw4r::math::VEC3& rScale) {
    Unlink();
    SetModelScaleDirect(rScale);
    LinkKDTree();
}

void BModelObj::SetModelScaleDirect(const nw4r::math::VEC3& rScale) {
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> rootScale;
    mModelScale = rScale;
    if (mModelScale.x != 1.0f || mModelScale.y != 1.0f || mModelScale.z != 1.0f) {
        if (mModel->mRootScale) {
            mModel->mRootScale->SetRootScale(mModelScale);
            return;
        }

        rootScale.reset(nw4r::g3d::AnmObjChrRootScale::Construct(GetDefaultAllocator(), nullptr, static_cast<nw4r::g3d::ScnMdlSimple*>(mModel->GetScnObj())->GetResMdl()), DestroyG3dObj);
        rootScale->SetRootScale(mModelScale);
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

f32 BModelObj::CalcPlayRate(const char* pName, f32 frame, f32 duration) {
    typedef std::vector<std::tr1::shared_ptr<CFileData> > FileList;
    FileList::iterator it;
    CModel* pModel = mModel.get();
    for (it = pModel->mModelData.GetFiles().begin(); it != pModel->mModelData.GetFiles().end(); ++it) {
        nw4r::g3d::ResAnmChr anim = nw4r::g3d::ResFile((*it)->GetData()).GetResAnmChr(pName);
        if (anim.IsValid()) {
            return (anim.GetNumFrame() - frame) / duration;
        }
    }
    return 1.0f;
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

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::LengthPlay(const char* pName, f32 frame, f32 length) {
    return mModel->Play(pName, frame, CalcPlayRate(pName, frame, length));
}

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::LengthRequest(const char* pName, f32 frame, f32 length) {
    return mModel->PlayLoop(pName, frame, CalcPlayRate(pName, frame, length));
}

std::tr1::shared_ptr<CModel::CMotionProgram> BModelObj::LengthRequestNext(const char* pName, f32 frame, f32 length) {
    return mModel->PlayOnce(pName, frame, CalcPlayRate(pName, frame, length));
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

inline void BModelObj::AddToWorldScene() {
    BWorld* pWorld = mWorld;
    if (pWorld != nullptr && mModel) {
        if (mFlags & FLAG_OUTLINE) {
            UpdateOutline();
            pWorld->GetOutlineGroup()->PushBack(mModel->GetScnObj());
        } else {
            if (mFlags & FLAG_SHADOW) {
                CShadowCamera* pShadowCamera = pWorld->GetShadowCamera();
                if (pShadowCamera != nullptr) {
                    pShadowCamera->SetupShadowModel(mModel);
                }
            }
            pWorld->GetModelGroup()->PushBack(mModel->GetScnObj());
        }
    }
}

void BModelObj::ChangeModel(const std::tr1::shared_ptr<CModel>& rModel, u32 flags) {
    Unlink();
    SetModel(rModel, flags);

    AddToWorldScene();
}

void BModelObj::SetSizeMode(int mode) {
    if (mode != ((mFlags & FLAG_SIZE_MODE) != 0)) {
        Unlink();
        if (mode == 1) {
            mFlags |= FLAG_SIZE_MODE;
        } else {
            mFlags &= ~FLAG_SIZE_MODE;
        }
        mSize = CalcSize((mFlags & FLAG_SIZE_MODE) != 0);
        UpdateBounds();
        LinkKDTree();
    }
}

void BModelObj::SetOutlineType(int type) {
    if (type != mOutlineType) {
        mOutlineType = type;
        UpdateOutline();
    }
}

void BModelObj::UpdateOutline() {
    if ((mFlags & FLAG_OUTLINE) && mWorld != nullptr && mModel && mWorld->HasOutlineData()) {
        u8 level;
        switch (mOutlineType) {
        case 0:
            level = 0;
            break;
        default:
            level = mId & 0x3F;
            if (level == 0 || level == 0x20) {
                level++;
            }
            break;
        case 2:
            level = 0x20;
            break;
        }
        u8 alpha = (level << 2) | (level >> 4);

        nw4r::g3d::ScnMdl* pScnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mModel->GetScnObj());
        u32 numMats = pScnMdl->GetResMdl().GetResMatNumEntries();
        for (u32 i = 0; i < numMats; i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess access(pScnMdl, i);
            access.GetResMatPix(false).GXSetDstAlpha(true, alpha);
        }
    }
}

inline nw4r::math::VEC3 BModelObj::CalcExtent(const nw4r::math::AABB& box, int mode) const {
    f32 maxX = static_cast<f32>(__fabs(box.max.x));
    f32 minX = static_cast<f32>(__fabs(box.min.x));
    maxX = (minX < maxX) ? maxX : minX;
    f32 extentX = mScale.x * maxX;
    f32 maxZ = static_cast<f32>(__fabs(box.max.z));
    f32 minZ = static_cast<f32>(__fabs(box.min.z));
    maxZ = (minZ < maxZ) ? maxZ : minZ;
    f32 extentZ = mScale.z * maxZ;
    nw4r::math::VEC3 size;
    if (mode == 1) {
        size.x = (extentZ < extentX) ? extentZ : extentX;
    } else {
        size.x = (extentX < extentZ) ? extentZ : extentX;
    }

    if (0.0f < box.max.y) {
        size.y = box.max.y * mScale.y * 0.5f;
    } else {
        size.y = 0.0f;
    }

    size.z = size.x;
    return size;
}

nw4r::math::VEC3 BModelObj::CalcSize(int mode) const {
    if (!mModel) {
        return nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    }

    if (mFlags & FLAG_MODEL_SCALE_SIZE) {
        nw4r::math::VEC3 scale = mModelScale;
        nw4r::math::AABB bounds = mModel->GetBounds();
        nw4r::g3d::ResMdlInfo info = static_cast<nw4r::g3d::ScnMdlSimple*>(mModel->GetScnObj())->GetResMdl().GetResMdlInfo();
        bounds.min.x *= scale.x;
        bounds.min.y *= scale.y;
        bounds.min.z *= scale.z;
        bounds.max.x *= scale.x;
        bounds.max.y *= scale.y;
        bounds.max.z *= scale.z;

        if (info.ref().is_valid_volume) {
            nw4r::math::AABB volume;
            volume.min.x = info.ref().volume_min.x * scale.x;
            volume.min.y = info.ref().volume_min.y * scale.y;
            volume.min.z = info.ref().volume_min.z * scale.z;
            volume.max.x = info.ref().volume_max.x * scale.x;
            volume.max.y = info.ref().volume_max.y * scale.y;
            volume.max.z = info.ref().volume_max.z * scale.z;
            if (!(mFlags & FLAG_NO_BOUNDING)) {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, &volume);
            } else {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, nullptr);
            }

            if (bounds.max - bounds.min == nw4r::math::VEC3(0.0f, 0.0f, 0.0f)) {
                bounds = volume;
            }
        }
        nw4r::math::VEC3 size = CalcExtent(bounds, mode);
        return size;
    } else {
        nw4r::math::VEC3 scale = mModelScale;
        nw4r::math::AABB bounds = mModel->GetBounds();
        nw4r::g3d::ResMdlInfo info = static_cast<nw4r::g3d::ScnMdlSimple*>(mModel->GetScnObj())->GetResMdl().GetResMdlInfo();

        if (info.ref().is_valid_volume) {
            bounds.min.x = info.ref().volume_min.x * scale.x;
            bounds.min.y = info.ref().volume_min.y * scale.y;
            bounds.min.z = info.ref().volume_min.z * scale.z;
            bounds.max.x = info.ref().volume_max.x * scale.x;
            bounds.max.y = info.ref().volume_max.y * scale.y;
            bounds.max.z = info.ref().volume_max.z * scale.z;
            if (!(mFlags & FLAG_NO_BOUNDING)) {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, &bounds);
            } else {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, nullptr);
            }
        } else {
            bounds.min.x *= scale.x;
            bounds.min.z *= scale.z;
            bounds.max.x *= scale.x;
            bounds.max.z *= scale.z;
        }
        f32 maxX = static_cast<f32>(__fabs(bounds.max.x));
        f32 minX = static_cast<f32>(__fabs(bounds.min.x));
        maxX = (minX < maxX) ? maxX : minX;
        f32 extentX = mScale.x * maxX;
        f32 maxZ = static_cast<f32>(__fabs(bounds.max.z));
        f32 minZ = static_cast<f32>(__fabs(bounds.min.z));
        maxZ = (minZ < maxZ) ? maxZ : minZ;
        f32 extentZ = mScale.z * maxZ;
        f32 radius;
        if (mode == 1) {
            radius = (extentZ < extentX) ? extentZ : extentX;
        } else {
            radius = (extentX < extentZ) ? extentZ : extentX;
        }

        f32 height;
        if (0.0f < bounds.max.y) {
            height = bounds.max.y * mScale.y * 0.5f;
        } else {
            height = 0.0f;
        }
        return nw4r::math::VEC3(radius, height, radius);
    }
}

nw4r::math::VEC3 BModelObj::GetSize() {
    return mSize;
}

DrawLayerSet* BModelObj::GetDrawLayers() {
    return &mDrawLayers;
}

void BModelObj::OnEnterWorld() {
    AddToWorldScene();
    LinkKDTree();
}

void BModelObj::OnLeaveWorld(BWorld* pWorld) {
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

void BModelObj::SetVisible(bool visible) {
    if (visible) {
        mState |= STATE_VISIBLE;
    } else {
        mState &= ~STATE_VISIBLE;
    }

    if (visible) {
        LinkKDTree();
    } else {
        Unlink();
    }
}

void BModelObj::SetRotation(f32 rotation) {
    mRotation = rotation;
    mFlags |= FLAG_DIRTY_MTX;
}
