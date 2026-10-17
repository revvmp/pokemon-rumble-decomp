#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"
#include "nw4hbm/lyt/lyt_pane.h"
#include "nw4hbm/lyt/lyt_resourceAccessor.h"

#include "revolution/os/OSFastCast.h"

#include <string.h>

inline void* operator new(size_t, void* p) { return p; }

namespace {

using namespace nw4hbm;
using namespace nw4hbm::lyt;

inline bool RIsSame(const f32 a, const f32 b, const f32 tolerance) {
    f32 c = a - b;
    return -tolerance < c && c < tolerance;
}

u16 GetStepCurveValue(f32 frame, const res::StepKey* keyArray, u32 keySize) {
    if (keySize == 1 || frame <= keyArray[0].frame) {
        return keyArray[0].value;
    }

    if (frame >= keyArray[keySize - 1].frame) {
        return keyArray[keySize - 1].value;
    }

    int ikeyL = 0;
    int ikeyR = keySize - 1;

    while (ikeyL != ikeyR - 1 && ikeyL != ikeyR) {
        int ikeyCenter = (ikeyL + ikeyR) / 2;
        const res::StepKey& centerKey = keyArray[ikeyCenter];

        if (frame < centerKey.frame) {
            ikeyR = ikeyCenter;
        } else {
            ikeyL = ikeyCenter;
        }
    }

    if (RIsSame(frame, keyArray[ikeyR].frame, 0.001f)) {
        return keyArray[ikeyR].value;
    } else {
        return keyArray[ikeyL].value;
    }
}

f32 GetHermiteCurveValue(f32 frame, const res::HermiteKey* keyArray, u32 keySize) {
    if (keySize == 1 || frame <= keyArray[0].frame) {
        return keyArray[0].value;
    }

    if (frame >= keyArray[keySize - 1].frame) {
        return keyArray[keySize - 1].value;
    }

    int ikeyL = 0;
    int ikeyR = keySize - 1;

    while (ikeyL != ikeyR - 1 && ikeyL != ikeyR) {
        int ikeyCenter = (ikeyL + ikeyR) / 2;

        if (frame <= keyArray[ikeyCenter].frame) {
            ikeyR = ikeyCenter;
        } else {
            ikeyL = ikeyCenter;
        }
    }

    const res::HermiteKey& key0 = keyArray[ikeyL];
    const res::HermiteKey& key1 = keyArray[ikeyR];

    if (RIsSame(frame, key1.frame, 0.001f)) {
        if (ikeyR < keySize - 1 && keyArray[ikeyR + 1].frame == key1.frame) {
            return keyArray[ikeyR + 1].value;
        } else {
            return key1.value;
        }
    }

    f32 t1 = frame - key0.frame;
    f32 t2 = 1.0f / (key1.frame - key0.frame);
    f32 v0 = key0.value;
    f32 v1 = key1.value;
    f32 s0 = key0.slope;
    f32 s1 = key1.slope;

    f32 t1t1t2 = t1 * t1 * t2;
    f32 t1t1t2t2 = t1t1t2 * t2;
    f32 t1t1t1t2t2 = t1 * t1t1t2t2;
    f32 t1t1t1t2t2t2 = t1t1t1t2t2 * t2;

    return v0 * ((2.0f * t1t1t1t2t2t2) - (3.0f * t1t1t2t2) + 1.0f) +
           v1 * ((-2.0f * t1t1t1t2t2t2) + (3.0f * t1t1t2t2)) +
           s0 * ((t1t1t1t2t2) - (2.0f * t1t1t2) + t1) +
           s1 * ((t1t1t1t2t2) - (t1t1t2));
}

void AnimatePaneSRT(Pane* pPane, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);
        const res::HermiteKey* keys = detail::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);

        pPane->SetSRTElement(pAnimTarget->target, GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum));
    }
}

void AnimateVisibility(Pane* pPane, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);
        const res::StepKey* keys = detail::ConvertOffsToPtr<res::StepKey>(pAnimTarget, pAnimTarget->keysOffset);

        pPane->SetVisible(GetStepCurveValue(frame, keys, pAnimTarget->keyNum) != 0);
    }
}

void AnimateVertexColor(Pane* pPane, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);
        const res::HermiteKey* keys = detail::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);

        f32 value = GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum);
        value += 0.5f;

        u8 u8Val;
        OSf32tou8(&value, &u8Val);
        pPane->SetColorElement(pAnimTarget->target, u8Val);
    }
}

