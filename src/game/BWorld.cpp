#include <game/BWorld.h>

#include <game/AnmObjChrRootScale.h>
#include <game/BWorldCamera.h>
#include <game/CShadowCamera.h>
#include <game/CKDTree.h>
#include <game/EffectGroupID.h>
#include <game/MathUtil.h>
#include <game/Memory.h>
#include <game/Sound.h>
#include <game/WorkBuffer.h>
#include <map>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/math/math_geometry.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <revolution/gx.h>
#include <revolution/mtx.h>

BWorld::Desc::Desc() :
    mDrawType(0),
    mObjectCapacity(0),
    mOutlineCapacity(0),
    mModelCapacity(0),
    mScnObjCapacity(0),
    mEffectCapacity(0),
    mCaptureEnable(false),
    mSoundListenerEnable(false) {}

void BWorld::Desc::SetDrawType(u32 type) {
    mDrawType = type;
}

void BWorld::Desc::SetObjectCapacity(u32 capacity) {
    mObjectCapacity = capacity;
}

void BWorld::Desc::SetOutlineCapacity(u32 capacity) {
    mOutlineCapacity = capacity;
}

void BWorld::Desc::SetModelCapacity(u32 capacity) {
    mModelCapacity = capacity;
}

void BWorld::Desc::SetScnObjCapacity(u32 capacity) {
    mScnObjCapacity = capacity;
}

void BWorld::Desc::SetEffectCapacity(u32 capacity) {
    mEffectCapacity = capacity;
}

void BWorld::Desc::SetCaptureEnable(bool enable) {
    mCaptureEnable = enable;
}

void BWorld::Desc::SetSoundListenerEnable(bool enable) {
    mSoundListenerEnable = enable;
}

void BWorld::TextureState::Save(nw4r::ef::ResTexture texture) {
    if (!texture.IsValid()) {
        return;
    }

    mTexture = texture;
    mWidth = texture.ptr()->width;
    mHeight = texture.ptr()->height;
    mFormat = texture.ptr()->format;
    mHasTlut = texture.ptr()->format == GX_TF_C4 || texture.ptr()->format == GX_TF_C8 ||
               texture.ptr()->format == GX_TF_C14X2;
    mMipmap = texture.ptr()->mipmap;
    mpTexture = texture.ptr()->texture;
    if (mHasTlut) {
        mTlutFormat = texture.ptr()->tlutFormat;
        mTlutEntries = texture.ptr()->tlutEntries;
        mpTlut = texture.ptr()->tlut;
    }
}

void BWorld::TextureState::Restore() {
    if (!mTexture.IsValid()) {
        return;
    }

    mTexture.SetWidth(mWidth);
    mTexture.SetHeight(mHeight);
    mTexture.SetFormat(mFormat);
    mTexture.SetMipmap(mMipmap);
    mTexture.SetTexture(mpTexture);
    if (mHasTlut) {
        mTexture.SetTlutFormat(mTlutFormat);
        mTexture.SetTlutEntries(mTlutEntries);
        mTexture.SetTlut(mpTlut);
    }
}

