#include <nw4r/g3d/g3d_anmchrext.h>
#include <nw4r/g3d/res/g3d_resnode.h>
namespace nw4r { namespace g3d {
NW4R_G3D_RTTI_DEF(AnmObjChrGroup);

AnmObjChrGroup::AnmObjChrGroup(MEMAllocator* pAllocator, ResMdl mdl,
                             u16* pBindingBuf, int numBinding,
                             AnmObjChr** ppChildrenBuf, int numChildren)
    : AnmObjChr(pAllocator, pBindingBuf, numBinding), mResMdl(mdl),
      mChildrenArraySize(numChildren), mpChildrenArray(ppChildrenBuf) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        mpChildrenArray[i] = nullptr;
    }
    SetRootScale(math::VEC3(1.0f, 1.0f, 1.0f));
}

AnmObjChrGroup::~AnmObjChrGroup() {
    DetachAllChildren();
}

AnmObjChr *AnmObjChrGroup::AttachChild(int idx, AnmObjChr *pRes) {
    AnmObjChr *pOld = DetachChild(idx);
    bool hasAnm = false;

    for (u32 i = 0; i < mNumBinding; i++) {
        if (!pRes->TestDefined(i)) {
            continue;
        }

        hasAnm = true;
        mpBinding[i] = 0;
    }

    if (hasAnm) {
        SetAnmFlag(FLAG_ANM_BOUND, true);
    }

    mpChildrenArray[idx] = pRes;
    pRes->G3dProc(G3DPROC_ATTACH_PARENT, 0, this);
    return pOld;
}

AnmObjChr *AnmObjChrGroup::DetachChild(int idx) {
    AnmObjChr *pOld = mpChildrenArray[idx];

    if (pOld != nullptr) {
        pOld->G3dProc(G3DPROC_DETACH_PARENT, 0, this);
        mpChildrenArray[idx] = nullptr;

        bool hasAnm = false;
        for (u32 i = 0; i < mNumBinding; i++) {
            u16 binding = BINDING_UNDEFINED;

            for (int j = 0; j < mChildrenArraySize; j++) {
                AnmObjChr *pChild = mpChildrenArray[j];

                if (pChild == nullptr || !pChild->TestDefined(i)) {
                    continue;
                }

                hasAnm = true;
                binding = 0;
                break;
            }

            mpBinding[i] = binding;
        }

        if (!hasAnm) {
            SetAnmFlag(FLAG_ANM_BOUND, false);
        }
    }

    return pOld;
}

void AnmObjChrGroup::DetachAllChildren() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        DetachChild(i);
    }
}

void AnmObjChrGroup::UpdateFrame() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->UpdateFrame();
        }
    }
}

void AnmObjChrGroup::SetFrame(f32 frame) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->SetFrame(frame);
        }
    }
}

f32 AnmObjChrGroup::GetFrame() const {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            return mpChildrenArray[i]->GetFrame();
        }
    }

    return 0.0f;
}

void AnmObjChrGroup::SetUpdateRate(f32 rate) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->SetUpdateRate(rate);
        }
    }
}

f32 AnmObjChrGroup::GetUpdateRate() const {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            return mpChildrenArray[i]->GetUpdateRate();
        }
    }

    return 1.0f;
}

bool AnmObjChrGroup::Bind(const ResMdl mdl) {
    bool success = false;

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChr *pChild = mpChildrenArray[i];
        if (pChild == nullptr) {
            continue;
        }

        bool childSuccess = pChild->Bind(mdl);
        success = success || childSuccess;

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

bool AnmObjChrGroup::Bind(const ResMdl mdl, u32 target, BindOption option) {
    bool success = false;

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChr *pChild = mpChildrenArray[i];
        if (pChild == nullptr) {
            continue;
        }

        bool childSuccess = pChild->Bind(mdl, target, option);
        success = success || childSuccess;

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

void AnmObjChrGroup::Release() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->Release();
        }
    }

    AnmObjChr::Release();
}

void AnmObjChrGroup::Release(const ResMdl mdl, u32 target, BindOption option) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChr *pChild = mpChildrenArray[i];

        if (pChild != nullptr) {
            pChild->Release(mdl, target, option);
        }
    }

    AnmObjChr::Release();

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjChr *pChild = mpChildrenArray[i];
        if (pChild == nullptr) {
            continue;
        }

        for (u32 j = 0; j < mNumBinding; j++) {
            if (pChild->TestDefined(j)) {
                mpBinding[j] = 0;
            }
        }
    }
}

void AnmObjChrGroup::G3dProc(u32 task, u32 param, void *pInfo) {
#pragma unused(param)

    switch (task) {
        case G3DPROC_CHILD_DETACHED: {
            for (int i = 0; i < mChildrenArraySize; i++) {
                if (mpChildrenArray[i] == pInfo) {
                    DetachChild(i);
                    return;
                }
            }

            break;
        }

        case G3DPROC_DETACH_PARENT: {
            SetParent(nullptr);
            break;
        }

        case G3DPROC_ATTACH_PARENT: {
            SetParent(static_cast<G3dObj *>(pInfo));
            break;
        }
    }
}


void AnmObjChrGroup::SetRootScale(const math::VEC3& rScale) {
    mRootScale = rScale;
    mRootScaleFlags = 0;
    if (mRootScale.x == 1.0f && mRootScale.y == 1.0f && mRootScale.z == 1.0f) {
        mRootScaleFlags |= ChrAnmResult::FLAG_SCALE_ONE;
    }
    if (mRootScale.x == mRootScale.y && mRootScale.y == mRootScale.z) {
        mRootScaleFlags |= ChrAnmResult::FLAG_SCALE_UNIFORM;
    }
}

const ChrAnmResult* AnmObjChrGroup::ApplyRootScale(ChrAnmResult* pResult, u32 idx) const {
    if (idx == 0) {
        if (pResult->flags & ChrAnmResult::FLAG_ANM_EXISTS) {
            if (mRootScaleFlags != (ChrAnmResult::FLAG_SCALE_ONE | ChrAnmResult::FLAG_SCALE_UNIFORM)) {
                if (mRootScaleFlags == ChrAnmResult::FLAG_SCALE_UNIFORM) {
                    pResult->flags &= ~(ChrAnmResult::FLAG_MTX_IDENT | ChrAnmResult::FLAG_SCALE_ONE);
                } else {
                    pResult->flags &= ~(ChrAnmResult::FLAG_MTX_IDENT | ChrAnmResult::FLAG_SCALE_ONE |
                                        ChrAnmResult::FLAG_SCALE_UNIFORM);
                }
                pResult->s.x *= mRootScale.x;
                pResult->s.y *= mRootScale.y;
                pResult->s.z *= mRootScale.z;
            }
        } else {
            pResult->flags = mRootScaleFlags | ChrAnmResult::FLAG_ANM_EXISTS |
                             ChrAnmResult::FLAG_PATCH_ROT | ChrAnmResult::FLAG_PATCH_TRANS;
            pResult->s = mRootScale;
            mResMdl.GetResNode(0).PatchChrAnmResult(pResult);
        }
    }
    return pResult;
}

} }
