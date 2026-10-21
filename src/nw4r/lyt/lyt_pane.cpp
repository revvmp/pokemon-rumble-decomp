#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_animation.h>
#include <nw4r/lyt/lyt_drawInfo.h>
#include <nw4r/lyt/lyt_resources.h>

#include <revolution/GX.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/MTX.h>

#include <string.h>

namespace nw4r {
namespace lyt {

NW4R_UT_RTTI_DEF_BASE(Pane);

namespace detail {

PaneBase::PaneBase() {}

PaneBase::~PaneBase() {}

}

Pane::Pane() {
    Init();
    mBasePosition = HORIZONTALPOSITION_MAX + 1;
    memset(mName, 0, sizeof(mName));
    memset(mUserData, 0, sizeof(mUserData));
    mTranslate = math::VEC3(0.0f, 0.0f, 0.0f);
    mRotate = math::VEC3(0.0f, 0.0f, 0.0f);
    mScale = math::VEC2(1.0f, 1.0f);
    mSize = Size(0.0f, 0.0f);
    mAlpha = 0xFF;
    mGlbAlpha = 0xFF;
    mFlag = 0;
    SetVisible(true);
}

Pane::Pane(const res::Pane* pRes) {
    Init();

    mBasePosition = pRes->basePosition;

    SetName(pRes->name);
    SetUserData(pRes->userData);

    mTranslate = pRes->translate;
    mRotate = pRes->rotate;
    mScale = pRes->scale;
    mSize = pRes->size;

    mAlpha = pRes->alpha;
    mGlbAlpha = mAlpha;
    mFlag = pRes->flag;
}

void Pane::Init() {
    mpParent = nullptr;
    mpMaterial = nullptr;
    mbUserAllocated = false;
    mpExtUserDataList = nullptr;
}

Pane::~Pane() {
    NW4R_UT_LINKLIST_FOREACH_SAFE (it, mChildList, {
        mChildList.Erase(it);

        if (!it->IsUserAllocated()) {
            Layout::DeleteObj(&*it);
        }
    })

    UnbindAnimationSelf(nullptr);

    if (mpMaterial != nullptr && !mpMaterial->IsUserAllocated()) {
        Layout::DeleteObj(mpMaterial);
    }
}

void Pane::SetName(const char* pName) {
    strncpy(mName, pName, 16);
    mName[16] = '\0';
}

void Pane::SetUserData(const char* pUserData) {
    strncpy(mUserData, pUserData, 8);
    mUserData[8] = '\0';
}

void Pane::AppendChild(Pane* pChild) {
    InsertChild(mChildList.GetEndIter(), pChild);
}

void Pane::InsertChild(PaneList::Iterator next, Pane* pChild) {
    mChildList.Insert(next, pChild);
    pChild->mpParent = this;
}

void Pane::PrependChild(Pane* pChild) {
    mChildList.Insert(mChildList.GetBeginIter(), pChild);
    pChild->mpParent = this;
}

void Pane::RemoveChild(Pane* pChild) {
    mChildList.Erase(pChild);
    pChild->mpParent = nullptr;
}

ut::Rect Pane::GetPaneRect() const {
    ut::Rect rect;
    math::VEC2 base = GetVtxPos();

    rect.left = base.x;
    rect.top = base.y;
    rect.right = base.x + mSize.width;
    rect.bottom = base.y - mSize.height;

    return rect;
}

ut::Color Pane::GetVtxColor(u32 idx) const {
#pragma unused(idx)

    return ut::Color::WHITE;
}

void Pane::SetVtxColor(u32 idx, ut::Color color) {
#pragma unused(idx)
#pragma unused(color)
}

u8 Pane::GetColorElement(u32 idx) const {
    switch (idx) {
    case ANIMTARGET_PANE_COLOR_ALPHA: {
        return mAlpha;
    }

    default: {
        return GetVtxColorElement(idx);
    }
    }
}

void Pane::SetColorElement(u32 idx, u8 value) {
    switch (idx) {
    case ANIMTARGET_PANE_COLOR_ALPHA: {
        mAlpha = value;
        break;
    }

    default: {
        SetVtxColorElement(idx, value);
        break;
    }
    }
}

u8 Pane::GetVtxColorElement(u32 idx) const {
#pragma unused(idx)

    return 0xFF;
}

void Pane::SetVtxColorElement(u32 idx, u8 value) {
#pragma unused(idx)
#pragma unused(value)
}

Pane* Pane::FindPaneByName(const char* pName, bool recursive) {
    if (detail::EqualsResName(mName, pName)) {
        return this;
    }

    if (recursive) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList, {
            Pane* pResult = it->FindPaneByName(pName, recursive);

            if (pResult != nullptr) {
                return pResult;
            }
        })
    }

    return nullptr;
}