#pragma dont_inline on
BWorld::BWorld(const Desc& rDesc) :
    mFlags(0),
    mLastObjHandle(0),
    mPendingFrames(0),
    mDrawType(rDesc.GetDrawType()),
    mObjEntries(rDesc.GetObjectCapacity()),
    mObjHead(nullptr),
    mObjTail(nullptr),
    mModelTree(new CKDTree<BModelObj>) {
    EnableCalc(true);

    mScnRoot.reset(nw4r::g3d::ScnRoot::Construct(GetDefaultAllocator(), nullptr, 0x10, rDesc.GetScnObjCapacity()),
                   DestroyG3dObj);
    mOutlineGroup.reset(nw4r::g3d::ScnGroup::Construct(GetDefaultAllocator(), nullptr, rDesc.GetOutlineCapacity()),
                        DestroyG3dObj);
    mModelGroup.reset(nw4r::g3d::ScnGroup::Construct(GetDefaultAllocator(), nullptr, rDesc.GetModelCapacity()),
                      DestroyG3dObj);

    if (rDesc.GetEffectCapacity() != 0) {
        u32 groupID = AllocEffectGroupID();
        if (groupID != EFFECT_GROUP_INVALID) {
            mEffectData.reset(new EffectData);
            mEffectData->mGroupID = groupID;
            mEffectData->mPauseGroupID = AllocEffectGroupID();
            mEffectData->mLastHandle = 0;
            mEffectData->mEffects.resize(rDesc.GetEffectCapacity());
            mEffectData->mCaptureEnable = false;
            mEffectData->mCaptureGroupID = EFFECT_GROUP_INVALID;

            if (rDesc.IsCaptureEnable()) {
                nw4r::ef::ResTexture texture = nw4r::ef::Resource::GetInstance()->FindTexture("dummy_capture", nullptr);
                if (texture.IsValid()) {
                    const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
                    mEffectData->mCaptureTexture.Init(GetWorkBuffer(0)->GetBuffer(), GetWorkBuffer(0)->GetSize(),
                                                      pRenderMode->fbWidth, pRenderMode->efbHeight, GX_TF_RGB565,
                                                      GX_CLAMP, GX_LINEAR);
                    mEffectData->mTextureState.Save(nw4r::ef::ResTexture(texture));
                    texture.SetWidth(mEffectData->mCaptureTexture.GetWidth());
                    texture.SetHeight(mEffectData->mCaptureTexture.GetHeight());
                    texture.SetFormat(mEffectData->mCaptureTexture.GetFormat());
                    texture.SetMipmap(false);
                    texture.SetTexture(mEffectData->mCaptureTexture.GetBuffer());
                    mEffectData->mCaptureEnable = true;
                    mEffectData->mCaptureGroupID = AllocEffectGroupID();
                }
            }
        }
    }

    if (mDrawType == DRAW_TYPE_OUTLINE || mDrawType == DRAW_TYPE_OUTLINE_ALPHA) {
        mOutlineData.reset(new OutlineData);
        const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
        mOutlineData->mTexture.Init(GetWorkBuffer(0)->GetBuffer(), GetWorkBuffer(0)->GetSize(), pRenderMode->fbWidth,
                                    pRenderMode->efbHeight, GX_TF_I8, GX_CLAMP, GX_NEAR);
        mOutlineData->mWidth = 1;
        mOutlineData->mAlpha = 0xFF;
        mOutlineData->mColor = 0xFF000000;
    }

    SetFlag(FLAG_SOUND_LISTENER, rDesc.IsSoundListenerEnable());
}
#pragma dont_inline reset

BWorld::~BWorld() {
    for (std::vector<ObjEntry>::iterator it = mObjEntries.begin(); it != mObjEntries.end(); ++it) {
        RemoveObject(it->mObj);
    }

    if (mEffectData) {
        RemoveAllEffects(TRUE);
        if (mEffectData->mCaptureEnable) {
            mEffectData->mTextureState.Restore();
        }
        if (mEffectData->mCaptureGroupID != EFFECT_GROUP_INVALID) {
            FreeEffectGroupID(mEffectData->mCaptureGroupID);
        }
        if (mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
            FreeEffectGroupID(mEffectData->mPauseGroupID);
        }
        FreeEffectGroupID(mEffectData->mGroupID);
        mEffectData.reset();
    }
}

void BWorld::EnableCalc(bool enable) {
    if (!enable) {
        mFlags |= FLAG_CALC_DISABLED;
    } else {
        mFlags &= ~FLAG_CALC_DISABLED;
    }
}

void BWorld::RequestPause() {
    mFlags = (mFlags & ~FLAG_RESUME_REQUESTED) | FLAG_PAUSE_REQUESTED;
}

void BWorld::RequestResume() {
    mFlags = (mFlags & ~FLAG_PAUSE_REQUESTED) | FLAG_RESUME_REQUESTED;
}

BWorldCamera* BWorld::GetCamera(int index) {
    if (index < 0 || mCameras.size() <= static_cast<u32>(index)) {
        return nullptr;
    }
    return mCameras[index].lock().get();
}

bool BWorld::AddCamera(std::tr1::shared_ptr<BWorldCamera> camera, u32 index) {
    if (!AddObject(camera)) {
        return false;
    }
    SetCamera(camera, index);
    return true;
}

