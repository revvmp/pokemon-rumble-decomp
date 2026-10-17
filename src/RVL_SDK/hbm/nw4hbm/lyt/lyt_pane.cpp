#include "nw4hbm/lyt/lyt_pane.h"
#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_drawInfo.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"

#include "revolution/gx/GXTransform.h"

#include <string.h>

namespace nw4hbm {
namespace lyt {
namespace {

void ReverseYAxis(math::MTX34* pMtx) {
    pMtx->m[0][1] = -pMtx->m[0][1];
    pMtx->m[1][1] = -pMtx->m[1][1];
    pMtx->m[2][1] = -pMtx->m[2][1];
}

}

NW4R_UT_RTTI_DEF_BASE(Pane);

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
}

Pane::~Pane() {
    for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter();) {
        PaneList::Iterator currIt = it++;
        mChildList.Erase(currIt);

        if (!currIt->IsUserAllocated()) {
            currIt->~Pane();
            Layout::FreeMemory(&*currIt);
        }
    }

    UnbindAnimationSelf(nullptr);

    if (mpMaterial && !mpMaterial->IsUserAllocated()) {
        mpMaterial->~Material();
        Layout::FreeMemory(mpMaterial);
    }
}

void Pane::SetName(const char* pName) {
    strncpy(mName, pName, sizeof(mName));
}

void Pane::SetUserData(const char* pUserData) {
    strncpy(mUserData, pUserData, sizeof(mUserData));
}

void Pane::AppendChild(Pane* pChild) {
    InsertChild(mChildList.GetEndIter(), pChild);
}

void Pane::InsertChild(PaneList::Iterator next, Pane* pChild) {
    mChildList.Insert(next, pChild);
    pChild->mpParent = this;
}

ut::Rect Pane::GetPaneRect(const DrawInfo& rInfo) const {
    ut::Rect rect;
    math::VEC2 basePt = GetVtxPos();

    rect.left = basePt.x;
    rect.top = basePt.y;
    rect.right = rect.left + mSize.width;
    rect.bottom = rect.top + mSize.height;

    if (rInfo.IsYAxisUp()) {
        rect.top = -rect.top;
        rect.bottom = -rect.bottom;
    }

    return rect;
}

ut::Color Pane::GetVtxColor(u32) const {
    return ut::Color(0xFFFFFFFF);
}

void Pane::SetVtxColor(u32, ut::Color) {}

u8 Pane::GetColorElement(u32 idx) const {
    switch (idx) {
    case ANIMTARGET_PANE_COLOR_ALPHA:
        return mAlpha;
    default:
        return GetVtxColorElement(idx);
    }
}

void Pane::SetColorElement(u32 idx, u8 value) {
    switch (idx) {
    case ANIMTARGET_PANE_COLOR_ALPHA:
        mAlpha = value;
        break;
    default:
        SetVtxColorElement(idx, value);
        break;
    }
}

u8 Pane::GetVtxColorElement(u32) const {
    return 0xFF;
}

void Pane::SetVtxColorElement(u32, u8) {}

Pane* Pane::FindPaneByName(const char* pName, bool recursive) {
    if (detail::EqualsPaneName(mName, pName)) {
        return this;
    }

    if (recursive) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            if (Pane* pPane = it->FindPaneByName(pName, true)) {
                return pPane;
            }
        }
    }

    return nullptr;
}

Material* Pane::FindMaterialByName(const char* pName, bool recursive) {
    if (mpMaterial && detail::EqualsMaterialName(mpMaterial->GetName(), pName)) {
        return mpMaterial;
    }

    if (recursive) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            if (Material* pMaterial = it->FindMaterialByName(pName, true)) {
                return pMaterial;
            }
        }
    }

    return nullptr;
}