Material* Pane::FindMaterialByName(const char* pName, bool recursive) {
    if (mpMaterial != nullptr &&
        detail::EqualsMaterialName(mpMaterial->GetName(), pName)) {

        return mpMaterial;
    }

    if (recursive) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList, {
            Material* pResult = it->FindMaterialByName(pName, recursive);

            if (pResult != nullptr) {
                return pResult;
            }
        })
    }

    return nullptr;
}

void Pane::CalculateMtx(const DrawInfo& rInfo) {
    if (!IsVisibleForDraw() && !rInfo.IsInvisiblePaneCalculateMtx()) {
        return;
    }

    math::MTX34 mtx1, mtx2;
    math::MTX34 rotateMtx;

    math::VEC2 scale = mScale;
    if (rInfo.IsLocationAdjust() && IsLocationAdjust()) {
        scale.x *= rInfo.GetLocationAdjustScale().x;
        scale.y *= rInfo.GetLocationAdjustScale().y;
    }

    PSMTXScale(mtx2, scale.x, scale.y, 1.0f);

    PSMTXRotRad(rotateMtx, 'x', NW4R_MATH_DEG_TO_RAD(mRotate.x));
    PSMTXConcat(rotateMtx, mtx2, mtx1);

    PSMTXRotRad(rotateMtx, 'y', NW4R_MATH_DEG_TO_RAD(mRotate.y));
    PSMTXConcat(rotateMtx, mtx1, mtx2);

    PSMTXRotRad(rotateMtx, 'z', NW4R_MATH_DEG_TO_RAD(mRotate.z));
    PSMTXConcat(rotateMtx, mtx2, mtx1);

    PSMTXTransApply(mtx1, mMtx, mTranslate.x, mTranslate.y, mTranslate.z);

    if (mpParent != nullptr) {
        math::MTX34Mult(&mGlbMtx, &mpParent->mGlbMtx, &mMtx);
    } else if (rInfo.IsMultipleViewMtxOnDraw()) {
        mGlbMtx = mMtx;
    } else {
        math::MTX34Mult(&mGlbMtx, &rInfo.GetViewMtx(), &mMtx);
    }

    if (rInfo.IsInfluencedAlpha() && mpParent != nullptr) {
        mGlbAlpha = static_cast<u8>(mAlpha * rInfo.GetGlobalAlpha());
    } else {
        mGlbAlpha = mAlpha;
    }

    f32 glbAlpha = rInfo.GetGlobalAlpha();
    bool influenced = rInfo.IsInfluencedAlpha();
    bool modifyInfo = IsInfluencedAlpha() && mAlpha != 255;

    if (modifyInfo) {
        DrawInfo& rMtInfo = const_cast<DrawInfo&>(rInfo);
        rMtInfo.SetGlobalAlpha(glbAlpha * mAlpha * (1.0f / 255.0f));
        rMtInfo.SetInfluencedAlpha(true);
    }

    CalculateMtxChild(rInfo);

    if (modifyInfo) {
        DrawInfo& rMtInfo = const_cast<DrawInfo&>(rInfo);
        rMtInfo.SetGlobalAlpha(glbAlpha);
        rMtInfo.SetInfluencedAlpha(influenced);
    }
}

void Pane::CalculateMtxChild(const DrawInfo& rInfo) {
    NW4R_UT_LINKLIST_FOREACH (it, mChildList, { it->CalculateMtx(rInfo); })
}

void Pane::Draw(const DrawInfo& rInfo) {
    if (IsVisibleForDraw()) {
        if (GetAlpha() != 0) {
            DrawSelf(rInfo);
        }
        NW4R_UT_LINKLIST_FOREACH (it, mChildList, { it->Draw(rInfo); })
    }
}

void Pane::DrawSelf(const DrawInfo& rInfo) {
#pragma unused(rInfo)

}

void Pane::Animate(u32 option) {
    AnimateSelf(option);

    if (IsVisible() || !(option & ANIMOPTION_SKIP_INVISIBLE)) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList, { it->Animate(option); })
    }
}

void Pane::AnimateSelf(u32 option) {
    NW4R_UT_LINKLIST_FOREACH (it, mAnimList, {
        if (!it->IsEnable()) {
            continue;
        }

        AnimTransform* pAnimTrans = it->GetAnimTransform();
        pAnimTrans->Animate(it->GetIndex(), this);
    })

    if (IsVisible() || !(option & ANIMOPTION_SKIP_INVISIBLE)) {
        if (mpMaterial != nullptr) {
            mpMaterial->Animate();
        }
    }
}

void Pane::BindAnimation(AnimTransform* pAnimTrans, bool recursive, bool disable) {
    pAnimTrans->Bind(this, recursive, disable);
}