void AnimateMaterialColor(Material* pMaterial, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);
        const res::HermiteKey* keys = detail::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);

        f32 value = GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum);
        value += 0.5f;

        s16 s16Val;
        OSf32tos16(&value, &s16Val);
        s16Val = ut::Min<s16>(ut::Max<s16>(s16Val, -1024), 1023);
        pMaterial->SetColorElement(pAnimTarget->target, s16Val);
    }
}

void AnimateTextureSRT(Material* pMaterial, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        if (pAnimTarget->id < pMaterial->GetTexSRTCap()) {
            const res::HermiteKey* keys = detail::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
            pMaterial->SetTexSRTElement(pAnimTarget->id, pAnimTarget->target, GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum));
        }
    }
}

void AnimateTexturePattern(Material* pMaterial, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame, void** tpls) {
    for (int j = 0; j < pAnimInfo->num; j++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[j]);

        if (pAnimTarget->id < pMaterial->GetTextureNum() && pAnimTarget->target == 0) {
            const res::StepKey* keys = detail::ConvertOffsToPtr<res::StepKey>(pAnimTarget, pAnimTarget->keysOffset);
            u16 fileIdx = GetStepCurveValue(frame, keys, pAnimTarget->keyNum);
            pMaterial->SetTextureNoWrap(pAnimTarget->id, static_cast<TPLPalette*>(tpls[fileIdx]));
        }
    }
}

void AnimateIndTexSRT(Material* pMaterial, const res::AnimationInfo* pAnimInfo, const u32* animTargetOffsets, f32 frame) {
    for (int i = 0; i < pAnimInfo->num; i++) {
        const res::AnimationTarget* pAnimTarget = detail::ConvertOffsToPtr<res::AnimationTarget>(pAnimInfo, animTargetOffsets[i]);

        if (pAnimTarget->id < pMaterial->GetIndTexSRTCap()) {
            const res::HermiteKey* keys = detail::ConvertOffsToPtr<res::HermiteKey>(pAnimTarget, pAnimTarget->keysOffset);
            pMaterial->SetIndTexSRTElement(pAnimTarget->id, pAnimTarget->target, GetHermiteCurveValue(frame, keys, pAnimTarget->keyNum));
        }
    }
}

}

namespace nw4hbm {
namespace lyt {

u16 AnimTransform::GetFrameSize() const {
    return mpRes->frameSize;
}

AnimTransformBasic::AnimTransformBasic() : mpFileResAry(nullptr), mAnimLinkAry(nullptr), mAnimLinkNum(0) {}

AnimTransformBasic::~AnimTransformBasic() {
    if (mAnimLinkAry) {
        Layout::FreeMemory(mAnimLinkAry);
    }

    if (mpFileResAry) {
        Layout::FreeMemory(mpFileResAry);
    }
}

void AnimTransformBasic::SetResource(const res::AnimationBlock* pRes, ResourceAccessor* pResAccessor) {
    mpRes = pRes;
    mpFileResAry = nullptr;

    if (pRes->fileNum) {
        mpFileResAry = static_cast<void**>(Layout::AllocMemory(sizeof(*mpFileResAry) * pRes->fileNum));

        if (mpFileResAry) {
            const u32* fileNameOffsets = detail::ConvertOffsToPtr<u32>(mpRes, sizeof(*mpRes));

            for (int i = 0; i < mpRes->fileNum; i++) {
                mpFileResAry[i] = pResAccessor->GetResource('timg', detail::GetStrTableStr(fileNameOffsets, i), nullptr);
            }
        }
    }

    mAnimLinkAry = static_cast<AnimationLink*>(Layout::AllocMemory(sizeof(*mAnimLinkAry) * pRes->animContNum));

    if (mAnimLinkAry) {
        mAnimLinkNum = pRes->animContNum;
        memset(mAnimLinkAry, 0, sizeof(*mAnimLinkAry) * pRes->animContNum);

        for (u16 i = 0; i < pRes->animContNum; i++) {
            new (&mAnimLinkAry[i]) AnimationLink();
        }
    }
}

void AnimTransformBasic::Bind(Pane* pPane, bool bRecursive) {
    const u32* animContOffsets = detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset);

    for (u16 i = 0; i < mpRes->animContNum; i++) {
        const res::AnimationContent* pAnimCont = detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, animContOffsets[i]);

