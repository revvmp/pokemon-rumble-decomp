#include <game/BWorld.h>

#include <game/AnmObjChrRootScale.h>
#include <game/CKDTree.h>
#include <game/EffectGroupID.h>
#include <game/Memory.h>
#include <game/WorkBuffer.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/gx.h>

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
