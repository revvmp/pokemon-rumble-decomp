#include <game/BWorld.h>

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
    nw4r::ef::ResTexture texture(mTexture);
    if (!texture.IsValid()) {
        return;
    }

    texture.SetWidth(mWidth);
    texture.SetHeight(mHeight);
    texture.SetFormat(mFormat);
    texture.SetMipmap(mMipmap);
    texture.SetTexture(mpTexture);
    if (mHasTlut) {
        texture.SetTlutFormat(mTlutFormat);
        texture.SetTlutEntries(mTlutEntries);
        texture.SetTlut(mpTlut);
    }
}