        if (pAnimCont->type == res::AnimationContent::ANIMTYPE_PANE) {
            Pane* pFindPane = pPane->FindPaneByName(pAnimCont->name, bRecursive);

            if (pFindPane) {
                mAnimLinkAry[i].SetAnimTransform(this, i);
                pFindPane->AddAnimationLink(&mAnimLinkAry[i]);
            }
        } else {
            Material* pFindMat = pPane->FindMaterialByName(pAnimCont->name, bRecursive);

            if (pFindMat) {
                mAnimLinkAry[i].SetAnimTransform(this, i);
                pFindMat->AddAnimationLink(&mAnimLinkAry[i]);
            }
        }
    }
}

void AnimTransformBasic::Bind(Material* pMaterial) {
    const u32* animContOffsets = detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset);

    for (u16 i = 0; i < mpRes->animContNum; i++) {
        const res::AnimationContent* pAnimCont = detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, animContOffsets[i]);

        if (pAnimCont->type != res::AnimationContent::ANIMTYPE_MATERIAL) {
            continue;
        }

        if (detail::EqualsMaterialName(pMaterial->GetName(), pAnimCont->name)) {
            mAnimLinkAry[i].SetAnimTransform(this, i);
            pMaterial->AddAnimationLink(&mAnimLinkAry[i]);
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Pane* pPane) {
    u32 animContOffset = detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset)[idx];
    const res::AnimationContent* pAnimCont = detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, animContOffset);
    const u32* animInfoOffsets = detail::ConvertOffsToPtr<u32>(pAnimCont, sizeof(*pAnimCont));

    for (int i = 0; i < pAnimCont->num; i++) {
        const res::AnimationInfo* pAnimInfo = detail::ConvertOffsToPtr<res::AnimationInfo>(pAnimCont, animInfoOffsets[i]);
        const u32* animTargetOffsets = detail::ConvertOffsToPtr<u32>(pAnimInfo, sizeof(*pAnimInfo));

        switch (pAnimInfo->kind) {
        case res::AnimationInfo::SIGNATURE_ANMPANESRT:
            AnimatePaneSRT(pPane, pAnimInfo, animTargetOffsets, mFrame);
            break;
        case res::AnimationInfo::SIGNATURE_ANMPANEVIS:
            AnimateVisibility(pPane, pAnimInfo, animTargetOffsets, mFrame);
            break;
        case res::AnimationInfo::SIGNATURE_ANMVTXCLR:
            AnimateVertexColor(pPane, pAnimInfo, animTargetOffsets, mFrame);
            break;
        }
    }
}

void AnimTransformBasic::Animate(u32 idx, Material* pMaterial) {
    u32 animContOffset = detail::ConvertOffsToPtr<u32>(mpRes, mpRes->animContOffsetsOffset)[idx];
    const res::AnimationContent* pAnimCont = detail::ConvertOffsToPtr<res::AnimationContent>(mpRes, animContOffset);
    const u32* animInfoOffsets = detail::ConvertOffsToPtr<u32>(pAnimCont, sizeof(*pAnimCont));

    for (int i = 0; i < pAnimCont->num; i++) {
        const res::AnimationInfo* pAnimInfo = detail::ConvertOffsToPtr<res::AnimationInfo>(pAnimCont, animInfoOffsets[i]);
        const u32* animTargetOffsets = detail::ConvertOffsToPtr<u32>(pAnimInfo, sizeof(*pAnimInfo));

        switch (pAnimInfo->kind) {
        case res::AnimationInfo::SIGNATURE_ANMMATCLR:
            AnimateMaterialColor(pMaterial, pAnimInfo, animTargetOffsets, mFrame);
            break;
        case res::AnimationInfo::SIGNATURE_ANMTEXSRT:
            AnimateTextureSRT(pMaterial, pAnimInfo, animTargetOffsets, mFrame);
            break;
        case res::AnimationInfo::SIGNATURE_ANMTEXPAT:
            if (mpFileResAry) {
                AnimateTexturePattern(pMaterial, pAnimInfo, animTargetOffsets, mFrame, mpFileResAry);
            }
            break;
        case res::AnimationInfo::SIGNATURE_ANMINDTEXSRT:
            AnimateIndTexSRT(pMaterial, pAnimInfo, animTargetOffsets, mFrame);
            break;
        }
    }
}

AnimationLink* detail::FindAnimationLink(AnimationLinkList* pAnimList, AnimTransform* pAnimTrans) {
    for (AnimationLinkList::Iterator it = pAnimList->GetBeginIter(); it != pAnimList->GetEndIter(); ++it) {
        if (pAnimTrans == it->GetAnimTransform()) {
            return &*it;
        }
    }

    return nullptr;
}

}
}
