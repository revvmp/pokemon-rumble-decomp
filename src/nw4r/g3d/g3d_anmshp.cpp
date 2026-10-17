#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/g3d/g3d_anmshp.h>
namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(AnmObjShp);
NW4R_G3D_RTTI_DEF(AnmObjShpNode);

NW4R_G3D_RTTI_DEF(AnmObjShpRes);

AnmObjShp::AnmObjShp(MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding)
    : AnmObj(pAllocator, nullptr), mNumBinding(numBinding), mpBinding(pBindingBuf) {
    Release();
}

bool AnmObjShp::TestExistence(u32 idx) const {
    return !(mpBinding[idx] & (BINDING_UNDEFINED | BINDING_INVALID));
}

bool AnmObjShp::TestDefined(u32 idx) const {
    return !(mpBinding[idx] & BINDING_UNDEFINED);
}

void AnmObjShp::Release() {
    for (int i = 0; i < mNumBinding; i++) {
        mpBinding[i] = BINDING_UNDEFINED;
    }

    SetAnmFlag(FLAG_ANM_BOUND, false);
}

AnmObjShpRes *AnmObjShp::Attach(int idx, AnmObjShpRes *pRes) {
#pragma unused(idx)
#pragma unused(pRes)

    return nullptr;
}

AnmObjShpRes *AnmObjShp::Detach(int idx) {
#pragma unused(idx)

    return nullptr;
}

void AnmObjShp::SetWeight(int idx, f32 weight) {
#pragma unused(idx)
#pragma unused(weight)
}

f32 AnmObjShp::GetWeight(int idx) const {
#pragma unused(idx)

    return 0.0f;
}

void AnmObjShp::DetachAll() {}

AnmObjShpNode::AnmObjShpNode(
    MEMAllocator *pAllocator, u16 *pBindingBuf, int numBinding, AnmObjShpRes **ppChildrenBuf, int numChildren
)
    : AnmObjShp(pAllocator, pBindingBuf, numBinding), mChildrenArraySize(numChildren), mpChildrenArray(ppChildrenBuf) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        mpChildrenArray[i] = nullptr;
    }
}

AnmObjShpNode::~AnmObjShpNode() {
    DetachAll();
}

AnmObjShpRes *AnmObjShpNode::Attach(int idx, AnmObjShpRes *pRes) {
    AnmObjShpRes *pOld = Detach(idx);
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

AnmObjShpRes *AnmObjShpNode::Detach(int idx) {
    AnmObjShpRes *pOld = mpChildrenArray[idx];

    if (pOld != nullptr) {
        pOld->G3dProc(G3DPROC_DETACH_PARENT, 0, this);
        mpChildrenArray[idx] = nullptr;

        bool hasAnm = false;
        for (u32 i = 0; i < mNumBinding; i++) {
            u16 binding = BINDING_UNDEFINED;

            for (int j = 0; j < mChildrenArraySize; j++) {
                AnmObjShpRes *pChild = mpChildrenArray[j];

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

void AnmObjShpNode::DetachAll() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        Detach(i);
    }
}

void AnmObjShpNode::UpdateFrame() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->UpdateFrame();
        }
    }
}

void AnmObjShpNode::SetFrame(f32 frame) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->SetFrame(frame);
        }
    }
}

f32 AnmObjShpNode::GetFrame() const {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            return mpChildrenArray[i]->GetFrame();
        }
    }

    return 0.0f;
}

void AnmObjShpNode::SetUpdateRate(f32 rate) {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->SetUpdateRate(rate);
        }
    }
}

f32 AnmObjShpNode::GetUpdateRate() const {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            return mpChildrenArray[i]->GetUpdateRate();
        }
    }

    return 1.0f;
}