void BWorld::SetCamera(std::tr1::shared_ptr<BWorldCamera> camera, u32 index) {
    ClearCamera(index);
    if (index >= mCameras.size()) {
        mCameras.resize(index + 1);
    }
    mCameras[index] = camera;
    camera->SetIndex(index);

    std::tr1::shared_ptr<CShadowCamera> shadowCamera = DynamicCast<CShadowCamera>(camera);
    if (shadowCamera) {
        mShadowCamera = shadowCamera;
    }
    camera->UpdateIndex(-1);
}

int BWorld::AddCamera(std::tr1::shared_ptr<BWorldCamera> camera) {
    if (!AddObject(camera)) {
        return -1;
    }
    return AttachCamera(camera);
}

int BWorld::AttachCamera(std::tr1::shared_ptr<BWorldCamera> camera) {
    if (camera->GetIndex() >= 0 && GetCamera(camera->GetIndex()) == camera.get()) {
        return camera->GetIndex();
    }

    int index;
    for (index = 0; index < static_cast<int>(mCameras.size()); index++) {
        if (!mCameras[index].lock()) {
            break;
        }
    }

    if (index < 32) {
        SetCamera(camera, index);
        return index;
    }
    return -1;
}

void BWorld::RemoveCamera(std::tr1::shared_ptr<BWorldCamera> camera) {
    if (camera) {
        ClearCamera(camera->GetIndex());
    }
}

void BWorld::ClearCamera(u32 index) {
    if (index >= mCameras.size()) {
        return;
    }

    std::tr1::shared_ptr<BWorldCamera> camera = mCameras[index].lock();
    mCameras[index].reset();
    if (camera) {
        camera->SetIndex(-1);
        camera->UpdateIndex(index);
    }

    if (camera.get() == mCurrentCamera.lock().get()) {
        mCurrentCamera.reset();
    }

    if (IsKindOf<CShadowCamera>(camera) && camera.get() == mShadowCamera.lock().get()) {
        mShadowCamera.reset();
    }
}

CShadowCamera* BWorld::GetShadowCamera() {
    return mShadowCamera.lock().get();
}

void BWorld::SetCurrentCamera(std::tr1::shared_ptr<BWorldCamera> camera) {
    mCurrentCamera = camera;
}

BWorldCamera* BWorld::GetCurrentCamera() {
    return mCurrentCamera.lock().get();
}

nw4r::g3d::Camera BWorld::GetG3dCamera() {
    BWorldCamera* camera = mCurrentCamera.lock().get();
    if (camera) {
        return camera->GetG3dCamera();
    }
    return mScnRoot->GetCamera(0);
}

void BWorld::SetOutlineAlpha(u8 alpha) {
    if (mOutlineData) {
        mOutlineData->mAlpha = alpha;
    }
}

void BWorld::SetOutlineWidth(int width) {
    if (mOutlineData) {
        mOutlineData->mWidth = width;
    }
}

bool BWorld::AddGround(std::tr1::shared_ptr<BWorldObj> obj) {
    if (!AddObject(obj)) {
        return false;
    }
    SetGround(obj);
    return true;
}

void BWorld::SetGround(std::tr1::shared_ptr<BWorldObj> obj) {
    mGround = obj;
}

bool BWorld::Add(std::tr1::shared_ptr<BWorldObj> obj) {
    if (!AddObject(obj)) {
        return false;
    }
    if (IsKindOf<BWorldCamera>(obj)) {
        AttachCamera(DynamicCast<BWorldCamera>(obj));
    }
    return true;
}

