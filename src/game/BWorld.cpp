#include <game/BWorld.h>

#include <game/AnmObjChrRootScale.h>
#include <game/BWorldCamera.h>
#include <game/CFileData.h>
#include <game/CShadowCamera.h>
#include <game/CKDTree.h>
#include <game/EffectGroupID.h>
#include <game/FrameStep.h>
#include <game/Graphics.h>
#include <game/MathUtil.h>
#include <game/Memory.h>
#include <game/Sound.h>
#include <game/WorkBuffer.h>
#include <map>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/math/math_geometry.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <revolution/gx.h>
#include <revolution/gx/GXBump.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/gx/GXVert.h>
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

    if (HasEffectData()) {
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

void BWorld::DrawOutline() {
    Graphics::ResetState();
    u16 width = mOutlineData->mTexture.GetWidth();
    u16 height = mOutlineData->mTexture.GetHeight();
    GXPixModeSync();
    GXInvalidateTexAll();
    GXLoadTexObj(&mOutlineData->mTexture.GetTexObj(), GX_TEXMAP0);

    Mtx44 projMtx;
    C_MTXOrtho(projMtx, 0.0f, height, 0.0f, width, 0.0f, 1.0f);
    GXSetProjection(projMtx, GX_ORTHOGRAPHIC);
    Mtx identity;
    PSMTXIdentity(identity);
    GXLoadPosMtxImm(identity, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetViewport(0.0f, 0.0f, width, height, 0.1f, 1000.0f);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(3);
    GXSetNumTevStages(3);
    GXSetNumIndStages(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

    Mtx widthMtx;
    PSMTXIdentity(widthMtx);
    widthMtx[0][3] = static_cast<f32>(mOutlineData->mWidth) / width;
    GXLoadTexMtxImm(widthMtx, GX_TEXMTX0, GX_MTX2x4);
    Mtx heightMtx;
    PSMTXIdentity(heightMtx);
    heightMtx[1][3] = static_cast<f32>(mOutlineData->mWidth) / height;
    GXLoadTexMtxImm(heightMtx, GX_TEXMTX1, GX_MTX2x4);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

    GXColor outlineColor = mOutlineData->mColor;
    outlineColor.a = mOutlineData->mAlpha;
    GXSetTevColor(GX_TEVREG1, outlineColor);
    GXSetTevColor(GX_TEVREG2, nw4r::ut::Color(0x82828201));

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXA, GX_CC_C2, GX_CC_C1, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);

    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXA, GX_CC_C2, GX_CC_C1, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA, GX_CA_A0, GX_CA_A2, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_COMP_A8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevDirect(GX_TEVSTAGE2);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_TEXA, GX_CC_C2, GX_CC_C1, GX_CC_CPREV);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_COMP_R8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_A0, GX_CA_APREV, GX_CA_A1);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_COMP_A8_EQ, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);

    GXSetAlphaCompare(GX_EQUAL, outlineColor.a, GX_AOP_OR, GX_NEVER, 0);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
    GXSetZCompLoc(GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);

    GXBegin(GX_QUADS, GX_VTXFMT7, 4);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2u8(0, 0);
    GXPosition3f32(width, 0.0f, 0.0f);
    GXTexCoord2u8(1, 0);
    GXPosition3f32(width, height, 0.0f);
    GXTexCoord2u8(1, 1);
    GXPosition3f32(0.0f, height, 0.0f);
    GXTexCoord2u8(0, 1);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
}

bool BWorld::LoadLightScene(const char* pPath, const char* pName, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadFile(pPath, 0, background);
    if (!file) {
        return false;
    }
    bool loaded;
    nw4r::g3d::ResFile resource(file->GetData());
    nw4r::g3d::ResAnmScn scene = resource.GetResAnmScn(pName);
    if (!scene.IsValid() || !scene.Bind() || !scene.HasResLightSet()) {
        loaded = false;
    } else {
        {
            std::tr1::shared_ptr<nw4r::g3d::AnmScnRes> animation(
                nw4r::g3d::AnmScnRes::Construct(GetDefaultAllocator(), nullptr, scene, true), DestroyG3dObj);
            animation->SetFrame(0.0f);
            if (animation->GetLightSetMaxRefNumber() != 0) {
                animation->GetLightSetting(&mScnRoot->GetLightSetting());
            }
        }
        loaded = true;
    }
    return loaded;
}

std::tr1::shared_ptr<CFileData> LoadFile(const char* pPath, u32 flags, bool background) {
    return CreateFileData(pPath, flags, background);
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

                ObjEntry* pEntries = &mObjEntries[0];
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

    ObjEntry* pEntries = &mObjEntries[0];
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
    obj->OnLeaveWorld(this);

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
    if (!HasEffectData()) {
        return EFFECT_GROUP_INVALID;
    }
    if (((mFlags & FLAG_PAUSED) || (mFlags & FLAG_SKIP_STATE_2)) && mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
        return mEffectData->mPauseGroupID;
    }
    return mEffectData->mGroupID;
}