bool AnmObjShpNode::Bind(const ResMdl mdl) {
    bool success = false;

    for (int i = 0; i < mChildrenArraySize; i++) {
        AnmObjShpRes *pChild = mpChildrenArray[i];
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

void AnmObjShpNode::Release() {
    for (int i = 0; i < mChildrenArraySize; i++) {
        if (mpChildrenArray[i] != nullptr) {
            mpChildrenArray[i]->Release();
        }
    }

    AnmObjShp::Release();
}

void AnmObjShpNode::G3dProc(u32 task, u32 param, void *pInfo) {
#pragma unused(param)

    switch (task) {
        case G3DPROC_CHILD_DETACHED: {
            for (int i = 0; i < mChildrenArraySize; i++) {
                if (mpChildrenArray[i] == pInfo) {
                    Detach(i);
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

AnmObjShpRes *AnmObjShpRes::Construct(MEMAllocator *pAllocator, u32 *pSize, ResAnmShp shp, ResMdl mdl, bool cache) {
    if (!shp.IsValid() || !mdl.IsValid()) {
        return nullptr;
    }

    int animNum = shp.GetShapeAnmNumEntries();
    int vtxNum = mdl.GetResVtxPosNumEntries();
    int bindNum = vtxNum;
    int vtxSetNum = shp.GetNumVtxNames();
    int cacheNum = cache ? animNum : 0;

    u32 bindSize = bindNum * sizeof(u16);
    u32 vtxSetSize = vtxSetNum * sizeof(ShpAnmVtxSet);
    u32 cacheSize = cacheNum * sizeof(ShpAnmResult);

    u32 bindOfs = ut::RoundUp<u32>(sizeof(AnmObjShpRes), 2);
    u32 vtxSetOfs = ut::RoundUp<u32>(bindOfs + bindSize, 4);
    u32 cacheOfs = ut::RoundUp<u32>(vtxSetOfs + vtxSetSize, 4);

    u32 size = ut::RoundUp<u32>(cacheOfs + cacheSize, 4);
    if (pSize != nullptr) {
        *pSize = size;
    }

    if (pAllocator == nullptr) {
        return nullptr;
    }

    u8 *pBuffer = reinterpret_cast<u8 *>(Alloc(pAllocator, size));
    if (pBuffer == nullptr) {
        return nullptr;
    }

    u16 *const pBindingBuf = reinterpret_cast<u16 *>(pBuffer + bindOfs);

    ShpAnmVtxSet *const pVtxSetBuf = reinterpret_cast<ShpAnmVtxSet *>(pBuffer + vtxSetOfs);

    ShpAnmResult *const pCacheBuf = reinterpret_cast<ShpAnmResult *>(pBuffer + cacheOfs);

    return new (pBuffer)
        AnmObjShpRes(pAllocator, shp, pBindingBuf, pVtxSetBuf, bindNum, cacheSize != 0 ? pCacheBuf : nullptr);
}

AnmObjShpRes::AnmObjShpRes(
    MEMAllocator *pAllocator, ResAnmShp shp, u16 *pBindingBuf, ShpAnmVtxSet *pVtxSetBuf, int numBinding,
    ShpAnmResult *pCacheBuf
)
    : AnmObjShp(pAllocator, pBindingBuf, numBinding),
      FrameCtrl(0.0f, shp.GetNumFrame(), GetAnmPlayPolicy(shp.GetAnmPolicy())),
      mRes(shp),
      mpVtxSetArray(pVtxSetBuf),
      mpResultCache(pCacheBuf) {
    if (mpResultCache != nullptr) {
        UpdateCache();
    }
}

void AnmObjShpRes::SetFrame(f32 frame) {
    SetFrm(frame);

    if (mpResultCache != nullptr) {
        UpdateCache();
    }
}

f32 AnmObjShpRes::GetFrame() const {
    return GetFrm();
}

void AnmObjShpRes::SetUpdateRate(f32 rate) {
    SetRate(rate);

    if (rate == 0.f && mpResultCache != nullptr) {
        UpdateCache();
    }
}

f32 AnmObjShpRes::GetUpdateRate() const {
    return GetRate();
}

void AnmObjShpRes::UpdateFrame() {
    if (GetRate() != 0.f) {
        UpdateFrm();

        if (mpResultCache != nullptr) {
            UpdateCache();
        }
    }
}

bool AnmObjShpRes::Bind(const ResMdl mdl) {
    bool success = false;
    u32 numAnim = mRes.GetShapeAnmNumEntries();

    for (u16 i = 0; i < numAnim; i++) {
        const ResAnmShpAnmData *pData = mRes.GetShapeAnm(i);

        ResName name(ut::AddOffsetToPtr(pData, pData->name - 4));

        ResVtxPos pos = mdl.GetResVtxPos(name);
        if (!pos.IsValid()) {
            continue;
        }

        mpBinding[pos.GetID()] = i;
        success = true;
    }

    int nameNum = mRes.GetNumVtxNames();
    const s32 *pVtxNameArray = mRes.GetVtxNameArray();

    for (int i = 0; i < nameNum; i++) {
        ShpAnmVtxSet &rSet = mpVtxSetArray[i];
        s32 offset = pVtxNameArray[i];

        ResName name = NW4R_G3D_OFS_TO_RESNAME(pVtxNameArray, offset);

        rSet.resVtxPos = mdl.GetResVtxPos(name);
        rSet.resVtxNrm = mdl.GetResVtxNrm(name);
        rSet.resVtxClr = mdl.GetResVtxClr(name);
    }

    SetAnmFlag(FLAG_ANM_BOUND, true);
    return success;
}

const ShpAnmResult *AnmObjShpRes::GetResult(ShpAnmResult *pResult, u32 idx) {
    u32 id = mpBinding[idx];

    if (id & (BINDING_UNDEFINED | BINDING_INVALID)) {
        pResult->flags = 0;
        return pResult;
    }

    if (mpResultCache != nullptr) {
        return &mpResultCache[id];
    }

    mRes.GetAnmResult(pResult, id, GetFrm(), mpVtxSetArray);
    return pResult;
}

void AnmObjShpRes::UpdateCache() {
    f32 frame = GetFrm();

    for (u32 i = 0; i < mNumBinding; i++) {
        u16 bind = mpBinding[i];

        if (!(bind & BINDING_UNDEFINED)) {
            u32 id = bind & BINDING_ID_MASK;
            mRes.GetAnmResult(&mpResultCache[id], id, frame, mpVtxSetArray);
        }
    }
}

void AnmObjShpRes::G3dProc(u32 task, u32 param, void *pInfo) {
#pragma unused(param)

    switch (task) {
        case G3DPROC_UPDATEFRAME: {
            UpdateFrame();
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

}
}