bool BWorld::AddObject(std::tr1::shared_ptr<BWorldObj> obj) {
    if (obj) {
        if (obj->mWorld != nullptr) {
            return true;
        }

        u32 capacity = mObjEntries.size();
        for (u32 i = 1; i <= capacity; i++) {
            u32 handle = mLastObjHandle + i;
            if (handle & 0x80000000) {
                return false;
            }

            u32 slot = handle % capacity;
            if (!mObjEntries[slot].mObj) {
                mLastObjHandle = handle;
                obj->mWorld = this;
                obj->mId = mLastObjHandle;

                ObjEntry* pEntries = mObjEntries.begin();
                ObjEntry* pEntry = pEntries + slot;
                pEntry->mObj = obj;
                u32 type = obj->mType;
                pEntry->mFlags = type | ENTRY_NEW;

                if (mObjHead == nullptr) {
                    mObjTail = pEntry;
                    mObjHead = pEntry;
                    pEntry->mPrev = 0;
                    pEntry->mNext = 0;
                } else if (type & 0x180) {
                    ObjEntry* pHead = mObjHead;
                    mObjHead = pEntry;
                    pEntry->mNext = pHead - pEntries + 1;
                    pEntry->mPrev = 0;
                    pHead->mPrev = pEntry - pEntries + 1;
                } else {
                    ObjEntry* pTail = mObjTail;
                    mObjTail = pEntry;
                    pEntry->mNext = 0;
                    pEntry->mPrev = pTail - pEntries + 1;
                    pTail->mNext = pEntry - pEntries + 1;
                }

                obj->OnEnterWorld();
                if (mFlags & FLAG_PAUSED) {
                    obj->Pause();
                }
                return true;
            }
        }
    }
    return false;
}
void BWorld::RemoveObject(std::tr1::shared_ptr<BWorldObj> obj) {
    if (!obj || obj->mWorld == nullptr) {
        return;
    }

    ObjEntry* pEntries = mObjEntries.begin();
    ObjEntry* pEntry = &mObjEntries[obj->mId % mObjEntries.size()];
    pEntry->mObj.reset();
    pEntry->mFlags = 0;

    ObjEntry* pNext = pEntry->GetNext(pEntries);
    ObjEntry* pPrev = pEntry->GetPrev(pEntries);
    if (pEntry->mPrev == 0) {
        if (pEntry->mNext == 0) {
            mObjTail = nullptr;
            mObjHead = nullptr;
        } else {
            mObjHead = pNext;
            pNext->mPrev = 0;
        }
    } else if (pEntry->mNext == 0) {
        mObjTail = pPrev;
        pPrev->mNext = 0;
    } else {
        pPrev->mNext = pNext - pEntries + 1;
        pNext->mPrev = pPrev - pEntries + 1;
    }

    if (std::tr1::shared_ptr<BWorldCamera> camera = DynamicCast<BWorldCamera>(obj)) {
        RemoveCamera(camera);
    }

    if (obj.get() == mGround.lock().get()) {
        mGround.reset();
    }

    obj->mWorld = nullptr;
    obj->OnLeaveWorld();

    if (mFlags & FLAG_IN_CALC) {
        mRemovedObjs.insert(obj);
    }
}

BWorldObj* BWorld::GetObject(u32 id) {
    if (id == 0) {
        return nullptr;
    }
    return mObjEntries[id % mObjEntries.size()].mObj.get();
}

std::vector<BWorldObj*> BWorld::GetObjects(u32 type) {
    std::vector<BWorldObj*> objs;
    objs.reserve(mObjEntries.size());

    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        u32 entryType = pEntry->mFlags & ~ENTRY_NEW;
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }

        objs.push_back(pEntry->mObj.get());
    }
    return objs;
}

int BWorld::CountObjects(u32 type) {
    int count = 0;
    u32 entryType;
    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        entryType = pEntry->GetType();
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }
        count++;
    }
    return count;
}

BWorldObj* BWorld::FindNearestObject(u32 type, BWorldObj* pOrigin) {
    BWorldObj* pNearest = nullptr;
    f32 nearestDistSq = 0.0f;
    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        u32 entryType = pEntry->mFlags & ~ENTRY_NEW;
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }

        BWorldObj* pObj = pEntry->mObj.get();
        if (pObj != pOrigin && pObj->IsVisible()) {
            nw4r::math::VEC3 diff = pObj->mPosition - pOrigin->mPosition;
            f32 distSq = nw4r::math::VEC3LenSq(&diff);
            if (pNearest == nullptr || distSq < nearestDistSq) {
                pNearest = pObj;
                nearestDistSq = distSq;
            }
        }
    }
    return pNearest;
}

namespace {

struct SectorPlane {
    f32 GetOriginDistance() const { return -mD; }