void Pane::UnbindAnimation(AnimTransform* pAnimTrans, bool recursive) {
    UnbindAnimationSelf(pAnimTrans);

    if (recursive) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList,
            { it->UnbindAnimation(pAnimTrans, recursive); })
    }
}

void Pane::UnbindAllAnimation(bool recursive) {
    UnbindAnimation(nullptr, recursive);
}

void Pane::UnbindAnimationSelf(AnimTransform* pAnimTrans) {
    if (mpMaterial != nullptr) {
        mpMaterial->UnbindAnimation(pAnimTrans);
    }

    detail::UnbindAnimationLink(&mAnimList, pAnimTrans);
}

void Pane::AddAnimationLink(AnimationLink* pAnimLink) {
    mAnimList.PushBack(pAnimLink);
}

AnimationLink* Pane::FindAnimationLinkSelf(AnimTransform* pAnimTrans) {
    return detail::FindAnimationLink(&mAnimList, pAnimTrans);
}

AnimationLink* Pane::FindAnimationLinkSelf(const AnimResource& rResource) {
    return detail::FindAnimationLink(&mAnimList, rResource);
}

void Pane::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable,
                              bool recursive) {

    AnimationLink* pAnimLink = FindAnimationLinkSelf(pAnimTrans);

    if (pAnimLink != nullptr) {
        pAnimLink->SetEnable(enable);
    }

    u8 materialNum = GetMaterialNum();

    for (u8 i = 0; i < materialNum; i++) {
        GetMaterial(i)->SetAnimationEnable(pAnimTrans, enable);
    }

    if (recursive) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList,
            { it->SetAnimationEnable(pAnimTrans, enable, recursive); })
    }
}

void Pane::SetAnimationEnable(const AnimResource& rResource, bool enable,
                              bool recursive) {

    AnimationLink* pAnimLink = FindAnimationLinkSelf(rResource);

    if (pAnimLink != nullptr) {
        pAnimLink->SetEnable(enable);
    }

    u8 materialNum = GetMaterialNum();

    for (u8 i = 0; i < materialNum; i++) {
        GetMaterial(i)->SetAnimationEnable(rResource, enable);
    }

    if (recursive) {
        NW4R_UT_LINKLIST_FOREACH (it, mChildList,
            { it->SetAnimationEnable(rResource, enable, recursive); })
    }

}

void Pane::LoadMtx(const DrawInfo& rInfo) {
    math::MTX34 mtx;
    math::MTX34* pMtx = nullptr;

    if (rInfo.IsMultipleViewMtxOnDraw()) {
        math::MTX34Mult(&mtx, &rInfo.GetViewMtx(), &mGlbMtx);
        pMtx = &mtx;
    } else {
        pMtx = &mGlbMtx;
    }

    GXLoadPosMtxImm(*pMtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

math::VEC2 Pane::GetVtxPos() const {
    math::VEC2 base(0.0f, 0.0f);

    switch (mBasePosition % HORIZONTALPOSITION_MAX) {
    default:
    case HORIZONTALPOSITION_LEFT: {
        base.x = 0.0f;
        break;
    }

    case HORIZONTALPOSITION_CENTER: {
        base.x = -mSize.width / 2;
        break;
    }

    case HORIZONTALPOSITION_RIGHT: {
        base.x = -mSize.width;
        break;
    }
    }

    switch (mBasePosition / HORIZONTALPOSITION_MAX) {
    default:
    case VERTICALPOSITION_TOP: {
        base.y = 0.0f;
        break;
    }

    case VERTICALPOSITION_CENTER: {
        base.y = mSize.height / 2;
        break;
    }

    case VERTICALPOSITION_BOTTOM: {
        base.y = mSize.height;
        break;
    }
    }

    return base;
}

u8 Pane::GetMaterialNum() const {
    return mpMaterial != nullptr ? 1 : 0;
}

Material* Pane::GetMaterial() const {
    return mpMaterial;
}

Material* Pane::GetMaterial(u32 index) const {
    if (index == 0) {
        return GetMaterial();
    }
    return nullptr;
}

const res::ExtUserData* Pane::GetExtUserData() const {
    if (mpExtUserDataList != nullptr) {
        return detail::ConvertOffsToPtr<res::ExtUserData>(mpExtUserDataList, sizeof(res::ExtUserDataList));
    }
    return nullptr;
}

const res::ExtUserData* Pane::FindExtUserDataByName(const char* pName) {
    const res::ExtUserData* pUserData = GetExtUserData();
    if (pUserData == nullptr) {
        return nullptr;
    }
    u16 count = mpExtUserDataList->num;
    for (int i = 0; i < count; i++, pUserData++) {
        if (strcmp(pName, pUserData->GetName()) == 0) {
            return pUserData;
        }
    }
    return nullptr;
}

void Pane::SetExtUserDataList(const res::ExtUserDataList* pList) {
    mpExtUserDataList = pList;
}

}
}