u32 BWorld::GetCaptureEffectGroupID() const {
    if (!HasEffectData()) {
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
    if (!HasEffectData()) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, rPosition, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow) {
    if (!HasEffectData()) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (!HasEffectData()) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffectFollowRotation(const CEffect::Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (!HasEffectData()) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->CreateFollowRotation(this, rParam, pFollow, rotation);
    return AddEffect(effect);
}

CEffect* BWorld::CreateEffect(const CEffect::Param& rParam, BWorldObj* pFollow, BWorldObj* pTarget) {
    if (!HasEffectData()) {
        return nullptr;
    }
    std::tr1::shared_ptr<CEffect> effect(new CEffect);
    effect->Create(this, rParam, pFollow, pTarget);
    return AddEffect(effect);
}

void BWorld::RemoveAllEffects(BOOL retire) {
    if (!HasEffectData()) {
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
    if (!HasEffectData()) {
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

    u32 skipStates = GetSkipStates();

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

namespace {

inline void RemoveAllChildren(nw4r::g3d::ScnGroup* pGroup) {
    while (!pGroup->Empty()) {
        pGroup->PopBack();
    }
}

inline void SetCopyClear(GXColor color, GXPixelFmt pixelFormat) {
    Graphics::SetCopyClear(color, 0xFFFFFF, pixelFormat);
}

inline void SetupFlatColor(nw4r::ut::Color color) {
    GXSetNumTexGens(0);
    GXSetNumChans(1);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanAmbColor(GX_COLOR0A0, color);
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_COPY);
}

}

void BWorld::CalcMtx() {
    if (mPendingFrames == 0) {
        return;
    }

    FrameStepScope frameStep;
    gFrameStep = mPendingFrames;

    u32 skipStates = GetSkipStates();

    for (std::vector<ObjEntry>::iterator it = mObjEntries.begin(); it != mObjEntries.end(); ++it) {
        BWorldObj* pObj = it->mObj.get();
        if (pObj != nullptr && !(pObj->mState & skipStates)) {
            pObj->CalcMtx();
        }
    }

    RemoveAllChildren(mScnRoot.get());
    mScnRoot->PushBack(mModelGroup.get());
    mScnRoot->PushBack(mOutlineGroup.get());

    nw4r::g3d::FrameCtrl::SetBaseUpdateRate(60.0f * gFrameStep / 60.0f);

    if (nw4r::g3d::AnmScn* pAnmScn = mScnRoot->GetAnmScn()) {
        pAnmScn->UpdateFrame();
    }
    mScnRoot->CalcWorld();
    mScnRoot->CalcVtx();
    mScnRoot->CalcMaterial();

    if (HasEffectData()) {
        for (std::vector<std::tr1::shared_ptr<CEffect> >::iterator it = mEffectData->mEffects.begin();
             it != mEffectData->mEffects.end(); ++it) {
            if (*it) {
                (*it)->CalcMtx();
                if ((*it)->IsFinished()) {
                    it->reset();
                }
            }
        }
    }
}

void BWorld::CalcView() {
    FrameStepScope frameStep;
    gFrameStep = mPendingFrames;
    mPendingFrames = 0;

    for (ObjEntry* pEntry = mObjHead; pEntry != nullptr; pEntry = GetNextEntry(pEntry)) {
        pEntry->mObj->UpdateVisible();
    }
}

namespace {

void DrawFrame(int width, int height);
void DrawTexture(int width, int height, const GXTexObj* pTexObj);

}

void BWorld::Draw() {
    Graphics::ResetState();

    GXPixelFmt pixelFormat;
    switch (mDrawType) {
    default:
        pixelFormat = GX_PF_RGB8_Z24;
        break;
    case DRAW_TYPE_OUTLINE:
    case DRAW_TYPE_OUTLINE_ALPHA:
    case DRAW_TYPE_ALPHA:
        pixelFormat = GX_PF_RGBA6_Z24;
        break;
    }

    GXColor black = {0, 0, 0, 0};
    DrawShadow(black, pixelFormat);
    Graphics::SetPixelFormat(pixelFormat, GX_ZC_LINEAR);

    int cameraIndex = 0;
    BWorldCamera* pCurrentCamera = mCurrentCamera.lock().get();
    if (pCurrentCamera != nullptr) {
        cameraIndex = pCurrentCamera->GetIndex();
    }
    nw4r::g3d::Camera camera = mScnRoot->GetCamera(cameraIndex);
    mScnRoot->SetCurrentCamera(cameraIndex);

    RemoveAllChildren(mScnRoot.get());
    mScnRoot->PushBack(mModelGroup.get());
    mScnRoot->PushBack(mOutlineGroup.get());
    mScnRoot->CalcView();

    RemoveAllChildren(mScnRoot.get());
    mScnRoot->PushBack(mOutlineGroup.get());
    mScnRoot->GatherDrawScnObj();
    mScnRoot->ZSort();

    GXSetColorUpdate(GX_TRUE);
    switch (mDrawType) {
    default:
        GXSetAlphaUpdate(GX_FALSE);
        break;
    case DRAW_TYPE_OUTLINE:
    case DRAW_TYPE_OUTLINE_ALPHA:
    case DRAW_TYPE_ALPHA:
        GXSetAlphaUpdate(GX_TRUE);
        break;
    }
    nw4r::g3d::G3DState::Invalidate(0x7FF);
    mScnRoot->DrawOpa();
    if (mOutlineData) {
        mOutlineData->mTexture.CopyFromEFB(GX_CTF_A8, false);
    }
    mScnRoot->DrawXlu();

    RemoveAllChildren(mScnRoot.get());
    mScnRoot->PushBack(mModelGroup.get());
    mScnRoot->GatherDrawScnObj();
    mScnRoot->ZSort();

    GXSetColorUpdate(GX_TRUE);
    switch (mDrawType) {
    default:
        GXSetAlphaUpdate(GX_FALSE);
        break;
    case DRAW_TYPE_ALPHA:
        GXSetAlphaUpdate(GX_TRUE);
        break;
    }
    nw4r::g3d::G3DState::Invalidate(0x7FF);
    mScnRoot->DrawOpa();
    mScnRoot->DrawXlu();

    if (mOutlineData) {
        DrawOutline();
    }

    if (HasEffectData()) {
        camera.GXSetViewport();
        camera.GXSetProjection();
        camera.GXSetScissor();
        camera.GXSetScissorBoxOffset();

        nw4r::math::MTX34 cameraMtx;
        camera.GetCameraMtx(&cameraMtx);
        nw4r::math::VEC3 cameraPos;
        camera.GetPosition(&cameraPos);

        nw4r::ef::EffectSystem* pSystem = nw4r::ef::EffectSystem::GetInstance();
        pSystem->SetProcessCamera(cameraPos, cameraMtx, camera.ref().projNear, camera.ref().projFar);

        int steps = gFrameStep * 60 / 60;
        if (!(mFlags & FLAG_SKIP_STATE_4)) {
            for (int i = 0; i < steps; i++) {
                if (!(mFlags & FLAG_PAUSED) && !(mFlags & FLAG_SKIP_STATE_2)) {
                    pSystem->Calc(mEffectData->mGroupID, 0);
                    if (mEffectData->mCaptureGroupID != EFFECT_GROUP_INVALID) {
                        pSystem->Calc(mEffectData->mCaptureGroupID, 0);
                    }
                }
                if (mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
                    pSystem->Calc(mEffectData->mPauseGroupID, 0);
                }
            }
        }

        GXLoadPosMtxImm(cameraMtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        camera.GXSetProjection();

        nw4r::ef::DrawInfo drawInfo;
        nw4r::math::MTX34 viewMtx;
        camera.GetCameraMtx(&viewMtx);
        drawInfo.SetViewMtx(viewMtx);

        const nw4r::g3d::CameraData& rData = camera.ref();
        switch (rData.flags & (nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM | nw4r::g3d::CameraData::FLAG_PROJ_PERSP |
                               nw4r::g3d::CameraData::FLAG_PROJ_ORTHO)) {
        case nw4r::g3d::CameraData::FLAG_PROJ_FRUSTUM: {
            nw4r::math::MTX34 projMtx;
            f32 near = rData.projNear;
            f32 right = rData.projRight;
            f32 left = rData.projLeft;
            f32 bottom = rData.projBottom;
            f32 top = rData.projTop;
            C_MTXLightFrustum(projMtx, top, bottom, left, right, near, 0.5f, -0.5f, 0.5f, 0.5f);
            drawInfo.SetProjMtx(projMtx);
            break;
        }
        case nw4r::g3d::CameraData::FLAG_PROJ_PERSP: {
            nw4r::math::MTX34 projMtx;
            f32 aspect = rData.projAspect;
            f32 fovy = rData.projFovy;
            C_MTXLightPerspective(projMtx, fovy, aspect, 0.5f, -0.5f, 0.5f, 0.5f);
            drawInfo.SetProjMtx(projMtx);
            break;
        }
        }

        pSystem->Draw(drawInfo, mEffectData->mGroupID);
        if (mEffectData->mPauseGroupID != EFFECT_GROUP_INVALID) {
            pSystem->Draw(drawInfo, mEffectData->mPauseGroupID);
        }
        if (mEffectData->mCaptureEnable) {
            mEffectData->mCaptureTexture.CopyFromEFB(false);
        }
        if (mEffectData->mCaptureGroupID != EFFECT_GROUP_INVALID) {
            pSystem->Draw(drawInfo, mEffectData->mCaptureGroupID);
        }
    }

    SetCopyClear(GXColor(), GX_PF_RGBA6_Z24);
}

namespace {

void DrawFrame(int width, int height) {
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, 0.0f, -1.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();

    SetupFlatColor(nw4r::ut::Color(0));
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);

    Mtx identity;
    PSMTXIdentity(identity);
    GXLoadPosMtxImm(identity, GX_PNMTX0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetLineWidth(6, GX_TO_ZERO);

    f32 bottom = height;
    f32 right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_LINESTRIP, GX_VTXFMT7, 5);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXPosition3f32(right, 0.5f, 0.0f);
    GXPosition3f32(right, bottom, 0.0f);
    GXPosition3f32(0.5f, bottom, 0.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
}

void DrawTexture(int width, int height, const GXTexObj* pTexObj) {
    nw4r::g3d::CameraData cameraData;
    nw4r::g3d::Camera camera(&cameraData);
    camera.Init();
    camera.SetOrtho(0.0f, height, 0.0f, width, 0.0f, -1.0f);
    camera.SetViewport(0.0f, 0.0f, width, height);
    camera.GXSetViewport();
    camera.GXSetProjection();
    camera.GXSetScissor();
    camera.GXSetScissorBoxOffset();

    GXSetNumChans(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTexGens(1);
    GXLoadTexObj(pTexObj, GX_TEXMAP0);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_COPY);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);

    Mtx identity;
    PSMTXIdentity(identity);
    GXLoadPosMtxImm(identity, GX_PNMTX0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT7, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    f32 bottom = height;
    f32 right = width;
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT7, 4);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(right, 0.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(right, bottom, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.0f, bottom, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

}

bool BWorld::DrawShadow(GXColor clearColor, GXPixelFmt pixelFormat) {
    CShadowCamera* pShadowCamera = mShadowCamera.lock().get();
    if (pShadowCamera == nullptr || !pShadowCamera->IsVisible()) {
        return false;
    }

    Graphics::SetPixelFormat(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
    mScnRoot->SetCurrentCamera(pShadowCamera->GetIndex());
    GXSetColorUpdate(GX_FALSE);
    GXSetAlphaUpdate(GX_TRUE);

    RemoveAllChildren(mScnRoot.get());
    mScnRoot->PushBack(mOutlineGroup.get());
    mScnRoot->CalcView();
    mScnRoot->GatherDrawScnObj();
    mScnRoot->ZSort();

    if (pShadowCamera->IsTextureAlpha()) {
        mScnRoot->DrawXlu();
        mScnRoot->DrawOpa();
    } else {
        mScnRoot->ForceDrawMode(true, nw4r::g3d::RESMDL_DRAWMODE_IGNORE_MATERIAL);

        nw4r::ut::Color shadowColor(0, 0, 0, pShadowCamera->GetAlpha());
        SetupFlatColor(shadowColor);
        mScnRoot->DrawXlu();
        mScnRoot->DrawOpa();
        mScnRoot->ForceDrawMode(false, nw4r::g3d::RESMDL_DRAWMODE_DEFAULT);
    }

    CaptureTexture& rFirstTexture = pShadowCamera->GetTexture(0);
    DrawFrame(rFirstTexture.GetWidth(), rFirstTexture.GetHeight());

    const GXRenderModeObj* pRenderMode = nw4r::g3d::G3DState::GetRenderModeObj();
    GXSetCopyFilter(GX_FALSE, nullptr, GX_FALSE, nullptr);

    int levels = pShadowCamera->GetTextureLevels();
    if (levels == 1) {
        SetCopyClear(clearColor, pixelFormat);
    } else {
        SetCopyClear(GXColor(), GX_PF_RGBA6_Z24);
    }
    pShadowCamera->GetTexture(0).CopyFromEFB(GX_CTF_A8, true);
    GXInvalidateTexAll();

    for (int i = 1; i < levels; i++) {
        CaptureTexture& rTexture = pShadowCamera->GetTexture(i);
        u16 width = rTexture.GetWidth();
        u16 height = rTexture.GetHeight();
        DrawTexture(width, height, &pShadowCamera->GetTexture(i - 1).GetTexObj());
        if (i == levels - 1) {
            DrawFrame(width, height);
            SetCopyClear(clearColor, pixelFormat);
        } else {
            SetCopyClear(GXColor(), GX_PF_RGBA6_Z24);
        }
        rTexture.CopyFromEFB(GX_CTF_A8, true);
        GXInvalidateTexAll();
    }

    GXSetCopyFilter(pRenderMode->aa, pRenderMode->sample_pattern, GX_TRUE, pRenderMode->vfilter);
    return true;
}