    f32 GetDistance(const nw4r::math::VEC3& rCenter, f32 radius) const {
        f32 dist = rCenter.z * mNormal.z + rCenter.x * mNormal.x - GetOriginDistance();
        if (radius <= dist) {
            return dist - radius;
        }
        if (dist <= -radius) {
            return dist + radius;
        }
        return 0.0f;
    }

    nw4r::math::VEC3 mNormal;
    f32 mD;
};

}

std::list<BWorldObj*> BWorld::FindObjectsInSector(u32 type, const nw4r::math::VEC3& rCenter, f32 innerRadius,
                                                  f32 outerRadius, f32 startAngle, f32 endAngle) {
    std::list<BWorldObj*> objs;
    nw4r::math::VEC3 center(rCenter.x, 0.0f, rCenter.z);
    f32 sin;
    f32 cos;

    SectorPlane startPlane;
    nw4r::math::SinCosDeg(&sin, &cos, startAngle + 90.0f);
    startPlane.mNormal.x = sin;
    startPlane.mNormal.y = 0.0f;
    startPlane.mNormal.z = cos;
    startPlane.mD = -nw4r::math::VEC3Dot(&center, &startPlane.mNormal);

    SectorPlane endPlane;
    nw4r::math::SinCosDeg(&sin, &cos, endAngle - 90.0f);
    endPlane.mNormal.x = sin;
    endPlane.mNormal.y = 0.0f;
    endPlane.mNormal.z = cos;
    endPlane.mD = -nw4r::math::VEC3Dot(&center, &endPlane.mNormal);

    f32 range = NormalizeAngle(endAngle - startAngle);
    bool narrow = range <= 180.0f;
    SectorPlane midPlane;
    if (narrow) {
        nw4r::math::SinCosDeg(&sin, &cos, startAngle + range * 0.5f);
        midPlane.mNormal.x = sin;
        midPlane.mNormal.y = 0.0f;
        midPlane.mNormal.z = cos;
        midPlane.mD = -nw4r::math::VEC3Dot(&center, &midPlane.mNormal);
    }

    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        u32 entryType = pEntry->mFlags & ~ENTRY_NEW;
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }

        BWorldObj* const& pObj = pEntry->mObj.get();
        nw4r::math::VEC3 pos = pObj->mPosition;
        nw4r::math::VEC3 diff = pos - center;
        f32 distSq = diff.z * diff.z + diff.x * diff.x;

        f32 radius = pObj->GetSize().z;
        if ((outerRadius + radius) * (outerRadius + radius) < distSq) {
            continue;
        }
        f32 inner = innerRadius - radius;
        if (0.0f < inner && distSq < inner * inner) {
            continue;
        }

        nw4r::math::SPHERE sphere;
        sphere.C.x = pos.x;
        sphere.C.y = 0.0f;
        sphere.C.z = pos.z;
        sphere.r = pObj->GetSize().z;
        if (narrow) {
            if (!(0.0f <= startPlane.GetDistance(sphere.C, sphere.r)) ||
                !(0.0f <= endPlane.GetDistance(sphere.C, sphere.r)) ||
                !(0.0f <= midPlane.GetDistance(sphere.C, sphere.r))) {
                continue;
            }
        } else if (startPlane.GetDistance(sphere.C, sphere.r) < 0.0f &&
                   endPlane.GetDistance(sphere.C, sphere.r) < 0.0f) {
            continue;
        }

        objs.push_back(pObj);
    }
    return objs;
}

std::list<BWorldObj*> BWorld::FindObjectsInRing(u32 type, const nw4r::math::VEC3& rCenter, f32 innerRadius,
                                                f32 outerRadius) {
    std::list<BWorldObj*> objs;
    nw4r::math::VEC3 center(rCenter.x, 0.0f, rCenter.z);

    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        u32 entryType = pEntry->mFlags & ~ENTRY_NEW;
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }

        BWorldObj* const& pObj = pEntry->mObj.get();
        nw4r::math::VEC3 pos = pObj->mPosition;
        nw4r::math::VEC3 diff = pos - center;
        f32 distSq = diff.z * diff.z + diff.x * diff.x;

        f32 radius = pObj->GetSize().z;
        if ((outerRadius + radius) * (outerRadius + radius) < distSq) {
            continue;
        }
        f32 inner = innerRadius - radius;
        if (0.0f < inner && distSq < inner * inner) {
            continue;
        }

        objs.push_back(pObj);
    }
    return objs;
}

