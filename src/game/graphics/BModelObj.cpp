#define BMODELOBJ_EMISSION_LAYOUT

#include <revolution/types.h>
#include <MSL_CPP/alloc_ptr.h>
#include <MSL_CPP/allocator.h>
#include <MSL_CPP/detail/shared_ptr_holder.h>
class CModel;
namespace nw4r {
namespace g3d {
class AnmObjChrRootScale;
class G3dObj;
}
}
#include <memory>
enum { BModelCharPointer = sizeof(std::tr1::shared_ptr<char>) };
enum { BModelScalePointer = sizeof(std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale>) };
enum { BModelModelPointer = sizeof(std::tr1::shared_ptr<CModel>) };
#include <stdexcept>
#include <game/world/BWorldObj.h>
#include <game/graphics/BModelObj.h>
enum { BModelScaleDeleter = sizeof(std::tr1::detail::shared_ptr_deleter<nw4r::g3d::AnmObjChrRootScale, void (*)(nw4r::g3d::G3dObj*)>) };
#include <game/world/CKDTree.h>

#include <game/animation/AnmObjChrRootScale.h>
#include <game/io/CFileData.h>
#include <game/graphics/CModel.h>
#include <game/world/BWorld.h>
#include <game/world/CShadowCamera.h>
#include <game/core/Memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/math/math_triangular.h>

template <> inline void KDTreeZXNode<2, 9>::RemoveChild(Other* pChild) {
    if (pChild == mChildren[0].get()) {
        if (IsEmpty() && !mChildren[1] && mParent != nullptr) {
            mParent->RemoveChild(this);
        } else {
            mChildren[0].reset();
        }
    } else {
        if (IsEmpty() && !mChildren[0] && mParent != nullptr) {
            mParent->RemoveChild(this);
        } else {
            mChildren[1].reset();
        }
    }
}
inline void BModelObj::SetRotation(f32 rotation) {
    bool probe = false;
    if (probe) {
        std::__error_string* message = nullptr;
        message->~__error_string();
        std::exception* error = nullptr;
        error->std::exception::~exception();
    }
    mRotation = rotation;
    mFlags |= FLAG_DIRTY_MTX;
}

void SetSceneObjectEnabled(nw4r::g3d::ScnObj* pScnObj, bool enable);

BModelObj::BModelObj(u32 type) :
    BWorldObj(type | TYPE_MODEL_OBJ),
    mFlags(0),
    mModelScale(1.0f, 1.0f, 1.0f),
    mSize(0.0f, 0.0f, 0.0f),
    mVelocity(0.0f, 0.0f, 0.0f),
    mOutlineType(1) {}


BModelObj::~BModelObj() {}

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

f32 BModelDegreesToIndex() {
    return 256.0f / 360.0f;
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

void BModelObj::SetOutlineType(int type) {
    if (type != mOutlineType) {
        mOutlineType = type;
        UpdateOutline();
    }
}

void BModelObj::UpdateOutline() {
    if (!(mFlags & FLAG_OUTLINE) || mWorld == nullptr || !mModel || !mWorld->HasOutlineData()) {
        return;
    }
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

inline void BModelObj::LinkKDTree() {
    if (IsVisible() && (mFlags & FLAG_KDTREE) && !IsLinked() && mWorld != nullptr) {
        mWorld->GetModelTree()->Insert(this);
    }
}

inline void BModelObj::CalcExtent(const nw4r::math::AABB& box, int mode, nw4r::math::VEC3& size) const {
    f32 maxX = fabs(box.max.x);
    f32 minX = fabs(box.min.x);
    maxX = (minX < maxX) ? maxX : minX;
    f32 extentX = mScale.x * maxX;
    extentX = static_cast<f32>(extentX);
    f32 maxZ = fabs(box.max.z);
    f32 minZ = fabs(box.min.z);
    maxZ = (minZ < maxZ) ? maxZ : minZ;
    f32 extentZ = mScale.z * maxZ;
    if (mode == 1) {
        size.x = (extentZ < extentX) ? extentZ : extentX;
    } else {
        size.x = (extentX < extentZ) ? extentZ : extentX;
    }
}

nw4r::math::VEC3 BModelObj::CalcSize(int mode) {
    if (!mModel) {
        return nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    }

    if (mFlags & FLAG_MODEL_SCALE_SIZE) {
        nw4r::math::VEC3 scale = mModelScale;
        nw4r::math::VEC3 size;
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
            const nw4r::g3d::ResMdlInfoData& rInfo = info.ref();
            volume.min.x = rInfo.volume_min.x * scale.x;
            volume.min.y = rInfo.volume_min.y * scale.y;
            volume.min.z = rInfo.volume_min.z * scale.z;
            volume.max.x = rInfo.volume_max.x * scale.x;
            volume.max.y = rInfo.volume_max.y * scale.y;
            volume.max.z = rInfo.volume_max.z * scale.z;
            if (!(mFlags & FLAG_NO_BOUNDING)) {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, &volume);
            } else {
                mModel->GetScnObj()->SetBoundingVolume(nw4r::g3d::ScnObj::BOUNDINGVOLUME_AABB_LOCAL, nullptr);
            }

            if (bounds.max - bounds.min == nw4r::math::VEC3(0.0f, 0.0f, 0.0f)) {
                bounds = volume;
            }
        }
        CalcExtent(bounds, mode, size);
        if (0.0f < bounds.max.y) {
            size.y = (bounds.max.y * mScale.y) / 2.0f;
        } else {
            size.y = 0.0f;
        }
        size.z = size.x;
        return size;
    } else {
        nw4r::math::VEC3 size;
        nw4r::math::VEC3 scale = mModelScale;
        nw4r::math::AABB bounds = mModel->GetBounds();
        nw4r::g3d::ResMdlInfo info = static_cast<nw4r::g3d::ScnMdlSimple*>(mModel->GetScnObj())->GetResMdl().GetResMdlInfo();

        if (info.ref().is_valid_volume) {
            const nw4r::g3d::ResMdlInfoData& rInfo = info.ref();
            bounds.min.x = rInfo.volume_min.x * scale.x;
            bounds.min.y = rInfo.volume_min.y * scale.y;
            bounds.min.z = rInfo.volume_min.z * scale.z;
            bounds.max.x = rInfo.volume_max.x * scale.x;
            bounds.max.y = rInfo.volume_max.y * scale.y;
            bounds.max.z = rInfo.volume_max.z * scale.z;
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
        CalcExtent(bounds, mode, size);
        size.z = size.x;
        size.y = mScale.y * (scale.y * static_cast<f32>(fabs(bounds.max.y))) / 2.0f;
        return size;
    }
}

nw4r::math::VEC3 BModelObj::GetSize() {
    return mSize;
}

DrawLayerSet* BModelObj::GetDrawLayers() {
    return &mDrawLayers;
}

void BModelObj::AddDrawLayer(const std::tr1::shared_ptr<IDrawLayer>& rLayer) {
    if (rLayer) {
        mDrawLayers.insert(rLayer);
    }
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