void Pane::CalculateMtx(const DrawInfo& rInfo) {
    const f32 invAlpha = 1.0f / 255.0f;

    if (!IsVisible() && !rInfo.IsInvisiblePaneCalculateMtx()) {
        return;
    }

    {
        math::MTX34 mtx1;
        math::MTX34 mtx2;
        math::MTX34 rotateMtx;

        {
            math::VEC2 scale(mScale);

            if (rInfo.IsLocationAdjust() && IsLocationAdjust()) {
                scale.x *= rInfo.GetLocationAdjustScale().x;
                scale.y *= rInfo.GetLocationAdjustScale().y;
            }

            PSMTXScale(mtx2, scale.x, scale.y, 1.0f);
        }

        PSMTXRotRad(rotateMtx, 'x', NW4R_MATH_DEG_TO_RAD(mRotate.x));
        PSMTXConcat(rotateMtx, mtx2, mtx1);

        PSMTXRotRad(rotateMtx, 'y', NW4R_MATH_DEG_TO_RAD(mRotate.y));
        PSMTXConcat(rotateMtx, mtx1, mtx2);

        PSMTXRotRad(rotateMtx, 'z', NW4R_MATH_DEG_TO_RAD(mRotate.z));
        PSMTXConcat(rotateMtx, mtx2, mtx1);

        PSMTXTransApply(mtx1, mMtx, mTranslate.x, mTranslate.y, mTranslate.z);
    }

    if (mpParent) {
        math::MTX34Mult(&mGlbMtx, &mpParent->mGlbMtx, &mMtx);
    } else if (rInfo.IsMultipleViewMtxOnDraw()) {
        mGlbMtx = mMtx;
    } else {
        math::MTX34Mult(&mGlbMtx, &rInfo.GetViewMtx(), &mMtx);
    }

    if (rInfo.IsInfluencedAlpha() && mpParent) {
        mGlbAlpha = mAlpha * rInfo.GetGlobalAlpha();
    } else {
        mGlbAlpha = mAlpha;
    }

    f32 glbAlpha = rInfo.GetGlobalAlpha();
    bool influenced = rInfo.IsInfluencedAlpha();
    bool modDrawInfo = IsInfluencedAlpha() && mAlpha != 0xFF;

    if (modDrawInfo) {
        DrawInfo& rMutInfo = const_cast<DrawInfo&>(rInfo);
        rMutInfo.SetGlobalAlpha(glbAlpha * mAlpha * invAlpha);
        rMutInfo.SetInfluencedAlpha(true);
    }

    CalculateMtxChild(rInfo);

    if (modDrawInfo) {
        DrawInfo& rMutInfo = const_cast<DrawInfo&>(rInfo);
        rMutInfo.SetGlobalAlpha(glbAlpha);
        rMutInfo.SetInfluencedAlpha(influenced);
    }
}

void Pane::CalculateMtxChild(const DrawInfo& rInfo) {
    for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
        it->CalculateMtx(rInfo);
    }
}

void Pane::Draw(const DrawInfo& rInfo) {
    if (IsVisible()) {
        DrawSelf(rInfo);

        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            it->Draw(rInfo);
        }
    }
}

void Pane::DrawSelf(const DrawInfo& rInfo) {
    if (mpParent && rInfo.IsDebugDrawMode()) {
        LoadMtx(rInfo);
        detail::DrawLine(GetVtxPos(), mSize, ut::Color(0x00FF00FF));
    }
}

void Pane::Animate(u32 option) {
    AnimateSelf(option);

    if (IsVisible() || !(option & ANIMOPTION_SKIP_INVISIBLE)) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            it->Animate(option);
        }
    }
}

void Pane::AnimateSelf(u32 option) {
    for (AnimationLinkList::Iterator it = mAnimList.GetBeginIter(); it != mAnimList.GetEndIter(); ++it) {
        if (it->IsEnable()) {
            AnimTransform* pAnimTrans = it->GetAnimTransform();
            pAnimTrans->Animate(it->GetIndex(), this);
        }
    }

    if ((IsVisible() || !(option & ANIMOPTION_SKIP_INVISIBLE)) && mpMaterial) {
        mpMaterial->Animate();
    }
}