std::list<BWorldObj*> BWorld::FindObjectsInRect(u32 type, const nw4r::math::VEC3& rCenter, f32 rotation, f32 minZ,
                                                f32 maxZ, f32 minX, f32 maxX) {
    std::list<BWorldObj*> objs;
    nw4r::math::VEC3 center(rCenter.x, 0.0f, rCenter.z);
    nw4r::math::VEC3 trans = -center;
    nw4r::math::MTX34 transMtx(1.0f, 0.0f, 0.0f, trans.x, 0.0f, 1.0f, 0.0f, trans.y, 0.0f, 0.0f, 1.0f, trans.z);

    f32 sin;
    f32 cos;
    nw4r::math::SinCosDeg(&sin, &cos, -rotation);
    nw4r::math::MTX34 rotMtx(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin, 0.0f, cos, 0.0f);

    nw4r::math::MTX34 mtx;
    PSMTXConcat(rotMtx, transMtx, mtx);

    u32 stopTypes = GetStopTypes(type);
    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        u32 entryType = pEntry->mFlags & ~ENTRY_NEW;
        if (!(entryType & type)) {
            if (entryType & stopTypes) {
                continue;
            }
            break;
        }

        BWorldObj* const& pObj = pEntry->mObj.get();
        nw4r::math::VEC3 local;
        PSMTXMultVec(mtx, pObj->mPosition, local);

        f32 radius = pObj->GetSize().z;
        if (minZ - radius <= local.z && local.z <= maxZ + radius && minX - radius <= local.x &&
            local.x <= maxX + radius) {
            objs.push_back(pObj);
        }
    }
    return objs;
}

CEffect* BWorld::GetEffect(u32 handle) {
    if (!mEffectData || handle == 0) {
        return nullptr;
    }
    return mEffectData->mEffects[handle % mEffectData->mEffects.size()].get();
}

u32 BWorld::GetEffectGroupID() const {
    if (!mEffectData) {
        return EFFECT_GROUP_INVALID;
    }
    if (((mFlags & FLAG_PAUSED) || (mFlags & FLAG_SKIP_STATE_2)) && mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
        return mEffectData->mPauseGroupID;
    }
    return mEffectData->mGroupID;
}

