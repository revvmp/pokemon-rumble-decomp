#include <nw4r/g3d/g3d_scngroupex.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/g3d_draw.h>
#include <nw4r/g3d/g3d_draw1mat1shp.h>
#include <new>

namespace nw4r {
namespace g3d {

NW4R_G3D_RTTI_DEF(ScnGroupEx);

namespace detail {

IScnObjGather::CheckStatus GatherProxy::CheckScnObj(ScnObj*) {
    return CHECKSTATUS_GATHER_SCNOBJ;
}


IScnObjGather::CullingStatus GatherProxy::Add(ScnObj* pObj, bool opa, bool xlu) {
    if (mpInner->CheckScnObj(pObj) == CHECKSTATUS_GATHER_SCNOBJ) {
        return mpInner->Add(pObj, opa, xlu);
    }

    ModelDrawGather* pGather = mpInner;
    if (pGather->MatchesGroup(pObj)) {
        return CULLINGSTATUS_INTERSECT;
    }
    return mpOuter->Add(pObj, opa, xlu);
}

void ModelDrawGather::DrawOpa(ResMdlDrawMode* pForceMode) {
    if (mpDrawOpa != nullptr) {
        Draw(mpDrawOpa, mNumScnObjOpa, mpArrayOpa, pForceMode, true);
    }
}

void ModelDrawGather::DrawXlu(ResMdlDrawMode* pForceMode) {
    if (mpDrawXlu != nullptr) {
        Draw(mpDrawXlu, mNumScnObjXlu, mpArrayXlu, pForceMode, false);
    }
}

IScnObjGather::CheckStatus ModelDrawGather::CheckScnObj(ScnObj* pObj) {
    ScnMdlSimple* pModel = G3dObj::DynamicCast<ScnMdlSimple>(pObj);
    return pModel != nullptr && pModel->GetResMdl() == GetResMdl() &&
                   pModel->GetAnmObj(ScnMdlSimple::ANMOBJTYPE_MATCLR) == nullptr &&
                   pModel->GetAnmObj(ScnMdlSimple::ANMOBJTYPE_TEXPAT) == nullptr &&
                   pModel->GetAnmObj(ScnMdlSimple::ANMOBJTYPE_TEXSRT) == nullptr &&
                   pModel->GetAnmObj(ScnMdlSimple::ANMOBJTYPE_SHP) == nullptr
               ? CHECKSTATUS_GATHER_SCNOBJ : CHECKSTATUS_IGNORE_SCNOBJ;
}

ResMdl ModelDrawGather::GetResMdl() const {
    return mResMdl;
}

void ModelDrawGather::Draw(const u8* pCode, u32 count, ScnObj** ppObjects,
                           ResMdlDrawMode* pForceMode, bool opa) {
    ResMat previousMaterial;
    bool ignoreMaterial = pForceMode != nullptr && (*pForceMode & RESMDL_DRAWMODE_IGNORE_MATERIAL);
    while (*pCode != 1) {
        ResMat material = GetResMdl().GetResMat((pCode[1] << 8) | pCode[2]);
        ResShp shape = GetResMdl().GetResShp((pCode[3] << 8) | pCode[4]);
        ResNode node = GetResMdl().GetResNode((pCode[5] << 8) | pCode[6]);
        bool first = true;
        for (u32 i = 0; i < count; i++) {
            u32 disabled;
            ppObjects[i]->GetScnObjOption(opa ? ScnObj::OPTION_DISABLE_DRAW_OPA : ScnObj::OPTION_DISABLE_DRAW_XLU, &disabled);
            if (disabled != 0) {
                continue;
            }
            u32 control = DRAW1MAT1SHP_CTRL_NOPPCSYNC;
            if (pForceMode != nullptr && (*pForceMode & RESMDL_DRAWMODE_FORCE_LIGHTOFF)) {
                control |= DRAW1MAT1SHP_CTRL_FORCE_LIGHTOFF;
            }
            ScnMdl* pModel = G3dObj::DynamicCast<ScnMdl>(ppObjects[i]);
            if (pModel != nullptr) {
                if (ScnMdl::CopiedVisAccess(pModel, node.GetID()).IsVisible()) {
                    G3DState::SetViewPosNrmMtxArray(pModel->GetViewPosMtxArray(), pModel->GetViewNrmMtxArray(), pModel->GetViewTexMtxArray());
                    if (pForceMode == nullptr && (pModel->GetDrawMode() & RESMDL_DRAWMODE_FORCE_LIGHTOFF)) {
                        control |= DRAW1MAT1SHP_CTRL_FORCE_LIGHTOFF;
                    }
                    Draw1Mat1ShpDirectly(ignoreMaterial || !first || material == previousMaterial ? ResMat() : material,
                                         shape, nullptr, nullptr, control, nullptr, detail::GetIndMtxOp(material, node, shape));
                }
            } else if (node.IsVisible()) {
                ScnMdlSimple* pSimple = G3dObj::DynamicCast<ScnMdlSimple>(ppObjects[i]);
                G3DState::SetViewPosNrmMtxArray(pSimple->GetViewPosMtxArray(), pSimple->GetViewNrmMtxArray(), pSimple->GetViewTexMtxArray());
                if (pForceMode == nullptr && (pSimple->GetDrawMode() & RESMDL_DRAWMODE_FORCE_LIGHTOFF)) {
                    control |= DRAW1MAT1SHP_CTRL_FORCE_LIGHTOFF;
                }
                Draw1Mat1ShpDirectly(ignoreMaterial || !first || material == previousMaterial ? ResMat() : material,
                                     shape, nullptr, nullptr, control, nullptr, detail::GetIndMtxOp(material, node, shape));
            }
            first = false;
        }
        previousMaterial = material;
        pCode += 8;
    }
}

bool ModelDrawGather::IsSameResource(const ModelDrawGather* pOther) const {
    if (GetSignature() == pOther->GetSignature() && pOther->mResMdl.ptr() == mResMdl.ptr()) {
        return true;
    }
    return false;
}

u32 ModelDrawGather::GetSignature() const {
    return 'SGEX';
}

ModelDrawGather::ModelDrawGather(ScnObj** pOpa, ScnObj** pXlu, u32 capacity, ResMdl mdl)
    : ScnObjGather(pOpa, pXlu, capacity), mResMdl(mdl),
      mpDrawOpa(mdl.IsValid() ? mdl.GetResByteCode("DrawOpa") : nullptr),
      mpDrawXlu(mdl.IsValid() ? mdl.GetResByteCode("DrawXlu") : nullptr) {}

}

ScnGroupEx* ScnGroupEx::Construct(MEMAllocator* pAllocator, u32* pSize,
                                  u32 maxChildren, u32 maxModels, ResMdl mdl) {
    u32 objectSize = align4(sizeof(ScnGroupEx) + maxChildren * sizeof(ScnObj*));
    u32 arraySize = maxModels * sizeof(ScnObj*);
    ScnGroupEx* pGroup = nullptr;
    u32 gatherOffset = align4(objectSize + arraySize * 2);
    u32 size = align4(gatherOffset + sizeof(detail::ModelDrawGather));
    if (pSize != nullptr) {
        *pSize = size;
    }
    if (pAllocator != nullptr) {
        u8* pBuffer = static_cast<u8*>(Alloc(pAllocator, size));
        if (pBuffer == nullptr) {
            return nullptr;
        }
        ScnObj** ppOpa = reinterpret_cast<ScnObj**>(pBuffer + objectSize);
        detail::ModelDrawGather* pGather = new (pBuffer + gatherOffset) detail::ModelDrawGather(
            ppOpa, ppOpa + maxModels, maxModels, mdl);
        pGroup = new (pBuffer) ScnGroupEx(pAllocator, reinterpret_cast<ScnObj**>(pBuffer + sizeof(ScnGroupEx)), maxChildren, pGather);
    }
    return pGroup;
}

namespace detail {
GatherProxy::~GatherProxy() {}
}

void ScnGroupEx::G3dProc(u32 task, u32 param, void* pInfo) {
    if (IsG3dProcDisabled(task)) {
        return;
    }
    switch (task) {
    case G3DPROC_GATHER_SCNOBJ: {
        IScnObjGather* pCollection = static_cast<IScnObjGather*>(pInfo);
        pCollection->Add(this, !TestScnObjFlag(SCNOBJFLAG_NOT_GATHER_DRAW_OPA), !TestScnObjFlag(SCNOBJFLAG_NOT_GATHER_DRAW_XLU));
        mpGather->Clear();
        detail::GatherProxy proxy(pCollection, mpGather);
        for (u32 i = 0; i < Size(); i++) {
            Begin()[i]->G3dProc(task, param, &proxy);
        }
        break;
    }
    case G3DPROC_ZSORT:
        switch (param) {
        case 0:
            mpGather->ZSort();
            break;
        case 1:
            mpGather->Sort();
            break;
        case 2: {
            IScnObjGather::LessThanFunc* pComparators = static_cast<IScnObjGather::LessThanFunc**>(pInfo)[1];
            mpGather->Sort(pComparators[0], pComparators[1]);
            break;
        }
        }
        DefG3dProcScnGroup(task, param, pInfo);
        break;
    case G3DPROC_DRAW_OPA:
        CheckCallback_DRAW_OPA(CALLBACK_TIMING_A, param, pInfo);
        mpGather->DrawOpa(static_cast<ResMdlDrawMode*>(pInfo));
        CheckCallback_DRAW_OPA(CALLBACK_TIMING_C, param, pInfo);
        break;
    case G3DPROC_DRAW_XLU:
        CheckCallback_DRAW_XLU(CALLBACK_TIMING_A, param, pInfo);
        mpGather->DrawXlu(static_cast<ResMdlDrawMode*>(pInfo));
        CheckCallback_DRAW_XLU(CALLBACK_TIMING_C, param, pInfo);
        break;
    default:
        DefG3dProcScnGroup(task, param, pInfo);
        break;
    }
}

ScnGroupEx::~ScnGroupEx() {}

}
}