void Pane::BindAnimation(AnimTransform* pAnimTrans, bool recursive) {
    pAnimTrans->Bind(this, recursive);
}

void Pane::UnbindAnimation(AnimTransform* pAnimTrans, bool recursive) {
    UnbindAnimationSelf(pAnimTrans);

    if (recursive) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            it->UnbindAnimation(pAnimTrans, recursive);
        }
    }
}

void Pane::UnbindAllAnimation(bool recursive) {
    UnbindAnimation(nullptr, recursive);
}

void Pane::UnbindAnimationSelf(AnimTransform* pAnimTrans) {
    if (mpMaterial) {
        mpMaterial->UnbindAnimation(pAnimTrans);
    }

    for (AnimationLinkList::Iterator it = mAnimList.GetBeginIter(); it != mAnimList.GetEndIter();) {
        AnimationLinkList::Iterator currIt = it++;

        if (pAnimTrans == nullptr || currIt->GetAnimTransform() == pAnimTrans) {
            mAnimList.Erase(currIt);
            currIt->Reset();
        }
    }
}

void Pane::AddAnimationLink(AnimationLink* pAnimLink) {
    mAnimList.PushBack(pAnimLink);
}

AnimationLink* Pane::FindAnimationLink(AnimTransform* pAnimTrans) {
    if (AnimationLink* pAnimLink = detail::FindAnimationLink(&mAnimList, pAnimTrans)) {
        return pAnimLink;
    }

    if (mpMaterial) {
        if (AnimationLink* pAnimLink = mpMaterial->FindAnimationLink(pAnimTrans)) {
            return pAnimLink;
        }
    }

    return nullptr;
}

void Pane::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable, bool recursive) {
    if (AnimationLink* pAnimLink = detail::FindAnimationLink(&mAnimList, pAnimTrans)) {
        pAnimLink->SetEnable(enable);
    }

    if (mpMaterial) {
        mpMaterial->SetAnimationEnable(pAnimTrans, enable);
    }

    if (recursive) {
        for (PaneList::Iterator it = mChildList.GetBeginIter(); it != mChildList.GetEndIter(); ++it) {
            it->SetAnimationEnable(pAnimTrans, enable, recursive);
        }
    }
}

void Pane::LoadMtx(const DrawInfo& rInfo) {
    math::MTX34 mtx;
    MtxPtr pMtx = nullptr;

    if (rInfo.IsMultipleViewMtxOnDraw()) {
        math::MTX34Mult(&mtx, &rInfo.GetViewMtx(), &mGlbMtx);

        if (rInfo.IsYAxisUp()) {
            ReverseYAxis(&mtx);
        }

        pMtx = mtx;
    } else if (rInfo.IsYAxisUp()) {
        math::MTX34Copy(&mtx, &mGlbMtx);
        ReverseYAxis(&mtx);
        pMtx = mtx;
    } else {
        pMtx = mGlbMtx;
    }

    GXLoadPosMtxImm(pMtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

math::VEC2 Pane::GetVtxPos() const {
    math::VEC2 basePt(0.0f, 0.0f);

    switch (mBasePosition % HORIZONTALPOSITION_MAX) {
    default:
    case HORIZONTALPOSITION_LEFT:
        basePt.x = 0.0f;
        break;
    case HORIZONTALPOSITION_CENTER:
        basePt.x = -mSize.width / 2.0f;
        break;
    case HORIZONTALPOSITION_RIGHT:
        basePt.x = -mSize.width;
        break;
    }

    switch (mBasePosition / HORIZONTALPOSITION_MAX) {
    default:
    case VERTICALPOSITION_TOP:
        basePt.y = 0.0f;
        break;
    case VERTICALPOSITION_CENTER:
        basePt.y = -mSize.height / 2.0f;
        break;
    case VERTICALPOSITION_BOTTOM:
        basePt.y = -mSize.height;
        break;
    }

    return basePt;
}

Material* Pane::GetMaterial() const {
    return mpMaterial;
}

}
}