u32 BWorld::GetCaptureEffectGroupID() const {
    if (!mEffectData) {
        return EFFECT_GROUP_INVALID;
    }
    if (((mFlags & FLAG_PAUSED) || (mFlags & FLAG_SKIP_STATE_2)) && mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
        return mEffectData->mPauseGroupID;
    }
    if (mEffectData->mCaptureGroupID != EFFECT_GROUP_INVALID) {
        return mEffectData->mCaptureGroupID;
    }
    return mEffectData->mGroupID;
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, const nw4r::math::VEC3& rPosition, f32 rotation) {
    if (!mEffectData) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, rPosition, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow) {
    if (!mEffectData) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (!mEffectData) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffectFollowRotation(const CEffect::Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (!mEffectData) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->CreateFollowRotation(this, rParam, pFollow, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow, BWorldObj* pTarget) {
    if (!mEffectData) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow, pTarget);
    return AddEffect(effect);
}

void BWorld::RemoveAllEffects(BOOL retire) {
    if (!mEffectData) {
        return;
    }

    for (std::vector<std::tr1::shared_ptr<CEffect> >::iterator it = mEffectData->mEffects.begin();
         it != mEffectData->mEffects.end(); ++it) {
        if (*it) {
            (*it)->Finalize(retire);
            it->reset();
        }
    }

    if (retire == TRUE) {
        nw4r::ef::EffectSystem* pSystem = nw4r::ef::EffectSystem::GetInstance();
        if (mEffectData->mCaptureGroupID != EFFECT_GROUP_INVALID) {
            pSystem->RetireParticleAll(mEffectData->mCaptureGroupID);
            pSystem->RetireEmitterAll(mEffectData->mCaptureGroupID);
            pSystem->RetireEffectAll(mEffectData->mCaptureGroupID);
        }
        if (mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
            pSystem->RetireParticleAll(mEffectData->mPauseGroupID);
            pSystem->RetireEmitterAll(mEffectData->mPauseGroupID);
            pSystem->RetireEffectAll(mEffectData->mPauseGroupID);
        }
        pSystem->RetireParticleAll(mEffectData->mGroupID);
        pSystem->RetireEmitterAll(mEffectData->mGroupID);
        pSystem->RetireEffectAll(mEffectData->mGroupID);
    }
}

bool BWorld::RemoveOldestEffect(bool retire) {
    if (!mEffectData) {
        return false;
    }

    std::map<u32, CEffect*> effects;
    for (std::vector<std::tr1::shared_ptr<CEffect> >::iterator it = mEffectData->mEffects.begin(); it != mEffectData->mEffects.end(); ++it) {
        if (*it && !(*it)->IsFinished() && !(*it)->IsPersistent()) {
            effects.insert(std::map<u32, CEffect*>::value_type((*it)->GetHandle(), it->get()));
        }
    }

    if (effects.empty()) {
        return false;
    }
    effects.begin()->second->Finalize(retire);
    return true;
}

CEffect* BWorld::AddEffect(std::tr1::shared_ptr<CEffect> effect) {
    if (effect && !effect->IsFinished()) {
        u32 capacity = mEffectData->mEffects.size();
        for (u32 i = 1; i <= capacity; i++) {
            u32 handle = mEffectData->mLastHandle + i;
            if (handle & 0x80000000) {
                return nullptr;
            }

            u32 slot = handle % capacity;
            if (!mEffectData->mEffects[slot] || mEffectData->mEffects[slot]->IsFinished()) {
                mEffectData->mLastHandle = handle;
                mEffectData->mEffects[slot] = effect;
                effect->SetHandle(handle);
                return effect.get();
            }
        }
    }
    return nullptr;
}

void BWorld::Calc() {
    if (mFlags & FLAG_CALC_DISABLED) {
        return;
    }

    if ((mFlags & FLAG_PAUSE_REQUESTED) && !(mFlags & FLAG_PAUSED)) {
        for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
            pEntry->mObj->Pause();
        }
    } else if ((mFlags & FLAG_RESUME_REQUESTED) && (mFlags & FLAG_PAUSED)) {
        for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
            pEntry->mObj->Resume();
        }
    }

    mPendingFrames++;
    mFlags = (mFlags & ~(FLAG_PAUSE_REQUESTED | FLAG_RESUME_REQUESTED)) | FLAG_IN_CALC;

    u32 skipStates = 0;
    if (mFlags & FLAG_SKIP_STATE_2) {
        skipStates |= 2;
    }
    if (mFlags & FLAG_SKIP_STATE_4) {
        skipStates |= 4;
    }
    if (mFlags & FLAG_PAUSED) {
        skipStates |= 0x10;
    }

    std::vector<ObjEntry>::iterator it = mObjEntries.begin();
    u32 lastHandle = mLastObjHandle;
    for (; it != mObjEntries.end(); ++it) {
        BWorldObj* pObj = it->mObj.get();
        if (pObj != nullptr && !(pObj->mState & skipStates)) {
            pObj->Calc();
            it->mFlags &= ~ENTRY_NEW;
        }
    }

    while (lastHandle != mLastObjHandle) {
        lastHandle = mLastObjHandle;
        for (it = mObjEntries.begin(); it != mObjEntries.end(); ++it) {
            BWorldObj* pObj = it->mObj.get();
            if (pObj != nullptr && !(pObj->mState & skipStates) && (it->mFlags & ENTRY_NEW)) {
                pObj->Calc();
                it->mFlags &= ~ENTRY_NEW;
            }
        }
    }

    mFlags &= ~FLAG_IN_CALC;
    mRemovedObjs.clear();

    if (mFlags & FLAG_SOUND_LISTENER) {
        std::tr1::shared_ptr<BWorldCamera> camera = mCurrentCamera.lock();
        if (camera) {
            nw4r::math::MTX34 mtx;
            camera->GetG3dCamera().GetCameraMtx(&mtx);
            GetSound3DListener().SetMatrix(mtx);
        }
    }
}
